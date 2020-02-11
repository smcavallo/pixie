#include "src/carnot/compiler/ast_visitor.h"

#include "src/carnot/compiler/compiler_error_context/compiler_error_context.h"
#include "src/carnot/compiler/ir/pattern_match.h"
#include "src/carnot/compiler/objects/collection_object.h"
#include "src/carnot/compiler/objects/expr_object.h"
#include "src/carnot/compiler/objects/none_object.h"
#include "src/carnot/compiler/objects/pixie_module.h"
#include "src/carnot/compiler/objects/type_object.h"
#include "src/carnot/compiler/parser/parser.h"

namespace pl {
namespace carnot {
namespace compiler {
using pypa::AstType;

StatusOr<FuncIR::Op> ASTVisitorImpl::GetOp(const std::string& python_op, const pypa::AstPtr node) {
  auto op_find = FuncIR::op_map.find(python_op);
  if (op_find == FuncIR::op_map.end()) {
    return CreateAstError(node, "Operator '$0' not handled", python_op);
  }
  return op_find->second;
}

StatusOr<FuncIR::Op> ASTVisitorImpl::GetUnaryOp(const std::string& python_op,
                                                const pypa::AstPtr node) {
  auto op_find = FuncIR::unary_op_map.find(python_op);
  if (op_find == FuncIR::op_map.end()) {
    return CreateAstError(node, "Unary Operator '$0' not handled", python_op);
  }
  return op_find->second;
}

StatusOr<std::shared_ptr<ASTVisitorImpl>> ASTVisitorImpl::Create(IR* graph,
                                                                 CompilerState* compiler_state,
                                                                 const FlagValues& flag_values) {
  std::shared_ptr<ASTVisitorImpl> ast_visitor = std::shared_ptr<ASTVisitorImpl>(
      new ASTVisitorImpl(graph, compiler_state, VarTable::Create()));
  PL_RETURN_IF_ERROR(ast_visitor->InitGlobals(flag_values));
  return ast_visitor;
}

std::shared_ptr<ASTVisitorImpl> ASTVisitorImpl::CreateChild() {
  return std::shared_ptr<ASTVisitorImpl>(
      new ASTVisitorImpl(ir_graph_, compiler_state_, var_table_->CreateChild()));
}

Status ASTVisitorImpl::InitGlobals(const FlagValues& flag_values) {
  PL_ASSIGN_OR_RETURN(pixie_module_, PixieModule::Create(ir_graph_, compiler_state_, flag_values));
  // TODO(philkuz) verify this is done before hand in a parent var table if one exists.
  var_table_->Add(PixieModule::kPixieModuleObjName, pixie_module_);
  // Populate the type objects
  PL_ASSIGN_OR_RETURN(auto string_type_object, TypeObject::Create(IRNodeType::kString));
  var_table_->Add(ASTVisitorImpl::kStringTypeName, string_type_object);
  PL_ASSIGN_OR_RETURN(auto int_type_object, TypeObject::Create(IRNodeType::kInt));
  var_table_->Add(ASTVisitorImpl::kIntTypeName, int_type_object);
  // Populate other reserved words
  var_table_->Add(ASTVisitorImpl::kNoneName, std::make_shared<NoneObject>());

  return Status::OK();
}

Status ASTVisitorImpl::ProcessExprStmtNode(const pypa::AstExpressionStatementPtr& e) {
  OperatorContext op_context({}, "", {});
  return Process(e->expr, op_context).status();
}

StatusOr<QLObjectPtr> ASTVisitorImpl::CallFunc(const pypa::AstPtr& ast, QLObjectPtr ql_object) {
  // TODO(philkuz) refactor ArgMap/ParsedArgs to push the function calls to the handler instead
  // of this hack that only works for pl modules.
  QLObjectType attr_object_type = ql_object->type_descriptor().type();
  if (attr_object_type != QLObjectType::kFunction) {
    return CreateAstError(ast, "does not return a usable value");
  }

  PL_ASSIGN_OR_RETURN(ql_object,
                      std::static_pointer_cast<FuncObject>(ql_object)->Call({}, ast, this));
  if (!ql_object->HasNode()) {
    return CreateAstError(ast, "does not return a usable value");
  }
  return ql_object;
}

StatusOr<QLObjectPtr> ASTVisitorImpl::ProcessSingleExpressionModule(
    const pypa::AstModulePtr& module) {
  OperatorContext op_context({}, "");
  const std::vector<pypa::AstStmt>& items_list = module->body->items;
  if (items_list.size() != 1) {
    return CreateAstError(module,
                          "ProcessModuleExpression only works for single lined statements.");
  }
  const pypa::AstStmt& stmt = items_list[0];
  switch (stmt->type) {
    case pypa::AstType::ExpressionStatement: {
      // TODO(nserrino): Replace with Process() when PL-1431 happens.
      PL_ASSIGN_OR_RETURN(auto ql_object,
                          Process(PYPA_PTR_CAST(ExpressionStatement, stmt)->expr, op_context));
      if (ql_object->type() == QLObjectType::kFunction && !ql_object->HasNode()) {
        return CallFunc(stmt, ql_object);
      }
      return ql_object;
    }
    default: {
      return CreateAstError(module, "Want expression, got $0", GetAstTypeName(stmt->type));
    }
  }
}

StatusOr<QLObjectPtr> ASTVisitorImpl::ParseAndProcessSingleExpression(
    std::string_view single_expr_str) {
  Parser parser;
  PL_ASSIGN_OR_RETURN(pypa::AstModulePtr ast, parser.Parse(single_expr_str.data()));
  return ProcessSingleExpressionModule(ast);
}

Status ASTVisitorImpl::ProcessModuleNode(const pypa::AstModulePtr& m) {
  return ProcessASTSuite(m->body, /*is_function_definition_body*/ false).status();
}

StatusOr<plannerpb::QueryFlagsSpec> ASTVisitorImpl::GetAvailableFlags(const pypa::AstModulePtr& m) {
  PL_RETURN_IF_ERROR(ProcessModuleNode(m));
  auto flags = pixie_module_->flags_object();
  return flags->GetAvailableFlags(m);
}

StatusOr<QLObjectPtr> ASTVisitorImpl::ProcessASTSuite(const pypa::AstSuitePtr& body,
                                                      bool is_function_definition_body) {
  pypa::AstStmtList items_list = body->items;
  if (items_list.size() == 0) {
    return CreateAstError(body, "No runnable code found");
  }
  // iterate through all the items on this list.
  for (pypa::AstStmt stmt : items_list) {
    switch (stmt->type) {
      case pypa::AstType::ExpressionStatement: {
        PL_RETURN_IF_ERROR(ProcessExprStmtNode(PYPA_PTR_CAST(ExpressionStatement, stmt)));
        break;
      }
      case pypa::AstType::Assign: {
        PL_RETURN_IF_ERROR(ProcessAssignNode(PYPA_PTR_CAST(Assign, stmt)));
        break;
      }
      case pypa::AstType::FunctionDef: {
        PL_RETURN_IF_ERROR(ProcessFunctionDefNode(PYPA_PTR_CAST(FunctionDef, stmt)));
        break;
      }
      case pypa::AstType::Return: {
        // If we are not parsing a function definition's body then we must error.
        if (!is_function_definition_body) {
          return CreateAstError(stmt, "'return' outside function");
        }
        // We exit early if the function definition return is used.
        return ProcessFuncDefReturn(PYPA_PTR_CAST(Return, stmt));
      }
      default: {
        return CreateAstError(stmt, "Can't parse expression of type $0",
                              GetAstTypeName(stmt->type));
      }
    }
  }
  // If we reach the end of the stmt list before hitting a return, return a NoneObject.
  return std::static_pointer_cast<QLObject>(std::make_shared<NoneObject>(body));
}

// Assignment by subscript is more restrictive than assignment by attribute.
// Subscript assignment is currently only valid for creating map expressions such as the following:
// df['foo'] = 1+2
Status ASTVisitorImpl::ProcessSubscriptAssignment(const pypa::AstSubscriptPtr& subscript,
                                                  const pypa::AstPtr& expr_node) {
  PL_ASSIGN_OR_RETURN(auto processed_node, ProcessData(subscript, {{}, "", {}}));
  PL_ASSIGN_OR_RETURN(auto processed_target_table, Process(subscript->value, {{}, "", {}}));

  if (processed_target_table->type() != QLObjectType::kDataframe) {
    return CreateAstError(subscript, "Can't assign to node via subscript of type $0",
                          processed_target_table->name());
  }

  return ProcessMapAssignment(subscript->value,
                              std::static_pointer_cast<Dataframe>(processed_target_table),
                              processed_node, expr_node);
}

// Assignment by attribute supports all of the cases that subscript assignment does,
// in addition to assigning to
Status ASTVisitorImpl::ProcessAttributeAssignment(const pypa::AstAttributePtr& attr,
                                                  const pypa::AstPtr& expr_node) {
  PL_ASSIGN_OR_RETURN(auto processed_target, Process(attr->value, {{}, "", {}}));

  if (processed_target->type() != QLObjectType::kDataframe) {
    PL_ASSIGN_OR_RETURN(std::string attr_name, GetAttributeStr(attr));
    PL_ASSIGN_OR_RETURN(auto processed_value,
                        Process(PYPA_PTR_CAST(Call, expr_node), {{}, "", {}}));
    return processed_target->AssignAttribute(attr_name, processed_value);
  }

  // If the target is a Dataframe, we are doing a Subscript map assignment like "df.foo = 2".
  // We need to do special handling here as opposed to the above logic in order to produce a new
  // dataframe.
  PL_ASSIGN_OR_RETURN(auto processed_node, ProcessData(attr, {{}, "", {}}));
  return ProcessMapAssignment(attr->value, std::static_pointer_cast<Dataframe>(processed_target),
                              processed_node, expr_node);
}

Status ASTVisitorImpl::ProcessMapAssignment(const pypa::AstPtr& assign_target,
                                            std::shared_ptr<Dataframe> parent_df, IRNode* target,
                                            const pypa::AstPtr& expr_node) {
  if (assign_target->type != AstType::Name) {
    return CreateAstError(assign_target,
                          "Can only assign to Dataframe by subscript from Name, received $0",
                          GetAstTypeName(assign_target->type));
  }
  auto assign_name_string = GetNameAsString(assign_target);

  if (!Match(target, ColumnNode())) {
    return CreateAstError(assign_target, "Can't assign to node of type $0", target->type_string());
  }
  auto target_column = static_cast<ColumnIR*>(target);

  if (!parent_df->op()) {
    return CreateAstError(assign_target,
                          "Cannot assign column to dataframe that does not contain an operator");
  }

  // Maps can only assign to the same table as the input table when of the form:
  // df['foo'] = df['bar'] + 2
  OperatorContext op_context{{parent_df->op()}, Dataframe::kMapOpId, {assign_name_string}};
  PL_ASSIGN_OR_RETURN(auto result, ProcessData(expr_node, op_context));
  if (!result->IsExpression()) {
    return CreateAstError(
        expr_node, "Expected to receive expression as map subscript assignment value, received $0.",
        result->type_string());
  }
  PL_ASSIGN_OR_RETURN(auto dataframe,
                      parent_df->FromColumnAssignment(expr_node, target_column,
                                                      static_cast<ExpressionIR*>(result)));
  var_table_->Add(assign_name_string, dataframe);

  return ir_graph_->DeleteNode(target_column->id());
}

StatusOr<QLObjectPtr> ASTVisitorImpl::Process(const pypa::AstExpr& node,
                                              const OperatorContext& op_context) {
  switch (node->type) {
    case AstType::Call:
      return ProcessCallNode(PYPA_PTR_CAST(Call, node), op_context);
    case AstType::Subscript:
      return ProcessSubscriptCall(PYPA_PTR_CAST(Subscript, node), op_context);
    case AstType::Name:
      return LookupVariable(PYPA_PTR_CAST(Name, node));
    case AstType::Attribute:
      return ProcessAttribute(PYPA_PTR_CAST(Attribute, node), op_context);
    case AstType::Str: {
      return ProcessStr(PYPA_PTR_CAST(Str, node));
    }
    case AstType::Number: {
      return ProcessNumber(PYPA_PTR_CAST(Number, node));
    }
    case AstType::List: {
      return ProcessList(PYPA_PTR_CAST(List, node), op_context);
    }
    case AstType::Tuple: {
      return ProcessTuple(PYPA_PTR_CAST(Tuple, node), op_context);
    }
    case AstType::BinOp: {
      return ProcessDataBinOp(PYPA_PTR_CAST(BinOp, node), op_context);
    }
    case AstType::BoolOp: {
      return ProcessDataBoolOp(PYPA_PTR_CAST(BoolOp, node), op_context);
    }
    case AstType::Compare: {
      return ProcessDataCompare(PYPA_PTR_CAST(Compare, node), op_context);
    }
    case AstType::UnaryOp: {
      return ProcessDataUnaryOp(PYPA_PTR_CAST(UnaryOp, node), op_context);
    }
    default:
      return CreateAstError(node, "Expression node '$0' not defined", GetAstTypeName(node->type));
  }
}

Status ASTVisitorImpl::ProcessAssignNode(const pypa::AstAssignPtr& node) {
  // Check # nodes to assign.
  if (node->targets.size() != 1) {
    return CreateAstError(node, "We only support single target assignment.");
  }
  // Get the name that we are targeting.
  auto target_node = node->targets[0];

  // Special handler for this type of statement: df['foo'] = df['bar']
  if (target_node->type == AstType::Subscript) {
    return ProcessSubscriptAssignment(PYPA_PTR_CAST(Subscript, node->targets[0]), node->value);
  }
  if (target_node->type == AstType::Attribute) {
    return ProcessAttributeAssignment(PYPA_PTR_CAST(Attribute, node->targets[0]), node->value);
  }

  if (target_node->type != AstType::Name) {
    return CreateAstError(target_node, "Assignment target must be a Name or Subscript");
  }

  std::string assign_name = GetNameAsString(target_node);
  if (assign_name == PixieModule::kPixieModuleObjName) {
    // TODO(nserrino, philkuz): Allow reassignment of the pixie module.
    // It doesn't work right now because default values like "px.now()" are hard-coded
    // into funcs, so if px goes away they can't be processed correctly.
    return CreateAstError(node, "Cannot reassign Pixie Module $0", assign_name);
  }

  OperatorContext op_context({}, "", {});
  PL_ASSIGN_OR_RETURN(auto processed_node, Process(node->value, op_context));
  var_table_->Add(assign_name, processed_node);
  return Status::OK();
}

Status ASTVisitorImpl::DoesArgMatchAnnotation(QLObjectPtr ql_arg, const pypa::AstExpr& annotation) {
  DCHECK(annotation);
  DCHECK(ql_arg->HasNode());
  auto arg = ql_arg->node();
  PL_ASSIGN_OR_RETURN(auto annotation_object, Process(annotation, {{}, "", {}}));
  if (annotation_object->type() == QLObjectType::kDataframe) {
    if (!arg->IsOperator()) {
      return arg->CreateIRNodeError("Expected '$0', received '$1'", Dataframe::DataframeType.name(),
                                    arg->type_string());
    }
    return Status::OK();
  } else if (annotation_object->type() == QLObjectType::kType) {
    auto type_object = std::static_pointer_cast<TypeObject>(annotation_object);
    PL_RETURN_IF_ERROR(type_object->NodeMatches(arg));
    return Status::OK();
  } else {
    return error::Unimplemented("'$0' unhandled annotation",
                                magic_enum::enum_name(annotation_object->type()));
  }
}

StatusOr<QLObjectPtr> ASTVisitorImpl::FuncDefHandler(
    const std::vector<std::string>& arg_names,
    const absl::flat_hash_map<std::string, pypa::AstExpr>& arg_annotations,
    const pypa::AstSuitePtr& body, const pypa::AstPtr& ast, const ParsedArgs& args) {
  // TODO(philkuz) (PL-1365) figure out how to wrap the internal errors with the ast that's passed
  // in.
  PL_UNUSED(ast);

  auto func_visitor = CreateChild();
  for (const std::string& arg_name : arg_names) {
    QLObjectPtr arg_object = args.GetArg(arg_name);

    if (arg_annotations.contains(arg_name)) {
      PL_RETURN_IF_ERROR(
          DoesArgMatchAnnotation(arg_object, arg_annotations.find(arg_name)->second));
    }
    func_visitor->var_table()->Add(arg_name, arg_object);
  }

  return func_visitor->ProcessASTSuite(body, /*is_function_definition_body*/ true);
}

Status ASTVisitorImpl::ProcessFunctionDefNode(const pypa::AstFunctionDefPtr& node) {
  // Create the func object.
  // Use the new function defintion body as the function object.
  // Every time the function is evaluated we should evlauate the body with the values for the
  // args passed into the scope.
  // Parse the args to create the necessary
  auto function_name_node = node->name;
  std::vector<std::string> parsed_arg_names;
  absl::flat_hash_map<std::string, pypa::AstExpr> arg_annotations;
  for (const auto& arg : node->args.arguments) {
    if (arg->type != AstType::Arg) {
      return CreateAstError(arg, "function parameter must be an argument, not a $0",
                            GetAstTypeName(arg->type));
    }
    auto arg_ptr = PYPA_PTR_CAST(Arg, arg);
    parsed_arg_names.push_back(arg_ptr->arg);
    if (arg_ptr->annotation) {
      arg_annotations[arg_ptr->arg] = arg_ptr->annotation;
    }
  }
  // TODO(philkuz) delete keywords from the function definition.
  // For some reason this is kept around, not clear why, making sure that it's 0 for now.
  DCHECK_EQ(node->args.keywords.size(), 0UL);

  // The default values for args. Should be the same length as args. For now we should consider not
  // processing these.
  DCHECK_EQ(node->args.defaults.size(), node->args.arguments.size());
  for (const auto& default_value : node->args.defaults) {
    // TODO(philkuz) support default.
    if (default_value != nullptr) {
      return CreateAstError(default_value, "default values not supported in function definitions");
    }
  }

  // TODO(philkuz) support *args.
  if (node->args.args) {
    return CreateAstError(node->args.args,
                          "variable length args are not supported in function definitions");
  }

  // TODO(philkuz) support **kwargs
  if (node->args.kwargs) {
    return CreateAstError(node->args.kwargs,
                          "variable length kwargs are not supported in function definitions");
  }

  if (function_name_node->type != AstType::Name) {
    return CreateAstError(function_name_node, "function definition must be a name, not a $0",
                          GetAstTypeName(function_name_node->type));
  }

  if (node->body->type != AstType::Suite) {
    return CreateAstError(node->body, "function body of type $0 not allowed",
                          GetAstTypeName(node->body->type));
  }
  pypa::AstSuitePtr body = PYPA_PTR_CAST(Suite, node->body);
  std::string function_name = GetNameAsString(function_name_node);

  PL_ASSIGN_OR_RETURN(auto defined_func,
                      FuncObject::Create(function_name, parsed_arg_names, {}, false, false,
                                         std::bind(&ASTVisitorImpl::FuncDefHandler, this,
                                                   parsed_arg_names, arg_annotations, body,
                                                   std::placeholders::_1, std::placeholders::_2)));
  var_table_->Add(function_name, defined_func);
  return Status::OK();
}

Status ASTVisitorImpl::ValidateSubscriptValue(const pypa::AstExpr& node,
                                              const OperatorContext& op_context) {
  if (op_context.operator_name != Dataframe::kMapOpId) {
    return Status::OK();
  }
  switch (node->type) {
    case AstType::Attribute: {
      // We want to make sure that the parent of an attribute is completely valid, even if it's
      // nested. ie. `df.ctx['service']`
      return ValidateSubscriptValue(PYPA_PTR_CAST(Attribute, node)->value, op_context);
    }
    case AstType::Name: {
      std::string name = GetNameAsString(node);
      if (std::find(op_context.referenceable_dataframes.begin(),
                    op_context.referenceable_dataframes.end(),
                    name) == op_context.referenceable_dataframes.end()) {
        return CreateAstError(node, "name '$0' is not available in this context", name);
      }
      ABSL_FALLTHROUGH_INTENDED;
    }
    default:
      return Status::OK();
  }
}
StatusOr<QLObjectPtr> ASTVisitorImpl::ProcessSubscriptCall(const pypa::AstSubscriptPtr& node,
                                                           const OperatorContext& op_context) {
  // Validate to make sure that we can actually take the subscript in this context.
  PL_RETURN_IF_ERROR(ValidateSubscriptValue(node->value, op_context));
  PL_ASSIGN_OR_RETURN(QLObjectPtr pyobject, Process(node->value, op_context));
  if (!pyobject->HasSubscriptMethod()) {
    return pyobject->CreateError("object is not subscriptable");
  }
  PL_ASSIGN_OR_RETURN(std::shared_ptr<FuncObject> func_object, pyobject->GetSubscriptMethod());

  auto slice = node->slice;
  if (slice->type != AstType::Index) {
    return CreateAstError(slice, "'$0' object cannot be an index", GetAstTypeName(slice->type));
  }

  std::vector<std::string> dfs = op_context.referenceable_dataframes;
  if (node->value->type == AstType::Name) {
    dfs.push_back(GetNameAsString(node->value));
  }

  OperatorContext new_op_context(op_context.parent_ops, op_context.operator_name, dfs);
  PL_ASSIGN_OR_RETURN(QLObjectPtr arg, Process(PYPA_PTR_CAST(Index, slice)->value, new_op_context));
  ArgMap args;
  args.args.push_back(arg);
  return func_object->Call(args, node, this);
}

StatusOr<std::string> ASTVisitorImpl::GetFuncName(const pypa::AstCallPtr& node) {
  std::string func_name;
  switch (node->function->type) {
    case AstType::Name: {
      func_name = GetNameAsString(node->function);
      break;
    }
    case AstType::Attribute: {
      auto attr = PYPA_PTR_CAST(Attribute, node->function);
      if (attr->attribute->type != AstType::Name) {
        return CreateAstError(node->function, "Couldn't get string name out of node of type $0.",
                              GetAstTypeName(attr->attribute->type));
      }
      func_name = GetNameAsString(attr->attribute);
      break;
    }
    default: {
      return CreateAstError(node->function, "Couldn't get string name out of node of type $0.",
                            GetAstTypeName(node->function->type));
    }
  }
  return func_name;
}

StatusOr<ArgMap> ASTVisitorImpl::ProcessArgs(const pypa::AstCallPtr& call_ast,
                                             const OperatorContext& op_context) {
  ArgMap arg_map;
  for (const auto arg : call_ast->arguments) {
    PL_ASSIGN_OR_RETURN(auto value, Process(arg, op_context));
    arg_map.args.push_back(value);
  }

  // Iterate through the keywords
  for (auto& kw_ptr : call_ast->keywords) {
    std::string key = GetNameAsString(kw_ptr->name);
    PL_ASSIGN_OR_RETURN(auto value, Process(kw_ptr->value, op_context));
    arg_map.kwargs.emplace_back(key, value);
  }

  return arg_map;
}

StatusOr<QLObjectPtr> ASTVisitorImpl::LookupVariable(const pypa::AstPtr& ast,
                                                     const std::string& name) {
  auto var = var_table_->Lookup(name);
  if (var == nullptr) {
    return CreateAstError(ast, "name '$0' is not defined", name);
  }
  return var;
}

StatusOr<OperatorIR*> ASTVisitorImpl::LookupName(const pypa::AstNamePtr& name_node) {
  PL_ASSIGN_OR_RETURN(QLObjectPtr pyobject, LookupVariable(name_node));
  if (!pyobject->HasNode()) {
    return CreateAstError(name_node, "'$0' not accessible", name_node->id);
  }
  IRNode* node = pyobject->node();
  if (!node->IsOperator()) {
    return node->CreateIRNodeError("Only dataframes may be assigned variables, $0 not allowed",
                                   node->type_string());
  }
  return static_cast<OperatorIR*>(node);
}

StatusOr<QLObjectPtr> ASTVisitorImpl::ProcessAttribute(const pypa::AstAttributePtr& node,
                                                       const OperatorContext& op_context) {
  PL_ASSIGN_OR_RETURN(std::string attr_name, GetAttributeStr(node));
  PL_ASSIGN_OR_RETURN(QLObjectPtr value_obj, Process(node->value, op_context));
  return value_obj->GetAttribute(node, attr_name);
}

StatusOr<std::string> ASTVisitorImpl::GetAttributeStr(const pypa::AstAttributePtr& attr) {
  if (attr->attribute->type != AstType::Name) {
    return CreateAstError(attr, "$0 not a valid attribute", GetAstTypeName(attr->attribute->type));
  }
  return GetNameAsString(attr->attribute);
}

StatusOr<QLObjectPtr> ASTVisitorImpl::ProcessCallNode(const pypa::AstCallPtr& node,
                                                      const OperatorContext& op_context) {
  std::shared_ptr<FuncObject> func_object;
  // pyobject declared up here because we need this object to be allocated when
  // func_object->Call() is made.
  PL_ASSIGN_OR_RETURN(QLObjectPtr pyobject, Process(node->function, op_context));
  if (pyobject->type_descriptor().type() == QLObjectType::kExpr) {
    if (Match(pyobject->node(), ColumnNode())) {
      return CreateAstError(node, "dataframe has no method '$0'",
                            static_cast<ColumnIR*>(pyobject->node())->col_name());
    }
    return CreateAstError(node, "expression object is not callable");
  }

  if (pyobject->type_descriptor().type() != QLObjectType::kFunction) {
    PL_ASSIGN_OR_RETURN(func_object, pyobject->GetCallMethod());
  } else {
    func_object = std::static_pointer_cast<FuncObject>(pyobject);
  }
  PL_ASSIGN_OR_RETURN(ArgMap args, ProcessArgs(node, op_context));
  return func_object->Call(args, node, this);
}

StatusOr<QLObjectPtr> ASTVisitorImpl::ProcessStr(const pypa::AstStrPtr& ast) {
  PL_ASSIGN_OR_RETURN(auto str_value, GetStrAstValue(ast));
  PL_ASSIGN_OR_RETURN(StringIR * node, ir_graph_->CreateNode<StringIR>(ast, str_value));
  return ExprObject::Create(node);
}

StatusOr<std::vector<IRNode*>> ASTVisitorImpl::ProcessCollectionChildren(
    const pypa::AstExprList& elements, const OperatorContext& op_context) {
  std::vector<IRNode*> children;
  for (auto& child : elements) {
    PL_ASSIGN_OR_RETURN(IRNode * child_node, ProcessData(child, op_context));
    children.push_back(child_node);
  }
  return children;
}

StatusOr<QLObjectPtr> ASTVisitorImpl::ProcessList(const pypa::AstListPtr& ast,
                                                  const OperatorContext& op_context) {
  PL_ASSIGN_OR_RETURN(std::vector<IRNode*> expr_vec,
                      ProcessCollectionChildren(ast->elements, op_context));
  PL_ASSIGN_OR_RETURN(ListIR * node, ir_graph_->CreateNode<ListIR>(ast, expr_vec));
  return CollectionObject::Create(node);
}

StatusOr<QLObjectPtr> ASTVisitorImpl::ProcessTuple(const pypa::AstTuplePtr& ast,
                                                   const OperatorContext& op_context) {
  PL_ASSIGN_OR_RETURN(std::vector<IRNode*> expr_vec,
                      ProcessCollectionChildren(ast->elements, op_context));
  PL_ASSIGN_OR_RETURN(TupleIR * node, ir_graph_->CreateNode<TupleIR>(ast, expr_vec));
  return CollectionObject::Create(node);
}

StatusOr<QLObjectPtr> ASTVisitorImpl::ProcessNumber(const pypa::AstNumberPtr& node) {
  switch (node->num_type) {
    case pypa::AstNumber::Type::Float: {
      PL_ASSIGN_OR_RETURN(FloatIR * ir_node, ir_graph_->CreateNode<FloatIR>(node, node->floating));
      return ExprObject::Create(ir_node);
    }
    case pypa::AstNumber::Type::Integer:
    case pypa::AstNumber::Type::Long: {
      PL_ASSIGN_OR_RETURN(IntIR * ir_node, ir_graph_->CreateNode<IntIR>(node, node->integer));
      return ExprObject::Create(ir_node);
    }
    default:
      return CreateAstError(node, "Couldn't find number type $0", node->num_type);
  }
}

StatusOr<QLObjectPtr> ASTVisitorImpl::ProcessDataBinOp(const pypa::AstBinOpPtr& node,
                                                       const OperatorContext& op_context) {
  std::string op_str = pypa::to_string(node->op);

  PL_ASSIGN_OR_RETURN(IRNode * left, ProcessData(node->left, op_context));
  PL_ASSIGN_OR_RETURN(IRNode * right, ProcessData(node->right, op_context));
  if (!left->IsExpression()) {
    return CreateAstError(
        node,
        "Expected left side of operation to be an expression, but got $0, which is not an "
        "expression..",
        left->type_string());
  }
  if (!right->IsExpression()) {
    return CreateAstError(
        node,
        "Expected right side of operation to be an expression, but got $0, which is not an "
        "expression.",
        right->type_string());
  }

  PL_ASSIGN_OR_RETURN(FuncIR::Op op, GetOp(op_str, node));
  std::vector<ExpressionIR*> expressions = {static_cast<ExpressionIR*>(left),
                                            static_cast<ExpressionIR*>(right)};
  PL_ASSIGN_OR_RETURN(FuncIR * ir_node, ir_graph_->CreateNode<FuncIR>(node, op, expressions));
  return ExprObject::Create(ir_node);
}

StatusOr<QLObjectPtr> ASTVisitorImpl::ProcessDataBoolOp(const pypa::AstBoolOpPtr& node,
                                                        const OperatorContext& op_context) {
  std::string op_str = pypa::to_string(node->op);
  if (node->values.size() != 2) {
    return CreateAstError(node, "Expected two arguments to '$0'.", op_str);
  }

  PL_ASSIGN_OR_RETURN(IRNode * left, ProcessData(node->values[0], op_context));
  PL_ASSIGN_OR_RETURN(IRNode * right, ProcessData(node->values[1], op_context));
  if (!left->IsExpression()) {
    return CreateAstError(
        node,
        "Expected left side of operation to be an expression, but got $0, which is not an "
        "expression..",
        left->type_string());
  }
  if (!right->IsExpression()) {
    return CreateAstError(
        node,
        "Expected right side of operation to be an expression, but got $0, which is not an "
        "expression.",
        right->type_string());
  }

  PL_ASSIGN_OR_RETURN(FuncIR::Op op, GetOp(op_str, node));
  std::vector<ExpressionIR*> expressions = {static_cast<ExpressionIR*>(left),
                                            static_cast<ExpressionIR*>(right)};
  PL_ASSIGN_OR_RETURN(FuncIR * ir_node, ir_graph_->CreateNode<FuncIR>(node, op, expressions));
  return ExprObject::Create(ir_node);
}

StatusOr<QLObjectPtr> ASTVisitorImpl::ProcessDataCompare(const pypa::AstComparePtr& node,
                                                         const OperatorContext& op_context) {
  DCHECK_EQ(node->operators.size(), 1ULL);
  std::string op_str = pypa::to_string(node->operators[0]);
  if (node->comparators.size() != 1) {
    return CreateAstError(node, "Only expected one argument to the right of '$0'.", op_str);
  }
  PL_ASSIGN_OR_RETURN(IRNode * left, ProcessData(node->left, op_context));
  if (!left->IsExpression()) {
    return CreateAstError(
        node,
        "Expected left side of operation to be an expression, but got $0, which is not an "
        "expression..",
        left->type_string());
  }
  std::vector<ExpressionIR*> expressions = {static_cast<ExpressionIR*>(left)};

  for (const auto& comp : node->comparators) {
    PL_ASSIGN_OR_RETURN(IRNode * expr, ProcessData(comp, op_context));
    if (!expr->IsExpression()) {
      return CreateAstError(comp, "Expected expression, but got $0, which is not an expression.",
                            expr->type_string());
    }
    expressions.push_back(static_cast<ExpressionIR*>(expr));
  }

  PL_ASSIGN_OR_RETURN(FuncIR::Op op, GetOp(op_str, node));
  PL_ASSIGN_OR_RETURN(FuncIR * ir_node, ir_graph_->CreateNode<FuncIR>(node, op, expressions));
  return ExprObject::Create(ir_node);
}

StatusOr<QLObjectPtr> ASTVisitorImpl::ProcessDataUnaryOp(const pypa::AstUnaryOpPtr& node,
                                                         const OperatorContext& op_context) {
  PL_ASSIGN_OR_RETURN(IRNode * operand, ProcessData(node->operand, op_context));
  if (!operand->IsExpression()) {
    return CreateAstError(node, "Expected operand of unary op to be expression, but got $0",
                          operand->type_string());
  }

  std::string op_str = pypa::to_string(node->op);
  PL_ASSIGN_OR_RETURN(FuncIR::Op op, GetUnaryOp(op_str, node));
  if (op.op_code == FuncIR::Opcode::non_op) {
    return ExprObject::Create(static_cast<ExpressionIR*>(operand));
  }
  std::vector<ExpressionIR*> args = {static_cast<ExpressionIR*>(operand)};
  PL_ASSIGN_OR_RETURN(FuncIR * ir_node, ir_graph_->CreateNode<FuncIR>(node, op, args));
  return ExprObject::Create(ir_node);
}

StatusOr<IRNode*> ASTVisitorImpl::ProcessData(const pypa::AstPtr& ast,
                                              const OperatorContext& op_context) {
  PL_ASSIGN_OR_RETURN(QLObjectPtr ql_object, Process(PYPA_PTR_CAST(Call, ast), op_context));

  // TODO(nserrino) : Remove this hack once PL-1431 is done.
  if (!ql_object->HasNode()) {
    PL_ASSIGN_OR_RETURN(ql_object, CallFunc(ast, ql_object));
  }
  DCHECK(ql_object->HasNode());
  return ql_object->node();
}

StatusOr<QLObjectPtr> ASTVisitorImpl::ProcessFuncDefReturn(const pypa::AstReturnPtr& ret) {
  if (ret->value == nullptr) {
    return std::static_pointer_cast<QLObject>(std::make_shared<NoneObject>(ret));
  }

  return Process(ret->value, {{}, "", {}});
}

}  // namespace compiler
}  // namespace carnot
}  // namespace pl
