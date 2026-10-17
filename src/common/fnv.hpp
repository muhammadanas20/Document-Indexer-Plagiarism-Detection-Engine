// FNV-1a 64-bit hash functions for strings and byte arrays
#ifndef PLAG_SRC_COMMON_FNV_HPP_
#define PLAG_SRC_COMMON_FNV_HPP_

#include <cstddef>
#include <cstdint>

namespace plag {

inline constexpr std::uint64_t kFnvOffsetBasis = 0xcbf29ce484222325ULL;
inline constexpr std::uint64_t kFnvPrime = 0x100000001b3ULL;

// Compute 64-bit FNV-1a hash of a raw buffer
inline std::uint64_t fnv1a(const void* data, std::size_t n) {
  const unsigned char* bytes = static_cast<const unsigned char*>(data);
  std::uint64_t hash = kFnvOffsetBasis;
  for (std::size_t i = 0; i < n; ++i) {
    hash ^= static_cast<std::uint64_t>(bytes[i]);
    hash *= kFnvPrime;
  }
  return hash;
}

// Compute 64-bit FNV-1a hash of a null-terminated string
inline std::uint64_t fnv1a(const char* s) {
  std::size_t len = 0;
  while (s[len] != '\0') {
    ++len;
  }
  return fnv1a(s, len);
}

// Mix high and low bits of a 64-bit hash value
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
