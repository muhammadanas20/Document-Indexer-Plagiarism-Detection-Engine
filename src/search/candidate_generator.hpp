// candidate_generator.hpp -- which document pairs are worth verifying.
// Owner: ALYAN.  src/search/ is Alyan's area.
// STUB (Alyan, W5): dummy bodies. Depends on Anas's IndexBuilder::lookup().
#ifndef PLAG_SRC_SEARCH_CANDIDATE_GENERATOR_HPP_
#define PLAG_SRC_SEARCH_CANDIDATE_GENERATOR_HPP_

#include <cstddef>

#include "common/dyn_array.hpp"
#include "contract/contract.hpp"

namespace plag {

class IndexBuilder;  // Anas's index -- forward-declared, never owned here.

struct CandidatePair {
  DocumentId docA = 0;
  DocumentId docB = 0;
  std::uint32_t sharedKgrams = 0;
};

// Reads shared fingerprints out of the index and returns the pairs above a
// threshold, so the expensive verification only runs on real suspects.
std::size_t generate_candidates(const IndexBuilder& index, std::size_t minShared,
                                DynArray<CandidatePair>& out);

}  // namespace plag

#endif  // PLAG_SRC_SEARCH_CANDIDATE_GENERATOR_HPP_
