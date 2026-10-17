// Modular arithmetic helpers for Rabin-Karp rolling hash
#ifndef PLAG_SRC_COMMON_MULMOD_HPP_
#define PLAG_SRC_COMMON_MULMOD_HPP_

#include <cstdint>

namespace plag {

// Prime modulus 2^61 - 1
inline constexpr std::uint64_t kMersenne61 = (std::uint64_t{1} << 61) - 1;

// Compute (a * b) % p using binary multiplication to avoid 64-bit overflow
inline std::uint64_t mulmod(std::uint64_t a, std::uint64_t b, std::uint64_t p) {
  std::uint64_t res = 0;
  a %= p;
  b %= p;
  while (b > 0) {
    if (b & 1ULL) {
      if (res >= p - a) {
        res -= (p - a);
      } else {
        res += a;
      }
    }
    if (a >= p - a) {
      a -= (p - a);
    } else {
      a += a;
    }
    b >>= 1;
  }
  return res;
}

// Compute (base^exp) % p using binary exponentiation
inline std::uint64_t powmod(std::uint64_t base, std::uint64_t exp,
                            std::uint64_t p) {
  std::uint64_t result = 1 % p;
  std::uint64_t b = base % p;
  while (exp > 0) {
    if (exp & 1ULL) {
      result = mulmod(result, b, p);
    }
    b = mulmod(b, b, p);
    exp >>= 1;
  }
  return result;
}

// Compute (a - b) % p safely without unsigned underflow
inline std::uint64_t submod(std::uint64_t a, std::uint64_t b, std::uint64_t p) {
  a %= p;
  b %= p;
  return (a + p - b) % p;
}

}  // namespace plag

#endif  // PLAG_SRC_COMMON_MULMOD_HPP_
