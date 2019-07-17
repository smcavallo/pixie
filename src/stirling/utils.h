#pragma once
#include <string>
#include <utility>
#include "src/common/base/base.h"
#include "src/common/base/status.h"
#include "src/stirling/socket_trace_event_type.h"

namespace pl {
namespace stirling {
// TODO(chengruizhe/oazizi): Move to common someday.
int BEBytesToInt(const char arr[], size_t size);
int LEStrToInt(const std::string str);

template <size_t N>
void EndianSwap(const char bytes[N], char result[N]) {
  if (N == 0) {
    return;
  }
  for (size_t k = 0; k < N; k++) {
    result[k] = bytes[N - k - 1];
  }
}

template <size_t N>
void IntToLEBytes(int num, char result[N]) {
  for (size_t i = 0; i < N; i++) {
    result[i] = (num >> (i * 8));
  }
}
}  // namespace stirling
}  // namespace pl
