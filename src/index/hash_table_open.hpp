// Open-addressing hash table using linear probing and tombstones
#ifndef PLAG_SRC_INDEX_HASH_TABLE_OPEN_HPP_
#define PLAG_SRC_INDEX_HASH_TABLE_OPEN_HPP_

#include <cstddef>
#include <cstdint>

#include "common/fnv.hpp"

namespace plag {

template <typename K, typename V>
class OpenHashTable {
 public:
  OpenHashTable()
      : slots_(nullptr),
        keys_(nullptr),
        vals_(nullptr),
        cap_(16),
        size_(0),
        used_(0),
        probes_(0),
        lookups_(0),
        max_probe_(0) {
    allocate_tables(cap_);
  }

  ~OpenHashTable() {
    delete[] slots_;
    delete[] keys_;
    delete[] vals_;
  }

  OpenHashTable(const OpenHashTable&) = delete;
  OpenHashTable& operator=(const OpenHashTable&) = delete;

  V& insert(const K& k, const V& v) {
    if ((used_ + 1) * 10 >= cap_ * 6) {
      grow();
    }

    std::size_t idx = index_of(static_cast<std::uint64_t>(k));
    std::size_t first_deleted = cap_;
    std::size_t steps = 1;

    while (slots_[idx] != Slot::Empty) {
      if (slots_[idx] == Slot::Occupied && keys_[idx] == k) {
        vals_[idx] = v;
        record_probe(steps);
        return vals_[idx];
      }
      if (slots_[idx] == Slot::Deleted && first_deleted == cap_) {
        first_deleted = idx;
      }
      idx = (idx + 1) % cap_;
      ++steps;
    }

    record_probe(steps);
    std::size_t target = (first_deleted != cap_) ? first_deleted : idx;
    if (slots_[target] == Slot::Empty) {
      ++used_;
    }
    slots_[target] = Slot::Occupied;
    keys_[target] = k;
    vals_[target] = v;
    ++size_;
    return vals_[target];
  }

  V* find(const K& k) {
    std::size_t idx = index_of(static_cast<std::uint64_t>(k));
    std::size_t steps = 1;
    while (slots_[idx] != Slot::Empty) {
      if (slots_[idx] == Slot::Occupied && keys_[idx] == k) {
        record_probe(steps);
        return &vals_[idx];
      }
      idx = (idx + 1) % cap_;
      ++steps;
      if (steps > cap_) break;
    }
    record_probe(steps);
    return nullptr;
  }

  const V* find(const K& k) const {
    std::size_t idx = index_of(static_cast<std::uint64_t>(k));
    std::size_t steps = 1;
    while (slots_[idx] != Slot::Empty) {
      if (slots_[idx] == Slot::Occupied && keys_[idx] == k) {
        return &vals_[idx];
      }
      idx = (idx + 1) % cap_;
      ++steps;
      if (steps > cap_) break;
    }
    return nullptr;
  }

  bool erase(const K& k) {
    std::size_t idx = index_of(static_cast<std::uint64_t>(k));
    std::size_t steps = 1;
    while (slots_[idx] != Slot::Empty) {
      if (slots_[idx] == Slot::Occupied && keys_[idx] == k) {
        slots_[idx] = Slot::Deleted;
        --size_;
        return true;
      }
      idx = (idx + 1) % cap_;
      ++steps;
      if (steps > cap_) break;
    }
    return false;
  }

  std::size_t bucketCount() const { return cap_; }
  std::size_t size() const { return size_; }

  double meanProbeLength() const {
    if (lookups_ == 0) return 0.0;
    return static_cast<double>(probes_) / static_cast<double>(lookups_);
  }

  std::size_t maxProbeLength() const { return max_probe_; }

  // Compatibility helpers for the shared hash table test suite
  double meanChainLength() const { return meanProbeLength(); }
  std::size_t longestChain() const { return maxProbeLength(); }

 private:
  enum class Slot : std::uint8_t { Empty = 0, Occupied, Deleted };

  void allocate_tables(std::size_t c) {
    slots_ = new Slot[c];
    keys_ = new K[c];
    vals_ = new V[c];
    for (std::size_t i = 0; i < c; ++i) {
      slots_[i] = Slot::Empty;
    }
  }

  void record_probe(std::size_t steps) {
    probes_ += steps;
    ++lookups_;
    if (steps > max_probe_) {
      max_probe_ = steps;
    }
  }

  std::size_t index_of(std::uint64_t h) const {
    return static_cast<std::size_t>(fnv_mix(h) % cap_);
  }

  void grow() {
    std::size_t old_cap = cap_;
    Slot* old_slots = slots_;
    K* old_keys = keys_;
    V* old_vals = vals_;

    cap_ = old_cap * 2;
    size_ = 0;
    used_ = 0;
    max_probe_ = 0;
    allocate_tables(cap_);

    for (std::size_t i = 0; i < old_cap; ++i) {
      if (old_slots[i] == Slot::Occupied) {
        insert(old_keys[i], old_vals[i]);
      }
    }

    delete[] old_slots;
    delete[] old_keys;
    delete[] old_vals;
  }

  Slot* slots_;
  K* keys_;
  V* vals_;
  std::size_t cap_;
  std::size_t size_;
  std::size_t used_;
  std::size_t probes_;
  std::size_t lookups_;
  std::size_t max_probe_;
};

}  // namespace plag

#endif  // PLAG_SRC_INDEX_HASH_TABLE_OPEN_HPP_
