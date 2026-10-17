// Implementation of verification mode parsing and suspicion labels
#include "search/verify_mode.hpp"

#include <cstring>

namespace plag {

VerifyMode parse_verify_mode(const char* flag) {
  if (flag == nullptr) {
    return VerifyMode::Single;
  }
  if (std::strcmp(flag, "double") == 0) {
    return VerifyMode::Double;
  }
  if (std::strcmp(flag, "exact") == 0) {
    return VerifyMode::Exact;
  }
  return VerifyMode::Single;
}

Suspicion label_for(double coverage) {
  if (coverage >= 0.80) return Suspicion::Critical;
  if (coverage >= 0.55) return Suspicion::High;
  if (coverage >= 0.30) return Suspicion::Medium;
  if (coverage >= 0.10) return Suspicion::Low;
  return Suspicion::None;
}

const char* to_string(Suspicion s) {
  switch (s) {
    case Suspicion::None:
      return "none";
    case Suspicion::Low:
      return "low";
    case Suspicion::Medium:
      return "medium";
    case Suspicion::High:
      return "high";
    case Suspicion::Critical:
      return "critical";
  }
  return "unknown";
}

}  // namespace plag
