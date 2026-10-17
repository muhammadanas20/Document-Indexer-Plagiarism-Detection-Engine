// Rabin-Karp rolling hash over k-token windows
#ifndef PLAG_SRC_INDEX_ROLLING_HASH_HPP_
#define PLAG_SRC_INDEX_ROLLING_HASH_HPP_

#include <cstddef>
#include <cstdint>

namespace plag {

struct RollingHash {
  std::uint64_t base = 0;
  std::uint64_t mod = 0;
  std::uint64_t highPow = 0;
  std::uint64_t value = 0;

  void init(const std::uint64_t* window, std::size_t k, std::uint64_t b,
            std::uint64_t p);
  void roll(std::uint64_t leaving, std::uint64_t entering);
  std::uint64_t hash() const;

  static std::uint64_t default_modulus();
  static std::uint64_t random_base();
};

}  // namespace plag

#endif  // PLAG_SRC_INDEX_ROLLING_HASH_HPP_
