// fnv.hpp -- FNV-1a, header-only, used for every lexeme and every key.
// Owner: Anas (shared, FROZEN after Week 1).
// WORKING and TESTED (tests/test_anas_common.cpp) against a known vector.
//
// FNV-1a hashes the byte first, then multiplies. That order is the whole
// reason it beats FNV-1 on short keys: the xor lands in the low bits before
// the multiply spreads it upward.
#ifndef PLAG_SRC_COMMON_FNV_HPP_
#define PLAG_SRC_COMMON_FNV_HPP_

#include <cstddef>
#include <cstdint>

namespace plag {

// 0xcbf29ce484222325 and 0x100000001b3 -- the standard FNV-1a 64-bit pair.
// Pinned by tests/test_anas_common.cpp against published test vectors, so a
// typo here cannot survive a test run.
inline constexpr std::uint64_t kFnvOffsetBasis = 0xcbf29ce484222325ULL;
inline constexpr std::uint64_t kFnvPrime = 0x100000001b3ULL;

inline std::uint64_t fnv1a(const void* data, std::size_t n) {
  const auto* p = static_cast<const unsigned char*>(data);
  std::uint64_t h = kFnvOffsetBasis;
  for (std::size_t i = 0; i < n; ++i) {
    h ^= static_cast<std::uint64_t>(p[i]);
    h *= kFnvPrime;
  }
  return h;
}

inline std::uint64_t fnv1a(const char* s) {
  std::size_t n = 0;
  while (s[n] != '\0') ++n;
  return fnv1a(s, n);
}

// Finaliser: fold the high 32 bits into the low ones. Cheap insurance for
// bucket indexing when a caller uses h % capacity directly.
inline std::uint64_t fnv_mix(std::uint64_t h) {
  h ^= h >> 33;
  h *= 0xff51afd7ed558ccdULL;
  h ^= h >> 33;
  h *= 0xc4ceb9fe1a85ec53ULL;
  h ^= h >> 33;
  return h;
}

}  // namespace plag

#endif  // PLAG_SRC_COMMON_FNV_HPP_
