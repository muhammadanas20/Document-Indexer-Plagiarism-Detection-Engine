// Implementation of KMP pattern matching
#include "search/kmp.hpp"

namespace plag {

void build_lps(const std::uint64_t* pat, std::size_t m,
               DynArray<std::uint32_t>& lps) {
  lps.clear();
  if (pat == nullptr || m == 0) {
    return;
  }

  lps.reserve(m);
  for (std::size_t i = 0; i < m; ++i) {
    lps.push_back(0u);
  }

  std::size_t len = 0;
  std::size_t i = 1;
  while (i < m) {
    if (pat[i] == pat[len]) {
      ++len;
      lps[i] = static_cast<std::uint32_t>(len);
      ++i;
    } else {
      if (len != 0) {
        len = lps[len - 1];
      } else {
        lps[i] = 0u;
        ++i;
      }
    }
  }
}

std::size_t kmp_find_all(const std::uint64_t* text, std::size_t n,
                         const std::uint64_t* pat, std::size_t m,
                         DynArray<std::uint32_t>& out) {
  out.clear();
  if (text == nullptr || pat == nullptr || m == 0 || n < m) {
    return 0;
  }

  DynArray<std::uint32_t> lps;
  build_lps(pat, m, lps);

  std::size_t i = 0;
  std::size_t j = 0;
  while (i < n) {
    if (text[i] == pat[j]) {
      ++i;
      ++j;
      if (j == m) {
        out.push_back(static_cast<std::uint32_t>(i - j));
        j = lps[j - 1];
      }
    } else {
      if (j != 0) {
        j = lps[j - 1];
      } else {
        ++i;
      }
    }
  }

  return out.size();
}

}  // namespace plag
