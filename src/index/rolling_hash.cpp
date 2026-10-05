// rolling_hash.cpp -- STUB (Anas, W2 / Step 4).
#include "index/rolling_hash.hpp"

namespace plag {

void RollingHash::init(const std::uint64_t* /*window*/, std::size_t /*k*/,
                       std::uint64_t b, std::uint64_t p) {
  // TODO(Anas W2 Step 4): fold the k tokens in, cache highPow = b^(k-1) mod p.
  base = b;
  mod = p;
  highPow = 0;
  value = 0;
}

void RollingHash::roll(std::uint64_t /*leaving*/, std::uint64_t /*entering*/) {
  // TODO(Anas W2 Step 4):
  //   value = (value + mod - mulmod(leaving % mod, highPow)) % mod;
  //   value = (mulmod(value, base) + entering % mod) % mod;
  // Add the modulus BEFORE subtracting -- the classic sign bug lives here.
}

std::uint64_t RollingHash::hash() const { return value; }

std::uint64_t RollingHash::default_modulus() {
  return (std::uint64_t{1} << 61) - 1;  // 2^61 - 1
}

std::uint64_t RollingHash::random_base() {
  // TODO(Anas W2 Step 4): seed from std::random_device, force odd, > 256.
  return 0;  // STUB dummy
}

}  // namespace plag
