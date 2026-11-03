// document.hpp  --  one loaded file of the corpus
// Owner: Anas
//
// A Document keeps everything the rest of the program needs about one file:
//   words      : the normalised tokens, in order
//   wordLines  : the line number of every token (for the report)
//   normalized : the same tokens joined by single spaces, as one big string.
//                The string-matching algorithms work on characters, so they
//                need this "one string per document" version.
//   wordStart  : character position of every word inside `normalized`.
//                With it we can turn a character position found by KMP back
//                into a word number (binary search) and then into a line number.

#ifndef DIP_DOCUMENT_HPP
#define DIP_DOCUMENT_HPP

#include <cstddef>
#include <string>
#include <vector>

namespace dip {

struct Document {
  std::string path;                 // e.g. samples/students/ali.cpp
  std::vector<std::string> words;   // normalised tokens
  std::vector<int> wordLines;       // 1-based line of each token
  std::string normalized;           // "int sum to int n int total 0 for ..."
  std::vector<std::size_t> wordStart;  // char offset of each token in `normalized`

  std::size_t wordCount() const { return words.size(); }

  // Build `normalized` and `wordStart` from `words`. Called by the loader.
  void buildNormalized() {
    normalized.clear();
    wordStart.clear();
    for (std::size_t i = 0; i < words.size(); ++i) {
      wordStart.push_back(normalized.size());
      if (i > 0) normalized.push_back(' ');
      normalized += words[i];
    }
  }

  // Index of the word that starts at character position `position`.
  // Binary search over wordStart: O(log n).
  std::size_t wordIndexOfPosition(std::size_t position) const {
    if (words.empty()) return 0;
    std::size_t low = 0;
    std::size_t high = wordStart.size() - 1;
    while (low < high) {
      std::size_t middle = low + (high - low + 1) / 2;
      if (wordStart[middle] <= position) {
        low = middle;
      } else {
        high = middle - 1;
      }
    }
    return low;
  }

  // 1-based line of the word that starts at character position `position`.
  int lineOfPosition(std::size_t position) const {
    if (words.empty()) return 0;
    return wordLines[wordIndexOfPosition(position)];
  }
};

}  // namespace dip

#endif  // DIP_DOCUMENT_HPP
