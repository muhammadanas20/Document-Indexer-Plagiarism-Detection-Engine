// probing.hpp  --  Hash table with LINEAR PROBING (open addressing)
// (Week 13 topic: collision resolution techniques)
// Owner: Anas
//
// Why a second table?  The course teaches two ways to resolve collisions, so we
// use both and compare them in the program:
//   * separate chaining  -> the main inverted index (it must support delete)
//   * linear probing     -> counting the words of ONE document
//                           (insert + search only, no delete needed, so we do
//                            not need tombstones / deleted markers)
//
// Linear probing: if the home bucket is taken, try the next one, then the next,
// and wrap around at the end of the array.

#ifndef DIP_PROBING_HPP
#define DIP_PROBING_HPP

#include <cstddef>
#include <string>

#include "hash_table.hpp"  // for hashKey()

namespace dip {

// Small table: word -> how many times that word appears in one document.
class LinearProbeTable {
 public:
  explicit LinearProbeTable(std::size_t capacity = 16)
      : keys_(nullptr), values_(nullptr), used_(nullptr),
        capacity_(capacity), size_(0), probeCount_(0), insertCount_(0) {
    if (capacity_ < 4) capacity_ = 4;
    keys_ = new std::string[capacity_];
    values_ = new int[capacity_];
    used_ = new bool[capacity_];
    for (std::size_t i = 0; i < capacity_; ++i) {
      values_[i] = 0;
      used_[i] = false;
    }
  }

  ~LinearProbeTable() {
    delete[] keys_;
    delete[] values_;
    delete[] used_;
  }
  LinearProbeTable(const LinearProbeTable&) = delete;
  LinearProbeTable& operator=(const LinearProbeTable&) = delete;

  // Add 1 to the count of `word` (insert it the first time). Average O(1).
  void add(const std::string& word) {
    if (static_cast<double>(size_ + 1) / static_cast<double>(capacity_) > 0.75) {
      grow();
    }
    std::size_t index = static_cast<std::size_t>(hashKey(word) % capacity_);
    std::size_t probes = 1;
    // Probe forward until we find the word or an empty slot.
    while (used_[index] && keys_[index] != word) {
      index = (index + 1) % capacity_;
      ++probes;
    }
    if (!used_[index]) {
      keys_[index] = word;
      used_[index] = true;
      values_[index] = 0;
      ++size_;
    }
    ++values_[index];
    probeCount_ += probes;
    ++insertCount_;
  }

  // Count of a word, or 0 if it is not there. Average O(1).
  int count(const std::string& word) const {
    std::size_t index = static_cast<std::size_t>(hashKey(word) % capacity_);
    std::size_t checked = 0;
    while (used_[index] && checked < capacity_) {
      if (keys_[index] == word) return values_[index];
      index = (index + 1) % capacity_;
      ++checked;
    }
    return 0;
  }

  std::size_t size() const { return size_; }
  std::size_t capacity() const { return capacity_; }
  double loadFactor() const {
    return static_cast<double>(size_) / static_cast<double>(capacity_);
  }
  // Average number of slots we had to look at while inserting.
  // With chaining this corresponds to the average chain length.
  double averageProbes() const {
    if (insertCount_ == 0) return 0.0;
    return static_cast<double>(probeCount_) / static_cast<double>(insertCount_);
  }

 private:
  // Double the table and re-insert all the old words.
  void grow() {
    std::size_t newCapacity = capacity_ * 2;
    std::string* oldKeys = keys_;
    int* oldValues = values_;
    bool* oldUsed = used_;
    std::size_t oldCapacity = capacity_;

    keys_ = new std::string[newCapacity];
    values_ = new int[newCapacity];
    used_ = new bool[newCapacity];
    for (std::size_t i = 0; i < newCapacity; ++i) {
      values_[i] = 0;
      used_[i] = false;
    }
    capacity_ = newCapacity;
    size_ = 0;

    for (std::size_t i = 0; i < oldCapacity; ++i) {
      if (!oldUsed[i]) continue;
      for (int k = 0; k < oldValues[i]; ++k) add(oldKeys[i]);
    }
    delete[] oldKeys;
    delete[] oldValues;
    delete[] oldUsed;
  }

  std::string* keys_;  // the word stored in each slot
  int* values_;        // its count
  bool* used_;         // false = slot is empty
  std::size_t capacity_;
  std::size_t size_;
  std::size_t probeCount_;
  std::size_t insertCount_;
};

}  // namespace dip

#endif  // DIP_PROBING_HPP
