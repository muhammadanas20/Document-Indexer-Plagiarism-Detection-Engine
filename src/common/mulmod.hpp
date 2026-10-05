// mulmod.hpp -- (a*b) mod p without overflow, plus powmod.
// Owner: Anas (shared, FROZEN after Week 1).
// WORKING and TESTED (tests/test_anas_common.cpp) against a 128-bit reference.
//
// a*b of two 64-bit numbers does not fit in 64 bits, so the product is taken
// in 128-bit and reduced once. Never returns a negative value: everything
// here is unsigned, so there is no sign to get wrong.
#ifndef PLAG_SRC_COMMON_MULMOD_HPP_
#define PLAG_SRC_COMMON_MULMOD_HPP_

#include <cstdint>

namespace plag {

// 2^61 - 1: prime, and convenient because a 64-bit product reduced modulo it
// still fits comfortably in a uint64_t.
inline constexpr std::uint64_t kMersenne61 = (std::uint64_t{1} << 61) - 1;

// Requires p > 0. Uses the 128-bit intermediate that gcc/clang provide.
inline std::uint64_t mulmod(std::uint64_t a, std::uint64_t b, std::uint64_t p) {
  using u128 = unsigned __int128;
  return static_cast<std::uint64_t>((static_cast<u128>(a % p) * (b % p)) % p);
}

// base^exp mod p, by squaring: O(log exp).
inline std::uint64_t powmod(std::uint64_t base, std::uint64_t exp,
                            std::uint64_t p) {
  std::uint64_t result = 1 % p;
  std::uint64_t b = base % p;
  while (exp > 0) {
    if ((exp & 1U) != 0U) result = mulmod(result, b, p);
    b = mulmod(b, b, p);
    exp >>= 1;
  }
  return result;
}

// (a - b) mod p, done safely: add the modulus before subtracting. This is the
// line that prevents the rolling hash from going "negative".
inline std::uint64_t submod(std::uint64_t a, std::uint64_t b, std::uint64_t p) {
  return (a % p + p - b % p) % p;
}

}  // namespace plag

#endif  // PLAG_SRC_COMMON_MULMOD_HPP_
