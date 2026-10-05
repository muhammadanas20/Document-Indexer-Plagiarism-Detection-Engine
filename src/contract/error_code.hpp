// error_code.hpp -- the only error vocabulary the engine uses.
// Owner: Anas.  FROZEN after Week 1: to change it, open an issue titled
// "contract: need <hook>" -- Anas adds it in a 5-line commit.
#ifndef PLAG_SRC_CONTRACT_ERROR_CODE_HPP_
#define PLAG_SRC_CONTRACT_ERROR_CODE_HPP_

#include <cstdint>

namespace plag {

// No exceptions anywhere in the engine. Every failure path returns one of
// these so a 10 000-file corpus with one unreadable file still finishes.
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
