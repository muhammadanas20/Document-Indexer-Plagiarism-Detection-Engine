// candidate_generator.cpp -- STUB (Alyan, W5).
#include "search/candidate_generator.hpp"

#include "index/index_builder.hpp"

namespace plag {

std::size_t generate_candidates(const IndexBuilder& /*index*/,
                                std::size_t /*minShared*/,
                                DynArray<CandidatePair>& /*out*/) {
  // TODO(Alyan W5): reuse HashTable<K,V> as a (docA,docB) -> int counter.
  return 0;  // STUB dummy
}

}  // namespace plag
