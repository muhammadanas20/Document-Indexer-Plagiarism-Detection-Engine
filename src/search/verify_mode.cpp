// verify_mode.cpp -- STUB (Alyan, W3).
#include "search/verify_mode.hpp"

namespace plag {

VerifyMode parse_verify_mode(const char* /*flag*/) {
  return VerifyMode::Single;  // STUB dummy
}

Suspicion label_for(double /*coverage*/) {
  return Suspicion::None;  // STUB dummy
}

const char* to_string(Suspicion s) {
  switch (s) {
    case Suspicion::None:     return "none";
    case Suspicion::Low:      return "low";
    case Suspicion::Medium:   return "medium";
    case Suspicion::High:     return "high";
    case Suspicion::Critical: return "critical";
  }
  return "unknown";
}

}  // namespace plag
