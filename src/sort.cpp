// sort.cpp  --  see sort.hpp
// Owner: Anas

#include "sort.hpp"

namespace dip {
namespace {

// ---- merge sort: split, sort both halves, merge them back -------------------
void mergeRange(std::vector<unsigned long long>& keys, std::size_t left,
                std::size_t middle, std::size_t right,
                std::vector<unsigned long long>& buffer) {
  std::size_t i = left;        // walks the left half
  std::size_t j = middle + 1;  // walks the right half
  std::size_t out = left;

  while (i <= middle && j <= right) {
    if (keys[i] <= keys[j]) {
      buffer[out++] = keys[i++];
    } else {
      buffer[out++] = keys[j++];
    }
  }
  while (i <= middle) buffer[out++] = keys[i++];
  while (j <= right) buffer[out++] = keys[j++];

  for (std::size_t k = left; k <= right; ++k) keys[k] = buffer[k];
}

void mergeSortRange(std::vector<unsigned long long>& keys, std::size_t left,
                    std::size_t right, std::vector<unsigned long long>& buffer) {
  if (left >= right) return;  // 0 or 1 element is already sorted
  std::size_t middle = left + (right - left) / 2;
  mergeSortRange(keys, left, middle, buffer);        // sort the left half
  mergeSortRange(keys, middle + 1, right, buffer);   // sort the right half
  mergeRange(keys, left, middle, right, buffer);     // then merge
}

// ---- quick sort ------------------------------------------------------------
// Lomuto partition: everything smaller than the pivot goes to the left.
std::size_t partition(std::vector<std::string>& items, std::size_t low,
                      std::size_t high) {
  std::size_t middle = low + (high - low) / 2;
  std::string pivot = items[middle];
  // Move the pivot to the end so the loop below is simple.
  std::string temporary = items[middle];
  items[middle] = items[high];
  items[high] = temporary;

  std::size_t smaller = low;
  for (std::size_t i = low; i < high; ++i) {
    if (items[i] < pivot) {
      temporary = items[i];
      items[i] = items[smaller];
      items[smaller] = temporary;
      ++smaller;
    }
  }
  temporary = items[smaller];
  items[smaller] = items[high];
  items[high] = temporary;
  return smaller;
}

void quickSortRange(std::vector<std::string>& items, std::size_t low,
                    std::size_t high) {
  if (low >= high) return;
  std::size_t pivotIndex = partition(items, low, high);
  if (pivotIndex > low) quickSortRange(items, low, pivotIndex - 1);
  if (pivotIndex < high) quickSortRange(items, pivotIndex + 1, high);
}

// ---- merge sort on the pair list (same algorithm, other element type) ------
void mergePairs(std::vector<PairResult>& pairs, std::size_t left,
                std::size_t middle, std::size_t right,
                std::vector<PairResult>& buffer) {
  std::size_t i = left;
  std::size_t j = middle + 1;
  std::size_t out = left;

  while (i <= middle && j <= right) {
    // '>=' keeps the order of equal scores stable.
    if (pairs[i].jaccard >= pairs[j].jaccard) {
      buffer[out++] = pairs[i++];
    } else {
      buffer[out++] = pairs[j++];
    }
  }
  while (i <= middle) buffer[out++] = pairs[i++];
  while (j <= right) buffer[out++] = pairs[j++];
  for (std::size_t k = left; k <= right; ++k) pairs[k] = buffer[k];
}

void mergeSortPairRange(std::vector<PairResult>& pairs, std::size_t left,
                        std::size_t right, std::vector<PairResult>& buffer) {
  if (left >= right) return;
  std::size_t middle = left + (right - left) / 2;
  mergeSortPairRange(pairs, left, middle, buffer);
  mergeSortPairRange(pairs, middle + 1, right, buffer);
  mergePairs(pairs, left, middle, right, buffer);
}

}  // namespace

void mergeSortPairs(std::vector<PairResult>& pairs) {
  if (pairs.size() < 2) return;
  std::vector<PairResult> buffer(pairs.size());
  mergeSortPairRange(pairs, 0, pairs.size() - 1, buffer);
}

void mergeSortFingerprints(std::vector<unsigned long long>& keys) {
  if (keys.size() < 2) return;
  std::vector<unsigned long long> buffer(keys.size(), 0ULL);
  mergeSortRange(keys, 0, keys.size() - 1, buffer);
}

void quickSortStrings(std::vector<std::string>& items) {
  if (items.size() < 2) return;
  quickSortRange(items, 0, items.size() - 1);
}

void insertionSortScores(std::vector<ScoreEntry>& entries) {
  for (std::size_t i = 1; i < entries.size(); ++i) {
    ScoreEntry current = entries[i];
    std::size_t j = i;
    // Walk backwards while the previous entry has a SMALLER score.
    while (j > 0 && entries[j - 1].score < current.score) {
      entries[j] = entries[j - 1];
      --j;
    }
    entries[j] = current;
  }
}

int binarySearchWord(const std::vector<std::string>& sortedWords,
                     const std::string& word) {
  int low = 0;
  int high = static_cast<int>(sortedWords.size()) - 1;
  while (low <= high) {
    int middle = low + (high - low) / 2;
    if (sortedWords[static_cast<std::size_t>(middle)] == word) return middle;
    if (sortedWords[static_cast<std::size_t>(middle)] < word) {
      low = middle + 1;
    } else {
      high = middle - 1;
    }
  }
  return -1;  // not found
}

}  // namespace dip
