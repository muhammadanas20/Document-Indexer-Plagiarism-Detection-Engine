// Knuth-Morris-Pratt (KMP) string matching algorithm over token ID arrays
#ifndef PLAG_SRC_SEARCH_KMP_HPP_
#define PLAG_SRC_SEARCH_KMP_HPP_

#include <cstddef>
#include <cstdint>

#include "common/dyn_array.hpp"

namespace plag {

void build_lps(const std::uint64_t* pat, std::size_t m,
               DynArray<std::uint32_t>& lps);

std::size_t kmp_find_all(const std::uint64_t* text, std::size_t n,
                         const std::uint64_t* pat, std::size_t m,
                         DynArray<std::uint32_t>& out);

}  // namespace plag

#endif  // PLAG_SRC_SEARCH_KMP_HPP_
