// hash_table_open.hpp -- open addressing (linear probing + tombstones).
// Owner: Anas.  src/index/ is Anas's area.
//
// STUB (W5 / Step 7a): inline dummy bodies so the project links. This variant
// exists to be BENCHMARKED against chaining, not to run the engine.
#ifndef PLAG_SRC_INDEX_HASH_TABLE_OPEN_HPP_
#define PLAG_SRC_INDEX_HASH_TABLE_OPEN_HPP_

#include <cstddef>
#include <cstdint>

namespace plag {

// Flat array, EMPTY / OCCUPIED / DELETED per slot, linear probe i=(i+1)%cap.
//
// The classic tombstone bug: lookup must walk PAST DELETED and stop only at
// EMPTY. The tombstone test is written first and watched to fail.
// Grows at alpha = 0.6, earlier than chaining's 0.75, because linear probing
// clusters.
template <class K, class V>
class OpenHashTable {
 public:
  OpenHashTable() = default;

  ~OpenHashTable() {
    delete[] slots_;
    slots_ = nullptr;
  }

  OpenHashTable(const OpenHashTable&) = delete;
  OpenHashTable& operator=(const OpenHashTable&) = delete;

  V& insert(const K& k, const V& v) {
    (void)k;
    (void)v;
    // TODO(Anas W5 Step 7a): probe past OCCUPIED, reuse a DELETED slot, grow
    // at alpha 0.6.
    static V stub_value{};
    return stub_value;
  }

  V* find(const K& k) {
    (void)k;
    // TODO(Anas W5 Step 7a): probe until EMPTY; DELETED does not end the walk.
    return nullptr;
  }

  const V* find(const K& k) const {
    (void)k;
    return nullptr;
  }

  bool erase(const K& k) {
    (void)k;
    return false;  // TODO(Anas W5 Step 7a): mark DELETED, never clear.
  }

  std::size_t bucketCount() const { return cap_; }
  std::size_t size() const { return size_; }

  double meanProbeLength() const {
    return cap_ == 0 ? 0.0 : static_cast<double>(probes_) / static_cast<double>(lookups_ + 1);
  }

  std::size_t maxProbeLength() const { return max_probe_; }

 private:
  enum class Slot : std::uint8_t { Empty = 0, Occupied, Deleted };

  std::size_t index_of(std::uint64_t h) const {
    return static_cast<std::size_t>((h ^ (h >> 32)) % cap_);
  }

  void grow() {
    // TODO(Anas W5 Step 7a): tombstones are dropped on rehash.
  }

  Slot* slots_ = nullptr;
  K* keys_ = nullptr;
  V* vals_ = nullptr;
  std::size_t cap_ = 16;
  std::size_t size_ = 0;
  std::size_t probes_ = 0;
  std::size_t lookups_ = 0;
  std::size_t max_probe_ = 0;
};

}  // namespace plag

#endif  // PLAG_SRC_INDEX_HASH_TABLE_OPEN_HPP_
