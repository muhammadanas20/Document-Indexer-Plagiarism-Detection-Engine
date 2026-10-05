// scorer.cpp -- STUB (Alyan, W5).
#include "search/scorer.hpp"

namespace plag {

double jaccard(std::size_t /*shared*/, std::size_t /*a*/, std::size_t /*b*/) {
  return 0.0;  // STUB dummy
}

double containment(std::size_t /*shared*/, std::size_t /*a*/) {
  return 0.0;  // STUB dummy
}

double coverage(std::size_t /*matchedTokens*/, std::size_t /*shorterDoc*/) {
  return 0.0;  // STUB dummy
}

double suspicion_key(const PairScore& /*p*/) { return 0.0; }  // STUB dummy

PairScore score_pair(DocumentId /*a*/, DocumentId /*b*/, std::size_t /*shared*/,
                     std::size_t /*tokensA*/, std::size_t /*tokensB*/,
                     std::size_t /*matchedTokens*/, std::uint32_t /*spans*/) {
  return PairScore{};  // STUB dummy
}

}  // namespace plag
