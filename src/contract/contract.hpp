// Shared data structures used across indexing, search, ranking, and reporting
#ifndef PLAG_SRC_CONTRACT_CONTRACT_HPP_
#define PLAG_SRC_CONTRACT_CONTRACT_HPP_

#include <cstddef>
#include <cstdint>

#include "common/dyn_array.hpp"
#include "contract/error_code.hpp"
#include "contract/span.hpp"

namespace plag {

using DocumentId = std::uint32_t;
using TokenId = std::uint64_t;

// Location where a k-gram fingerprint appeared
struct Occurrence {
  DocumentId docId = 0;
  std::uint32_t pos = 0;
};

using OccurrenceList = DynArray<Occurrence>;

// Single token information
struct Token {
  std::uint32_t off = 0;
  std::uint32_t len = 0;
  std::uint32_t line = 0;
  TokenId id = 0;
};

// Similarity score summary for a pair of documents
struct PairScore {
  DocumentId docA = 0;
  DocumentId docB = 0;
  double jaccard = 0.0;
  double containment = 0.0;
  double coverage = 0.0;
  std::uint32_t matchedTokens = 0;
  std::uint32_t spans = 0;
  std::uint8_t label = 0;
};

// Summary statistics for the indexed corpus
struct IndexStats {
  std::size_t files = 0;
  std::size_t bytes = 0;
  std::size_t tokens = 0;
  std::size_t kgrams = 0;
  std::size_t distinct = 0;
  std::size_t buckets = 0;
  double alpha = 0.0;
  double meanChain = 0.0;
  std::uint32_t longestChain = 0;
  double ms = 0.0;
  bool malformedSeen = false;
};

}  // namespace plag

#endif  // PLAG_SRC_CONTRACT_CONTRACT_HPP_
