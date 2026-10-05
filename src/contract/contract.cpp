// contract.cpp -- the one piece of behaviour the contract owns: error text.
// Owner: Anas.  FROZEN after Week 1.
#include "contract/error_code.hpp"

namespace plag {

const char* to_string(ErrorCode e) {
  switch (e) {
    case ErrorCode::Ok:          return "Ok";
    case ErrorCode::NotFound:    return "NotFound";
    case ErrorCode::NotReadable: return "NotReadable";
    case ErrorCode::EmptyFile:   return "EmptyFile";
    case ErrorCode::BadArg:      return "BadArg";
  }
  return "Unknown";
}

}  // namespace plag
