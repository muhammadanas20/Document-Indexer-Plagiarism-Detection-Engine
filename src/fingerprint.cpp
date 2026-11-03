// fingerprint.cpp  --  see fingerprint.hpp
// Owner: Anas

#include "fingerprint.hpp"

#include <cstddef>

#include "hash_table.hpp"
#include "sort.hpp"

namespace dip {
namespace {

const unsigned long long kBase = 131ULL;
const unsigned long long kMod = 1000000007ULL;  // prime, keeps the hash small

}  // namespace

std::vector<unsigned long long> computeFingerprints(
    const std::vector<std::string>& words, int k) {
  std::vector<unsigned long long> fingerprints;
  if (k <= 0) return fingerprints;
  const std::size_t window = static_cast<std::size_t>(k);
  if (words.size() < window) return fingerprints;

  // 1. Hash every word once (a word is a string, we need a number to roll on).
  std::vector<unsigned long long> wordHashes(words.size());
  for (std::size_t i = 0; i < words.size(); ++i) {
    wordHashes[i] = hashKey(words[i]) % kMod;
  }

  // 2. base^(k-1) mod p, computed once => every slide is O(1).
  unsigned long long highPower = 1ULL;
  for (std::size_t i = 0; i + 1 < window; ++i) {
    highPower = (highPower * kBase) % kMod;
  }

  // 3. First window.
  unsigned long long hash = 0ULL;
  for (std::size_t i = 0; i < window; ++i) {
    hash = (hash * kBase + wordHashes[i]) % kMod;
  }

  // 4. A hash table removes duplicates: the same sentence twice in one document
  //    should count once, otherwise a repeated paragraph would inflate the score.
  HashTable<unsigned long long, int> seen(64);
  seen.put(hash, 1);

  // 5. Slide the window over the rest of the tokens.
  for (std::size_t start = 1; start + window <= words.size(); ++start) {
    unsigned long long leaving = (wordHashes[start - 1] * highPower) % kMod;
    hash = (hash + kMod - leaving) % kMod;
    hash = (hash * kBase + wordHashes[start + window - 1]) % kMod;
    seen.put(hash, 1);
  }

  // 6. Copy the distinct fingerprints out of the hash table.
  fingerprints.reserve(seen.size());
  for (std::size_t b = 0; b < seen.bucketCount(); ++b) {
    for (typename LinkedList<std::pair<unsigned long long, int> >::Node* n =
             seen.bucket(b)->head();
         n != nullptr; n = n->next) {
      fingerprints.push_back(n->value.first);
    }
  }

  // 7. Sorted output => the comparison of two documents is a merge-style walk.
  mergeSortFingerprints(fingerprints);
  return fingerprints;
}

}  // namespace dip
