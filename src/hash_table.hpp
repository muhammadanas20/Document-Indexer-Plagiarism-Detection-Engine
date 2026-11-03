// hash_table.hpp  --  Hash table with SEPARATE CHAINING
// (Week 13 topic: Hashing, hash functions, collision resolution, rehashing)
// Owner: Anas
//
// Where it is used in the project:
//   The inverted index: key = a word, value = the posting list of that word
//   (which documents contain it, on which line, how many times).
//   It is also used to store the distinct k-gram fingerprints of a document.
//
// Collision resolution = separate chaining:
//   every bucket is a LinkedList, so keys that land in the same bucket just sit
//   in the same list. That is why we could write LinkedList in list.hpp.

#ifndef DIP_HASH_TABLE_HPP
#define DIP_HASH_TABLE_HPP

#include <cstddef>
#include <string>

#include "list.hpp"

namespace dip {

// ---------------------------------------------------------------- hash functions
// A hash function turns a key into a number. The number is then reduced modulo
// the number of buckets. Two simple functions are enough for this project.

// For text: djb2 style  hash = hash * 31 + character.
inline unsigned long long hashKey(const std::string& key) {
  unsigned long long hash = 5381;
  for (std::size_t i = 0; i < key.size(); ++i) {
    hash = hash * 31ULL + static_cast<unsigned char>(key[i]);
  }
  return hash;
}

// For numbers: the value itself is already well distributed.
inline unsigned long long hashKey(unsigned long long key) { return key; }
inline unsigned long long hashKey(long long key) {
  return static_cast<unsigned long long>(key);
}
inline unsigned long long hashKey(int key) {
  return static_cast<unsigned long long>(key);
}

// ---------------------------------------------------------------- the table
// K = key type, V = value type. (A template only means "the type is a
// parameter", so the same class works for words and for fingerprint numbers.)
template <typename K, typename V>
class HashTable {
 public:
  typedef LinkedList<std::pair<K, V> > Bucket;  // one chain

  explicit HashTable(std::size_t bucketCount = 16)
      : buckets_(nullptr), bucketCount_(bucketCount), size_(0) {
    if (bucketCount_ < 4) bucketCount_ = 4;
    buckets_ = new Bucket*[bucketCount_];
    for (std::size_t i = 0; i < bucketCount_; ++i) buckets_[i] = new Bucket();
  }

  // Rule of Three: free every bucket and every node inside it.
  ~HashTable() {
    for (std::size_t i = 0; i < bucketCount_; ++i) delete buckets_[i];
    delete[] buckets_;
  }
  HashTable(const HashTable&) = delete;             // a hash table is never copied
  HashTable& operator=(const HashTable&) = delete;  // (the posting lists would be shared)

  // Insert the key, or update the value if the key is already there.
  // Returns a pointer to the stored value. Average O(1).
  V* put(const K& key, const V& value) {
    // Rehashing: keep the load factor (entries / buckets) below 0.75.
    if (static_cast<double>(size_ + 1) / static_cast<double>(bucketCount_) > 0.75) {
      rehash(bucketCount_ * 2);
    }
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

  // Find the value of a key, or nullptr if the key does not exist.
  // Average O(1), worst case O(n) when every key lands in one bucket.
  V* get(const K& key) {
    std::size_t index = bucketIndex(key);
    for (typename Bucket::Node* n = buckets_[index]->head(); n != nullptr; n = n->next) {
      if (n->value.first == key) return &n->value.second;
    }
    return nullptr;
  }

  bool contains(const K& key) { return get(key) != nullptr; }

  // Remove a key. Returns true if it was there. Average O(1).
  bool remove(const K& key) {
    std::size_t index = bucketIndex(key);
    for (typename Bucket::Node* n = buckets_[index]->head(); n != nullptr; n = n->next) {
      if (n->value.first == key) {
        // LinkedList::remove needs a whole pair, so we build a temporary one.
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

  // Longest chain in the table - this is how we check that the hash function
  // spreads the keys well (a good table has a long chain close to the average).
  std::size_t longestChain() const {
    std::size_t longest = 0;
    for (std::size_t i = 0; i < bucketCount_; ++i) {
      if (buckets_[i]->size() > longest) longest = buckets_[i]->size();
    }
    return longest;
  }

  // Read-only access so the report can walk over every bucket.
  const Bucket* bucket(std::size_t index) const { return buckets_[index]; }

 private:
  std::size_t bucketIndex(const K& key) const {
    return static_cast<std::size_t>(hashKey(key) % bucketCount_);
  }

  // Grow the table and put every existing key into its new bucket.
  // This is called "rehashing" (Week 13).
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

  Bucket** buckets_;       // array of chains
  std::size_t bucketCount_;
  std::size_t size_;       // number of stored (key, value) pairs
};

}  // namespace dip

#endif  // DIP_HASH_TABLE_HPP
