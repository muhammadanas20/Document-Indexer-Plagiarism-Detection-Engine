// kmp.cpp -- STUB (Alyan, W2). Anas must not implement this.
#include "search/kmp.hpp"

namespace plag {

void build_lps(const std::uint64_t* /*pat*/, std::size_t /*m*/,
               DynArray<std::uint32_t>& /*lps*/) {
  // TODO(Alyan W2): LPS table.
}

std::size_t kmp_find_all(const std::uint64_t* /*text*/, std::size_t /*n*/,
                         const std::uint64_t* /*pat*/, std::size_t /*m*/,
                         DynArray<std::uint32_t>& /*out*/) {
  // TODO(Alyan W2): KMP scan; verified against a naive-search oracle.
  return 0;  // STUB dummy
}

}  // namespace plag
