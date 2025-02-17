// Code generated by MockGen. DO NOT EDIT.
// Source: server.go

// Package mock_controller is a generated GoMock package.
package mock_controller

import (
	reflect "reflect"

	uuid "github.com/gofrs/uuid"
	gomock "github.com/golang/mock/gomock"
	datastore "px.dev/pixie/src/cloud/profile/datastore"
)

// MockUserDatastore is a mock of UserDatastore interface.
type MockUserDatastore struct {
	ctrl     *gomock.Controller
	recorder *MockUserDatastoreMockRecorder
}

// MockUserDatastoreMockRecorder is the mock recorder for MockUserDatastore.
type MockUserDatastoreMockRecorder struct {
	mock *MockUserDatastore
}

// NewMockUserDatastore creates a new mock instance.
func NewMockUserDatastore(ctrl *gomock.Controller) *MockUserDatastore {
	mock := &MockUserDatastore{ctrl: ctrl}
	mock.recorder = &MockUserDatastoreMockRecorder{mock}
	return mock
}

// EXPECT returns an object that allows the caller to indicate expected use.
func (m *MockUserDatastore) EXPECT() *MockUserDatastoreMockRecorder {
	return m.recorder
}

// CreateUser mocks base method.
func (m *MockUserDatastore) CreateUser(arg0 *datastore.UserInfo) (uuid.UUID, error) {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "CreateUser", arg0)
	ret0, _ := ret[0].(uuid.UUID)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// CreateUser indicates an expected call of CreateUser.
func (mr *MockUserDatastoreMockRecorder) CreateUser(arg0 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "CreateUser", reflect.TypeOf((*MockUserDatastore)(nil).CreateUser), arg0)
}

// CreateUserAndOrg mocks base method.
func (m *MockUserDatastore) CreateUserAndOrg(arg0 *datastore.OrgInfo, arg1 *datastore.UserInfo) (uuid.UUID, uuid.UUID, error) {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "CreateUserAndOrg", arg0, arg1)
	ret0, _ := ret[0].(uuid.UUID)
	ret1, _ := ret[1].(uuid.UUID)
	ret2, _ := ret[2].(error)
	return ret0, ret1, ret2
}

// CreateUserAndOrg indicates an expected call of CreateUserAndOrg.
func (mr *MockUserDatastoreMockRecorder) CreateUserAndOrg(arg0, arg1 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "CreateUserAndOrg", reflect.TypeOf((*MockUserDatastore)(nil).CreateUserAndOrg), arg0, arg1)
}

// GetUser mocks base method.
func (m *MockUserDatastore) GetUser(arg0 uuid.UUID) (*datastore.UserInfo, error) {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "GetUser", arg0)
	ret0, _ := ret[0].(*datastore.UserInfo)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// GetUser indicates an expected call of GetUser.
func (mr *MockUserDatastoreMockRecorder) GetUser(arg0 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "GetUser", reflect.TypeOf((*MockUserDatastore)(nil).GetUser), arg0)
}

// GetUserByAuthProviderID mocks base method.
func (m *MockUserDatastore) GetUserByAuthProviderID(arg0 string) (*datastore.UserInfo, error) {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "GetUserByAuthProviderID", arg0)
	ret0, _ := ret[0].(*datastore.UserInfo)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// GetUserByAuthProviderID indicates an expected call of GetUserByAuthProviderID.
func (mr *MockUserDatastoreMockRecorder) GetUserByAuthProviderID(arg0 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "GetUserByAuthProviderID", reflect.TypeOf((*MockUserDatastore)(nil).GetUserByAuthProviderID), arg0)
}

// GetUserByEmail mocks base method.
func (m *MockUserDatastore) GetUserByEmail(arg0 string) (*datastore.UserInfo, error) {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "GetUserByEmail", arg0)
	ret0, _ := ret[0].(*datastore.UserInfo)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// GetUserByEmail indicates an expected call of GetUserByEmail.
func (mr *MockUserDatastoreMockRecorder) GetUserByEmail(arg0 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "GetUserByEmail", reflect.TypeOf((*MockUserDatastore)(nil).GetUserByEmail), arg0)
}

// UpdateUser mocks base method.
func (m *MockUserDatastore) UpdateUser(arg0 *datastore.UserInfo) error {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "UpdateUser", arg0)
	ret0, _ := ret[0].(error)
	return ret0
}

// UpdateUser indicates an expected call of UpdateUser.
func (mr *MockUserDatastoreMockRecorder) UpdateUser(arg0 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "UpdateUser", reflect.TypeOf((*MockUserDatastore)(nil).UpdateUser), arg0)
}

// MockOrgDatastore is a mock of OrgDatastore interface.
type MockOrgDatastore struct {
	ctrl     *gomock.Controller
	recorder *MockOrgDatastoreMockRecorder
}

// MockOrgDatastoreMockRecorder is the mock recorder for MockOrgDatastore.
type MockOrgDatastoreMockRecorder struct {
	mock *MockOrgDatastore
}

// NewMockOrgDatastore creates a new mock instance.
func NewMockOrgDatastore(ctrl *gomock.Controller) *MockOrgDatastore {
	mock := &MockOrgDatastore{ctrl: ctrl}
	mock.recorder = &MockOrgDatastoreMockRecorder{mock}
	return mock
}

// EXPECT returns an object that allows the caller to indicate expected use.
func (m *MockOrgDatastore) EXPECT() *MockOrgDatastoreMockRecorder {
	return m.recorder
}

// ApproveAllOrgUsers mocks base method.
func (m *MockOrgDatastore) ApproveAllOrgUsers(arg0 uuid.UUID) error {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "ApproveAllOrgUsers", arg0)
	ret0, _ := ret[0].(error)
	return ret0
}

// ApproveAllOrgUsers indicates an expected call of ApproveAllOrgUsers.
func (mr *MockOrgDatastoreMockRecorder) ApproveAllOrgUsers(arg0 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "ApproveAllOrgUsers", reflect.TypeOf((*MockOrgDatastore)(nil).ApproveAllOrgUsers), arg0)
}

// CreateOrg mocks base method.
func (m *MockOrgDatastore) CreateOrg(arg0 *datastore.OrgInfo) (uuid.UUID, error) {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "CreateOrg", arg0)
	ret0, _ := ret[0].(uuid.UUID)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// CreateOrg indicates an expected call of CreateOrg.
func (mr *MockOrgDatastoreMockRecorder) CreateOrg(arg0 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "CreateOrg", reflect.TypeOf((*MockOrgDatastore)(nil).CreateOrg), arg0)
}

// DeleteOrgAndUsers mocks base method.
func (m *MockOrgDatastore) DeleteOrgAndUsers(arg0 uuid.UUID) error {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "DeleteOrgAndUsers", arg0)
	ret0, _ := ret[0].(error)
	return ret0
}

// DeleteOrgAndUsers indicates an expected call of DeleteOrgAndUsers.
func (mr *MockOrgDatastoreMockRecorder) DeleteOrgAndUsers(arg0 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "DeleteOrgAndUsers", reflect.TypeOf((*MockOrgDatastore)(nil).DeleteOrgAndUsers), arg0)
}

// GetOrg mocks base method.
func (m *MockOrgDatastore) GetOrg(arg0 uuid.UUID) (*datastore.OrgInfo, error) {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "GetOrg", arg0)
	ret0, _ := ret[0].(*datastore.OrgInfo)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// GetOrg indicates an expected call of GetOrg.
func (mr *MockOrgDatastoreMockRecorder) GetOrg(arg0 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "GetOrg", reflect.TypeOf((*MockOrgDatastore)(nil).GetOrg), arg0)
}

// GetOrgByDomain mocks base method.
func (m *MockOrgDatastore) GetOrgByDomain(arg0 string) (*datastore.OrgInfo, error) {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "GetOrgByDomain", arg0)
	ret0, _ := ret[0].(*datastore.OrgInfo)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// GetOrgByDomain indicates an expected call of GetOrgByDomain.
func (mr *MockOrgDatastoreMockRecorder) GetOrgByDomain(arg0 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "GetOrgByDomain", reflect.TypeOf((*MockOrgDatastore)(nil).GetOrgByDomain), arg0)
}

// GetOrgByName mocks base method.
func (m *MockOrgDatastore) GetOrgByName(arg0 string) (*datastore.OrgInfo, error) {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "GetOrgByName", arg0)
	ret0, _ := ret[0].(*datastore.OrgInfo)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// GetOrgByName indicates an expected call of GetOrgByName.
func (mr *MockOrgDatastoreMockRecorder) GetOrgByName(arg0 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "GetOrgByName", reflect.TypeOf((*MockOrgDatastore)(nil).GetOrgByName), arg0)
}

// GetOrgs mocks base method.
func (m *MockOrgDatastore) GetOrgs() ([]*datastore.OrgInfo, error) {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "GetOrgs")
	ret0, _ := ret[0].([]*datastore.OrgInfo)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// GetOrgs indicates an expected call of GetOrgs.
func (mr *MockOrgDatastoreMockRecorder) GetOrgs() *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "GetOrgs", reflect.TypeOf((*MockOrgDatastore)(nil).GetOrgs))
}

// GetUsersInOrg mocks base method.
func (m *MockOrgDatastore) GetUsersInOrg(arg0 uuid.UUID) ([]*datastore.UserInfo, error) {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "GetUsersInOrg", arg0)
	ret0, _ := ret[0].([]*datastore.UserInfo)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// GetUsersInOrg indicates an expected call of GetUsersInOrg.
func (mr *MockOrgDatastoreMockRecorder) GetUsersInOrg(arg0 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "GetUsersInOrg", reflect.TypeOf((*MockOrgDatastore)(nil).GetUsersInOrg), arg0)
}

// UpdateOrg mocks base method.
func (m *MockOrgDatastore) UpdateOrg(arg0 *datastore.OrgInfo) error {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "UpdateOrg", arg0)
	ret0, _ := ret[0].(error)
	return ret0
}

// UpdateOrg indicates an expected call of UpdateOrg.
func (mr *MockOrgDatastoreMockRecorder) UpdateOrg(arg0 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "UpdateOrg", reflect.TypeOf((*MockOrgDatastore)(nil).UpdateOrg), arg0)
}

// MockUserSettingsDatastore is a mock of UserSettingsDatastore interface.
type MockUserSettingsDatastore struct {
	ctrl     *gomock.Controller
	recorder *MockUserSettingsDatastoreMockRecorder
}

// MockUserSettingsDatastoreMockRecorder is the mock recorder for MockUserSettingsDatastore.
type MockUserSettingsDatastoreMockRecorder struct {
	mock *MockUserSettingsDatastore
}

// NewMockUserSettingsDatastore creates a new mock instance.
func NewMockUserSettingsDatastore(ctrl *gomock.Controller) *MockUserSettingsDatastore {
	mock := &MockUserSettingsDatastore{ctrl: ctrl}
	mock.recorder = &MockUserSettingsDatastoreMockRecorder{mock}
	return mock
}

// EXPECT returns an object that allows the caller to indicate expected use.
func (m *MockUserSettingsDatastore) EXPECT() *MockUserSettingsDatastoreMockRecorder {
	return m.recorder
}

// GetUserAttributes mocks base method.
func (m *MockUserSettingsDatastore) GetUserAttributes(arg0 uuid.UUID) (*datastore.UserAttributes, error) {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "GetUserAttributes", arg0)
	ret0, _ := ret[0].(*datastore.UserAttributes)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// GetUserAttributes indicates an expected call of GetUserAttributes.
func (mr *MockUserSettingsDatastoreMockRecorder) GetUserAttributes(arg0 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "GetUserAttributes", reflect.TypeOf((*MockUserSettingsDatastore)(nil).GetUserAttributes), arg0)
}

// GetUserSettings mocks base method.
func (m *MockUserSettingsDatastore) GetUserSettings(arg0 uuid.UUID) (*datastore.UserSettings, error) {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "GetUserSettings", arg0)
	ret0, _ := ret[0].(*datastore.UserSettings)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// GetUserSettings indicates an expected call of GetUserSettings.
func (mr *MockUserSettingsDatastoreMockRecorder) GetUserSettings(arg0 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "GetUserSettings", reflect.TypeOf((*MockUserSettingsDatastore)(nil).GetUserSettings), arg0)
}

// SetUserAttributes mocks base method.
func (m *MockUserSettingsDatastore) SetUserAttributes(arg0 *datastore.UserAttributes) error {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "SetUserAttributes", arg0)
	ret0, _ := ret[0].(error)
	return ret0
}

// SetUserAttributes indicates an expected call of SetUserAttributes.
func (mr *MockUserSettingsDatastoreMockRecorder) SetUserAttributes(arg0 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SetUserAttributes", reflect.TypeOf((*MockUserSettingsDatastore)(nil).SetUserAttributes), arg0)
}

// UpdateUserSettings mocks base method.
func (m *MockUserSettingsDatastore) UpdateUserSettings(arg0 *datastore.UserSettings) error {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "UpdateUserSettings", arg0)
	ret0, _ := ret[0].(error)
	return ret0
}

// UpdateUserSettings indicates an expected call of UpdateUserSettings.
func (mr *MockUserSettingsDatastoreMockRecorder) UpdateUserSettings(arg0 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "UpdateUserSettings", reflect.TypeOf((*MockUserSettingsDatastore)(nil).UpdateUserSettings), arg0)
}

// MockOrgSettingsDatastore is a mock of OrgSettingsDatastore interface.
type MockOrgSettingsDatastore struct {
	ctrl     *gomock.Controller
	recorder *MockOrgSettingsDatastoreMockRecorder
}

// MockOrgSettingsDatastoreMockRecorder is the mock recorder for MockOrgSettingsDatastore.
type MockOrgSettingsDatastoreMockRecorder struct {
	mock *MockOrgSettingsDatastore
}

// NewMockOrgSettingsDatastore creates a new mock instance.
func NewMockOrgSettingsDatastore(ctrl *gomock.Controller) *MockOrgSettingsDatastore {
	mock := &MockOrgSettingsDatastore{ctrl: ctrl}
	mock.recorder = &MockOrgSettingsDatastoreMockRecorder{mock}
	return mock
}

// EXPECT returns an object that allows the caller to indicate expected use.
func (m *MockOrgSettingsDatastore) EXPECT() *MockOrgSettingsDatastoreMockRecorder {
	return m.recorder
}

// AddIDEConfig mocks base method.
func (m *MockOrgSettingsDatastore) AddIDEConfig(arg0 uuid.UUID, arg1 *datastore.IDEConfig) error {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "AddIDEConfig", arg0, arg1)
	ret0, _ := ret[0].(error)
	return ret0
}

// AddIDEConfig indicates an expected call of AddIDEConfig.
func (mr *MockOrgSettingsDatastoreMockRecorder) AddIDEConfig(arg0, arg1 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "AddIDEConfig", reflect.TypeOf((*MockOrgSettingsDatastore)(nil).AddIDEConfig), arg0, arg1)
}

// DeleteIDEConfig mocks base method.
func (m *MockOrgSettingsDatastore) DeleteIDEConfig(arg0 uuid.UUID, arg1 string) error {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "DeleteIDEConfig", arg0, arg1)
	ret0, _ := ret[0].(error)
	return ret0
}

// DeleteIDEConfig indicates an expected call of DeleteIDEConfig.
func (mr *MockOrgSettingsDatastoreMockRecorder) DeleteIDEConfig(arg0, arg1 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "DeleteIDEConfig", reflect.TypeOf((*MockOrgSettingsDatastore)(nil).DeleteIDEConfig), arg0, arg1)
}

// GetIDEConfig mocks base method.
func (m *MockOrgSettingsDatastore) GetIDEConfig(arg0 uuid.UUID, arg1 string) (*datastore.IDEConfig, error) {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "GetIDEConfig", arg0, arg1)
	ret0, _ := ret[0].(*datastore.IDEConfig)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// GetIDEConfig indicates an expected call of GetIDEConfig.
func (mr *MockOrgSettingsDatastoreMockRecorder) GetIDEConfig(arg0, arg1 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "GetIDEConfig", reflect.TypeOf((*MockOrgSettingsDatastore)(nil).GetIDEConfig), arg0, arg1)
}

// GetIDEConfigs mocks base method.
func (m *MockOrgSettingsDatastore) GetIDEConfigs(arg0 uuid.UUID) ([]*datastore.IDEConfig, error) {
	m.ctrl.T.Helper()
	ret := m.ctrl.Call(m, "GetIDEConfigs", arg0)
	ret0, _ := ret[0].([]*datastore.IDEConfig)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// GetIDEConfigs indicates an expected call of GetIDEConfigs.
func (mr *MockOrgSettingsDatastoreMockRecorder) GetIDEConfigs(arg0 interface{}) *gomock.Call {
	mr.mock.ctrl.T.Helper()
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "GetIDEConfigs", reflect.TypeOf((*MockOrgSettingsDatastore)(nil).GetIDEConfigs), arg0)
}
