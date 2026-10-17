// Represents a matching token span between two documents
#ifndef PLAG_SRC_CONTRACT_SPAN_HPP_
#define PLAG_SRC_CONTRACT_SPAN_HPP_

#include <cstdint>

namespace plag {

// Half-open token intervals [startA, endA) in docA and [startB, endB) in docB
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
