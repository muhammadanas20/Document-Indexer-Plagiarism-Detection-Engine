// Hash table using separate chaining (no STL containers)
#ifndef PLAG_SRC_INDEX_HASH_TABLE_HPP_
#define PLAG_SRC_INDEX_HASH_TABLE_HPP_

#include <cstddef>
#include <cstdint>

#include "common/fnv.hpp"

namespace plag {

template <typename K, typename V>
class HashTable {
 public:
  HashTable() : buckets_(nullptr), cap_(16), size_(0) {
    buckets_ = new Node*[cap_];
    for (std::size_t i = 0; i < cap_; ++i) {
      buckets_[i] = nullptr;
    }
  }

  ~HashTable() {
    clear_nodes();
    delete[] buckets_;
    buckets_ = nullptr;
  }

  HashTable(const HashTable&) = delete;
  HashTable& operator=(const HashTable&) = delete;

  // Insert key-value pair or update existing value; returns reference to value
  V& insert(const K& k, const V& v) {
    if (size_ + 1 > (cap_ * 3) / 4) {
      grow();
    }
    std::size_t idx = index_of(static_cast<std::uint64_t>(k));
    Node* cur = buckets_[idx];
    while (cur != nullptr) {
      if (cur->key == k) {
        cur->val = v;
        return cur->val;
      }
      cur = cur->next;
    }

    Node* node = new Node{k, v, buckets_[idx]};
    buckets_[idx] = node;
    ++size_;
    return node->val;
  }

  V* find(const K& k) {
    if (buckets_ == nullptr || cap_ == 0) return nullptr;
    std::size_t idx = index_of(static_cast<std::uint64_t>(k));
    Node* cur = buckets_[idx];
    while (cur != nullptr) {
      if (cur->key == k) {
        return &cur->val;
      }
      cur = cur->next;
    }
    return nullptr;
  }

  const V* find(const K& k) const {
    if (buckets_ == nullptr || cap_ == 0) return nullptr;
    std::size_t idx = index_of(static_cast<std::uint64_t>(k));
    const Node* cur = buckets_[idx];
    while (cur != nullptr) {
      if (cur->key == k) {
        return &cur->val;
      }
      cur = cur->next;
    }
    return nullptr;
  }

  bool erase(const K& k) {
    if (buckets_ == nullptr || cap_ == 0) return false;
    std::size_t idx = index_of(static_cast<std::uint64_t>(k));
    Node* cur = buckets_[idx];
    Node* prev = nullptr;
    while (cur != nullptr) {
      if (cur->key == k) {
        if (prev == nullptr) {
          buckets_[idx] = cur->next;
        } else {
          prev->next = cur->next;
        }
        delete cur;
        --size_;
        return true;
      }
      prev = cur;
      cur = cur->next;
    }
    return false;
  }

  std::size_t bucketCount() const { return cap_; }
  std::size_t size() const { return size_; }

  double meanChainLength() const {
    if (cap_ == 0) return 0.0;
    return static_cast<double>(size_) / static_cast<double>(cap_);
  }

  std::size_t longestChain() const {
    std::size_t best = 0;
    for (std::size_t i = 0; i < cap_; ++i) {
      std::size_t len = 0;
      const Node* cur = buckets_[i];
      while (cur != nullptr) {
        ++len;
        cur = cur->next;
      }
      if (len > best) {
        best = len;
      }
    }
    return best;
  }

  template <typename Func>
  void for_each_bucket(Func fn) const {
    for (std::size_t i = 0; i < cap_; ++i) {
      const Node* cur = buckets_[i];
      while (cur != nullptr) {
        fn(cur->key, cur->val);
        cur = cur->next;
      }
    }
  }

 private:
  struct Node {
    K key;
    V val;
    Node* next;
  };

  std::size_t index_of(std::uint64_t h) const {
    return static_cast<std::size_t>(fnv_mix(h) % cap_);
  }

  void clear_nodes() {
    if (buckets_ == nullptr) return;
    for (std::size_t i = 0; i < cap_; ++i) {
      Node* cur = buckets_[i];
      while (cur != nullptr) {
        Node* next = cur->next;
        delete cur;
        cur = next;
      }
      buckets_[i] = nullptr;
    }
    size_ = 0;
  }

  void grow() {
    std::size_t old_cap = cap_;
    Node** old_buckets = buckets_;

    cap_ = old_cap * 2;
    buckets_ = new Node*[cap_];
    for (std::size_t i = 0; i < cap_; ++i) {
      buckets_[i] = nullptr;
    }

    for (std::size_t i = 0; i < old_cap; ++i) {
      Node* cur = old_buckets[i];
      while (cur != nullptr) {
        Node* next = cur->next;
        std::size_t idx = index_of(static_cast<std::uint64_t>(cur->key));
        cur->next = buckets_[idx];
        buckets_[idx] = cur;
        cur = next;
      }
    }
    delete[] old_buckets;
  }

  Node** buckets_;
  std::size_t cap_;
  std::size_t size_;
};

}  // namespace plag

#endif  // PLAG_SRC_INDEX_HASH_TABLE_HPP_
