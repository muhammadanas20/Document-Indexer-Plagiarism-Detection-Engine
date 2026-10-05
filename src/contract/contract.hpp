// contract.hpp -- shared data types only. No behaviour.
// Owner: Anas.  FROZEN after Week 1: to change it, open an issue titled
// "contract: need <hook>" -- Anas adds it in a 5-line commit, nobody edits.
//
// Why data-only: both halves of the engine (Anas's index, Alyan's matcher,
// scorer, ranking and report) talk through these structs and nothing else.
// Plain structs cannot conflict.
#ifndef PLAG_SRC_CONTRACT_CONTRACT_HPP_
#define PLAG_SRC_CONTRACT_CONTRACT_HPP_

#include <cstddef>
#include <cstdint>

#include "common/dyn_array.hpp"
#include "contract/error_code.hpp"
#include "contract/span.hpp"

namespace plag {

using DocumentId = std::uint32_t;
using TokenId = std::uint64_t;   // FNV-1a of the normalised lexeme

// One place where a k-gram fingerprint was seen.
struct Occurrence {
  DocumentId docId = 0;
  std::uint32_t pos = 0;   // token index of the window's first token
};

// The value type of the index hash table: every place a fingerprint occurs.
// Appending (not prepending) is a deliberate choice -- cache locality.
using OccurrenceList = DynArray<Occurrence>;

// A lexeme, stored once, viewed twice: exact (for printing) and normalised
// (for comparing). Both views are slices of the same file buffer.
struct Token {
  std::uint32_t off = 0;
  std::uint32_t len = 0;
  std::uint32_t line = 0;
  TokenId id = 0;
};

// One candidate pair, after scoring. Alyan owns the formulas; the struct is
// shared so the report can print it.
struct PairScore {
  DocumentId docA = 0;
  DocumentId docB = 0;
  double jaccard = 0.0;
  double containment = 0.0;
  double coverage = 0.0;
  std::uint32_t matchedTokens = 0;
  std::uint32_t spans = 0;
  std::uint8_t label = 0;   // suspicion label, defined by verify_mode.hpp
};

// Everything --stats prints, and the report header.
struct IndexStats {
  std::size_t files = 0;
  std::size_t bytes = 0;
  std::size_t tokens = 0;
  std::size_t kgrams = 0;
  std::size_t distinct = 0;
  std::size_t buckets = 0;
  double alpha = 0.0;           // load factor = size / buckets
  double meanChain = 0.0;
  std::uint32_t longestChain = 0;
  double ms = 0.0;
  bool malformedSeen = false;   // set by the tokenizer on a broken file
};

}  // namespace plag

#endif  // PLAG_SRC_CONTRACT_CONTRACT_HPP_
