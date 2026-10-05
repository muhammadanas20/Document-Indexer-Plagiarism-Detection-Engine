// hash_table.hpp -- separate-chaining map written from scratch (no STL).
// Owner: Anas.  src/index/ is Anas's area.
//
// STUB (W3 / Step 5): the members are defined inline with dummy bodies so the
// whole project links from Week 0 -- "stub first, so the other person never
// waits". The real implementation replaces these bodies in Step 5 and stays in
// this header, because a template's definition must be visible to its users.
// This is the graded centrepiece: the table everything else leans on.
#ifndef PLAG_SRC_INDEX_HASH_TABLE_HPP_
#define PLAG_SRC_INDEX_HASH_TABLE_HPP_

#include <cstddef>
#include <cstdint>

namespace plag {

// Key -> value map, separate chaining, own nodes, no STL containers.
// Templated on purpose: the index uses it as fingerprint -> OccurrenceList,
// Alyan reuses it as (docA,docB) -> int. If it only works for fingerprints it
// is not done.
//
// Design choices to defend in the viva:
//   * bucket index = (h ^ (h >> 32)) % capacity -- mixing the high bits saves
//     us when a finaliser leaves the low bits boring;
//   * grow when entries > capacity * 0.75 -- doubling keeps insertion amortised
//     O(1), so indexing the corpus stays linear;
//   * nodes held by the table, freed by the destructor; copy is deleted, not
//     merely expensive.
template <class K, class V>
class HashTable {
 public:
  HashTable() = default;

  ~HashTable() {
    // TODO(Anas W3 Step 5): walk every bucket and delete every node, then
    // delete[] buckets_. Today buckets_ is never allocated, so this is safe.
    delete[] buckets_;
    buckets_ = nullptr;
  }

  HashTable(const HashTable&) = delete;
  HashTable& operator=(const HashTable&) = delete;

  // Returns a reference to the stored value so callers can append to it.
  V& insert(const K& k, const V& v) {
    (void)k;
    (void)v;
    // TODO(Anas W3 Step 5): bucket index -> walk the chain -> append or push a
    // new node at the front -> ++size_ -> grow() if the load factor passed 0.75.
    static V stub_value{};
    return stub_value;
  }

  V* find(const K& k) {
    (void)k;
    // TODO(Anas W3 Step 5): walk the chain, compare keys.
    return nullptr;
  }

  const V* find(const K& k) const {
    (void)k;
    return nullptr;
  }

  bool erase(const K& k) {
    (void)k;
    return false;  // TODO(Anas W3 Step 5)
  }

  std::size_t bucketCount() const { return cap_; }
  std::size_t size() const { return size_; }

  double meanChainLength() const {
    return cap_ == 0 ? 0.0 : static_cast<double>(size_) / static_cast<double>(cap_);
  }

  std::size_t longestChain() const {
    // TODO(Anas W3 Step 5): max chain length over all buckets.
    return 0;
  }

  template <class F>
  void for_each_bucket(F&& fn) const {
    (void)fn;  // TODO(Anas W3 Step 5): visit (key, value) for every entry.
  }

 private:
  struct Node {
    K key;
    V val;
    Node* next;
  };

  // Mixing the high bits into the bucket index.
  std::size_t index_of(std::uint64_t h) const {
    return static_cast<std::size_t>((h ^ (h >> 32)) % cap_);
  }

  void grow() {
    // TODO(Anas W3 Step 5): cap_ *= 2, re-insert every node. Keys are
    // unchanged; only their bucket index moves.
  }

  Node** buckets_ = nullptr;
  std::size_t cap_ = 16;
  std::size_t size_ = 0;
};

}  // namespace plag

#endif  // PLAG_SRC_INDEX_HASH_TABLE_HPP_
