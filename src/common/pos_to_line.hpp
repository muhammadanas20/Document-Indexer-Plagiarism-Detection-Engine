// pos_to_line.hpp -- byte offset -> 1-based line number.
// Owner: Anas (shared, FROZEN after Week 1). Lives in src/common/ because the
// tokenizer fills it and Alyan's report reads it.
// WORKING and TESTED (tests/test_anas_common.cpp).
//
// Built once per file in a single pass; Alyan's report must never re-read a
// file just to print a line number.
#ifndef PLAG_SRC_COMMON_POS_TO_LINE_HPP_
#define PLAG_SRC_COMMON_POS_TO_LINE_HPP_

#include <cstddef>
#include <cstdint>

#include "common/dyn_array.hpp"

namespace plag {

// line_starts[i] = byte offset of the first character of line i+1.
inline void build_line_starts(const std::uint8_t* data, std::size_t n,
                              DynArray<std::uint32_t>& line_starts) {
  line_starts.clear();
  line_starts.push_back(0u);
  for (std::size_t i = 0; i < n; ++i) {
    if (data[i] == static_cast<std::uint8_t>('\n')) {
      line_starts.push_back(static_cast<std::uint32_t>(i + 1));
    }
  }
}

// Binary search: the last line start <= off. 1-based, clamped to >= 1.
inline std::uint32_t pos_to_line(const DynArray<std::uint32_t>& line_starts,
                                 std::uint32_t off) {
  if (line_starts.empty()) return 1;
  std::size_t lo = 0;
  std::size_t hi = line_starts.size();  // one past the last candidate
  while (hi - lo > 1) {
    const std::size_t mid = lo + (hi - lo) / 2;
    if (line_starts[mid] <= off) {
      lo = mid;
    } else {
      hi = mid;
    }
  }
  return static_cast<std::uint32_t>(lo + 1);
}

}  // namespace plag

#endif  // PLAG_SRC_COMMON_POS_TO_LINE_HPP_
