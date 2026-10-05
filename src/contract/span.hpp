// span.hpp -- a maximal copied block between two documents.
// Owner: Anas.  FROZEN after Week 1.
#ifndef PLAG_SRC_CONTRACT_SPAN_HPP_
#define PLAG_SRC_CONTRACT_SPAN_HPP_

#include <cstdint>

namespace plag {

// Half-open token ranges: [startA, endA) in docA, [startB, endB) in docB.
// The invariant merge_spans() asserts: endA - startA == endB - startB.
struct Span {
  std::uint32_t docA = 0;
  std::uint32_t startA = 0;
  std::uint32_t endA = 0;
  std::uint32_t docB = 0;
  std::uint32_t startB = 0;
  std::uint32_t endB = 0;

  std::uint32_t tokens() const { return endA - startA; }
};

}  // namespace plag

#endif  // PLAG_SRC_CONTRACT_SPAN_HPP_
