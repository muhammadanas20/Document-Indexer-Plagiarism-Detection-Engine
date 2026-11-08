#ifndef PROJECT_DOCUMENTS_HPP
#define PROJECT_DOCUMENTS_HPP

#include <cstddef>
#include <string>
#include <vector>

// document
struct Document {
  std::string path;
  std::vector<std::string> words;
  std::vector<int> wordLines;
  std::string normalized;
  std::vector<std::size_t> wordStart;

  std::size_t wordCount() const { return words.size(); }

  void buildNormalized() {
    normalized.clear();
    wordStart.clear();
    for (std::size_t i = 0; i < words.size(); ++i) {
      if (i > 0) normalized.push_back(' ');
      wordStart.push_back(normalized.size());
      normalized += words[i];
    }
  }

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

  int lineOfPosition(std::size_t position) const {
    if (words.empty()) return 0;
    return wordLines[wordIndexOfPosition(position)];
  }
};

// tokenizer
struct TokenizedText {
  std::vector<std::string> words;
  std::vector<int> lines;
};

TokenizedText tokenize(const std::string& text, bool stripComments);

// loader
bool loadCorpus(const std::string& folder, std::vector<Document>& documents,
                std::string& error);

bool isSourceCodeFile(const std::string& path);

bool isSupportedFile(const std::string& path);

#endif
