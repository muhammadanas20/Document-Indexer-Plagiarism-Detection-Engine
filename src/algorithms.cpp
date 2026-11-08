#include "algorithms.hpp"
#include <cstddef>

// sort
void mergeRange(std::vector<unsigned long long>& keys, std::size_t left,
                std::size_t middle, std::size_t right,
                std::vector<unsigned long long>& buffer) {
  std::size_t i = left;
  std::size_t j = middle + 1;
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
  if (left >= right) return;
  std::size_t middle = left + (right - left) / 2;
  mergeSortRange(keys, left, middle, buffer);
  mergeSortRange(keys, middle + 1, right, buffer);
  mergeRange(keys, left, middle, right, buffer);
}

std::size_t partition(std::vector<std::string>& items, std::size_t low,
                      std::size_t high) {
  std::size_t middle = low + (high - low) / 2;
  std::string pivot = items[middle];

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

void mergePairs(std::vector<PairResult>& pairs, std::size_t left,
                std::size_t middle, std::size_t right,
                std::vector<PairResult>& buffer) {
  std::size_t i = left;
  std::size_t j = middle + 1;
  std::size_t out = left;

  while (i <= middle && j <= right) {

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
  return -1;
}

// string match
const unsigned long long kRabinBase = 257ULL;
const unsigned long long kRabinMod = 1000000007ULL;

unsigned long long charHash(char c) {
  return static_cast<unsigned char>(c);
}

std::vector<std::size_t> bruteForceSearch(const std::string& text,
                                          const std::string& pattern) {
  std::vector<std::size_t> positions;
  if (pattern.empty() || pattern.size() > text.size()) return positions;

  for (std::size_t start = 0; start + pattern.size() <= text.size(); ++start) {
    std::size_t matched = 0;
    while (matched < pattern.size() &&
           text[start + matched] == pattern[matched]) {
      ++matched;
    }
    if (matched == pattern.size()) positions.push_back(start);
  }
  return positions;
}

std::vector<std::size_t> buildPrefixTable(const std::string& pattern) {

  // save how much of the pattern we can keep after a mismatch
  std::vector<std::size_t> prefix(pattern.size(), 0);
  std::size_t length = 0;
  for (std::size_t i = 1; i < pattern.size(); ++i) {
    while (length > 0 && pattern[i] != pattern[length]) {
      length = prefix[length - 1];
    }
    if (pattern[i] == pattern[length]) ++length;
    prefix[i] = length;
  }
  return prefix;
}

std::vector<std::size_t> kmpSearch(const std::string& text,
                                   const std::string& pattern) {
  std::vector<std::size_t> positions;
  if (pattern.empty() || pattern.size() > text.size()) return positions;

  std::vector<std::size_t> prefix = buildPrefixTable(pattern);
  std::size_t matched = 0;

  for (std::size_t i = 0; i < text.size(); ++i) {
    while (matched > 0 && text[i] != pattern[matched]) {
      matched = prefix[matched - 1];
    }
    if (text[i] == pattern[matched]) ++matched;
    if (matched == pattern.size()) {
      positions.push_back(i + 1 - pattern.size());
      matched = prefix[matched - 1];
    }
  }
  return positions;
}

std::vector<std::size_t> rabinKarpSearch(const std::string& text,
                                         const std::string& pattern) {
  std::vector<std::size_t> positions;
  if (pattern.empty() || pattern.size() > text.size()) return positions;

  const std::size_t m = pattern.size();

  unsigned long long highPower = 1ULL;
  for (std::size_t i = 0; i + 1 < m; ++i) {
    highPower = (highPower * kRabinBase) % kRabinMod;
  }

  unsigned long long patternHash = 0ULL;
  unsigned long long windowHash = 0ULL;
  for (std::size_t i = 0; i < m; ++i) {
    patternHash = (patternHash * kRabinBase + charHash(pattern[i])) % kRabinMod;
    windowHash = (windowHash * kRabinBase + charHash(text[i])) % kRabinMod;
  }
  if (windowHash == patternHash && text.compare(0, m, pattern) == 0) positions.push_back(0);

  for (std::size_t start = 1; start + m <= text.size(); ++start) {

    unsigned long long leaving = (charHash(text[start - 1]) * highPower) % kRabinMod;
    windowHash = (windowHash + kRabinMod - leaving) % kRabinMod;
    windowHash = (windowHash * kRabinBase + charHash(text[start + m - 1])) % kRabinMod;

    if (windowHash == patternHash) {

      bool equal = true;
      for (std::size_t k = 0; k < m; ++k) {
        if (text[start + k] != pattern[k]) {
          equal = false;
          break;
        }
      }
      if (equal) positions.push_back(start);
    }
  }
  return positions;
}
