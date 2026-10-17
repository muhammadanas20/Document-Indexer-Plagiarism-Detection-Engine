// Finds document pairs that share at least minShared k-gram fingerprints
#ifndef PLAG_SRC_SEARCH_CANDIDATE_GENERATOR_HPP_
#define PLAG_SRC_SEARCH_CANDIDATE_GENERATOR_HPP_

#include <cstddef>

#include "common/dyn_array.hpp"
#include "contract/contract.hpp"

namespace plag {

class IndexBuilder;

struct CandidatePair {
  DocumentId docA = 0;
  DocumentId docB = 0;
  std::uint32_t sharedKgrams = 0;
};

std::size_t generate_candidates(const IndexBuilder& index, std::size_t minShared,
                                DynArray<CandidatePair>& out);

}  // namespace plag

#endif  // PLAG_SRC_SEARCH_CANDIDATE_GENERATOR_HPP_
