// Error codes returned by file and CLI operations
#ifndef PLAG_SRC_CONTRACT_ERROR_CODE_HPP_
#define PLAG_SRC_CONTRACT_ERROR_CODE_HPP_

#include <cstdint>

namespace plag {

enum class ErrorCode : std::uint8_t {
  Ok = 0,
  NotFound,
  NotReadable,
  EmptyFile,
  BadArg,
};

const char* to_string(ErrorCode e);

}  // namespace plag

#endif  // PLAG_SRC_CONTRACT_ERROR_CODE_HPP_
