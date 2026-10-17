// Similarity scoring metrics (Jaccard, containment, coverage)
#ifndef PLAG_SRC_SEARCH_SCORER_HPP_
#define PLAG_SRC_SEARCH_SCORER_HPP_

#include <cstddef>

#include "contract/contract.hpp"

namespace plag {

double jaccard(std::size_t shared, std::size_t a, std::size_t b);
double containment(std::size_t shared, std::size_t a);
double coverage(std::size_t matchedTokens, std::size_t shorterDoc);

double suspicion_key(const PairScore& p);

PairScore score_pair(DocumentId a, DocumentId b, std::size_t shared,
                     std::size_t tokensA, std::size_t tokensB,
                     std::size_t matchedTokens, std::uint32_t spans);

}  // namespace plag

#endif  // PLAG_SRC_SEARCH_SCORER_HPP_
