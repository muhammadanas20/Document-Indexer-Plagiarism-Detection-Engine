#ifndef PROJECT_TABLES_HPP
#define PROJECT_TABLES_HPP

#include <cstddef>
#include <string>
#include <utility>
#include <vector>
#include "structures.hpp"

// hash table
inline unsigned long long hashKey(const std::string& key) {
  unsigned long long hash = 5381;
  for (std::size_t i = 0; i < key.size(); ++i) {
    hash = hash * 31ULL + static_cast<unsigned char>(key[i]);
  }
  return hash;
}

inline unsigned long long hashKey(unsigned long long key) { return key; }
inline unsigned long long hashKey(long long key) {
  return static_cast<unsigned long long>(key);
}
inline unsigned long long hashKey(int key) {
  return static_cast<unsigned long long>(key);
}

template <typename K, typename V>
class HashTable {
 public:
  typedef LinkedList<std::pair<K, V> > Bucket;

  explicit HashTable(std::size_t bucketCount = 16)
      : buckets_(nullptr), bucketCount_(bucketCount), size_(0) {
    if (bucketCount_ < 4) bucketCount_ = 4;
    buckets_ = new Bucket*[bucketCount_];
    for (std::size_t i = 0; i < bucketCount_; ++i) buckets_[i] = new Bucket();
  }

  ~HashTable() {
    for (std::size_t i = 0; i < bucketCount_; ++i) delete buckets_[i];
    delete[] buckets_;
  }
  HashTable(const HashTable&) = delete;
  HashTable& operator=(const HashTable&) = delete;

  V* put(const K& key, const V& value) {

    if (static_cast<double>(size_ + 1) / static_cast<double>(bucketCount_) > 0.75) {
      rehash(bucketCount_ * 2);
    }
    // words with the same bucket stay in one linked list
    std::size_t index = bucketIndex(key);
    for (typename Bucket::Node* n = buckets_[index]->head(); n != nullptr; n = n->next) {
      if (n->value.first == key) {
        n->value.second = value;
        return &n->value.second;
      }
    }
    buckets_[index]->pushFront(std::make_pair(key, value));
    ++size_;
    return &buckets_[index]->head()->value.second;
  }

  V* get(const K& key) {
    // words with the same bucket stay in one linked list
    std::size_t index = bucketIndex(key);
    for (typename Bucket::Node* n = buckets_[index]->head(); n != nullptr; n = n->next) {
      if (n->value.first == key) return &n->value.second;
    }
    return nullptr;
  }

  const V* get(const K& key) const {
    // words with the same bucket stay in one linked list
    std::size_t index = bucketIndex(key);
    for (typename Bucket::Node* n = buckets_[index]->head(); n != nullptr; n = n->next) {
      if (n->value.first == key) return &n->value.second;
    }
    return nullptr;
  }

  void clear() {
    for (std::size_t i = 0; i < bucketCount_; ++i) buckets_[i]->clear();
    size_ = 0;
  }

  bool contains(const K& key) { return get(key) != nullptr; }

  bool remove(const K& key) {
    // words with the same bucket stay in one linked list
    std::size_t index = bucketIndex(key);
    for (typename Bucket::Node* n = buckets_[index]->head(); n != nullptr; n = n->next) {
      if (n->value.first == key) {

        std::pair<K, V> wanted = n->value;
        buckets_[index]->remove(wanted);
        --size_;
        return true;
      }
    }
    return false;
  }

  std::size_t size() const { return size_; }
  std::size_t bucketCount() const { return bucketCount_; }
  double loadFactor() const {
    return static_cast<double>(size_) / static_cast<double>(bucketCount_);
  }

  std::size_t longestChain() const {
    std::size_t longest = 0;
    for (std::size_t i = 0; i < bucketCount_; ++i) {
      if (buckets_[i]->size() > longest) longest = buckets_[i]->size();
    }
    return longest;
  }

  const Bucket* bucket(std::size_t index) const { return buckets_[index]; }

 private:
  std::size_t bucketIndex(const K& key) const {
    return static_cast<std::size_t>(hashKey(key) % bucketCount_);
  }

  void rehash(std::size_t newBucketCount) {
    Bucket** newBuckets = new Bucket*[newBucketCount];
    for (std::size_t i = 0; i < newBucketCount; ++i) newBuckets[i] = new Bucket();

    for (std::size_t i = 0; i < bucketCount_; ++i) {
      for (typename Bucket::Node* n = buckets_[i]->head(); n != nullptr; n = n->next) {
        std::size_t index =
            static_cast<std::size_t>(hashKey(n->value.first) % newBucketCount);
        newBuckets[index]->pushFront(n->value);
      }
      delete buckets_[i];
    }
    delete[] buckets_;
    buckets_ = newBuckets;
    bucketCount_ = newBucketCount;
  }

  Bucket** buckets_;
  std::size_t bucketCount_;
  std::size_t size_;
};

// probing
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

  void add(const std::string& word) {
    if (static_cast<double>(size_ + 1) / static_cast<double>(capacity_) > 0.75) {
      grow();
    }
    std::size_t index = static_cast<std::size_t>(hashKey(word) % capacity_);
    std::size_t probes = 1;

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

  double averageProbes() const {
    if (insertCount_ == 0) return 0.0;
    return static_cast<double>(probeCount_) / static_cast<double>(insertCount_);
  }

 private:

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

  std::string* keys_;
  int* values_;
  bool* used_;
  std::size_t capacity_;
  std::size_t size_;
  std::size_t probeCount_;
  std::size_t insertCount_;
};

#endif
