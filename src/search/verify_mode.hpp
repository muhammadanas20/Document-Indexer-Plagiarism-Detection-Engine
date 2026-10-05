// verify_mode.hpp -- verification modes and suspicion labels.
// Owner: ALYAN.  src/search/ is Alyan's area.
// STUB (Alyan, W3): dummy bodies.
#ifndef PLAG_SRC_SEARCH_VERIFY_MODE_HPP_
#define PLAG_SRC_SEARCH_VERIFY_MODE_HPP_

#include <cstdint>

namespace plag {

enum class VerifyMode : std::uint8_t {
  Single = 0,   // one 64-bit fingerprint
  Double,       // two independent modulus pairs -- the false-positive demo
  Exact,        // re-compare the token ids themselves
};

enum class Suspicion : std::uint8_t {
  None = 0,
  Low,
  Medium,
  High,
  Critical,
};

VerifyMode parse_verify_mode(const char* flag);
Suspicion label_for(double coverage);
const char* to_string(Suspicion s);

}  // namespace plag

#endif  // PLAG_SRC_SEARCH_VERIFY_MODE_HPP_
