// scorer.hpp -- jaccard / containment / coverage and the suspicion key.
// Owner: ALYAN.  src/search/ is Alyan's area.
// STUB (Alyan, W5): dummy bodies.
#ifndef PLAG_SRC_SEARCH_SCORER_HPP_
#define PLAG_SRC_SEARCH_SCORER_HPP_

#include <cstddef>

#include "contract/contract.hpp"

namespace plag {

// |A n B| / |A u B|
double jaccard(std::size_t shared, std::size_t a, std::size_t b);
// |A n B| / |A| -- catches the student who copied 10 lines into a 500-line file
double containment(std::size_t shared, std::size_t a);
// matched tokens / total tokens of the shorter document
double coverage(std::size_t matchedTokens, std::size_t shorterDoc);

// One number to sort the leaderboard by; Alyan defines the weighting.
double suspicion_key(const PairScore& p);

PairScore score_pair(DocumentId a, DocumentId b, std::size_t shared,
                     std::size_t tokensA, std::size_t tokensB,
                     std::size_t matchedTokens, std::uint32_t spans);

}  // namespace plag

#endif  // PLAG_SRC_SEARCH_SCORER_HPP_
