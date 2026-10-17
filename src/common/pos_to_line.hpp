// Helper functions to map byte offsets to 1-based line numbers
#ifndef PLAG_SRC_COMMON_POS_TO_LINE_HPP_
#define PLAG_SRC_COMMON_POS_TO_LINE_HPP_

#include <cstddef>
#include <cstdint>

#include "common/dyn_array.hpp"

namespace plag {

// Record the starting byte offset of each line in the file
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

// Find the 1-based line number for a given byte offset using binary search
inline std::uint32_t pos_to_line(const DynArray<std::uint32_t>& line_starts,
                                 std::uint32_t off) {
  if (line_starts.empty()) return 1;
  std::size_t low = 0;
  std::size_t high = line_starts.size();
  while (high - low > 1) {
    std::size_t mid = low + (high - low) / 2;
    if (line_starts[mid] <= off) {
      low = mid;
    } else {
      high = mid;
    }
  }
  return static_cast<std::uint32_t>(low + 1);
}

}  // namespace plag

#endif  // PLAG_SRC_COMMON_POS_TO_LINE_HPP_
