// tokenizer.cpp  --  see tokenizer.hpp
// Owner: Anas

#include "tokenizer.hpp"

#include <cctype>
#include <cstddef>

namespace dip {

TokenizedText tokenize(const std::string& text, bool stripComments) {
  TokenizedText result;
  std::string currentWord;
  int line = 1;
  bool inBlockComment = false;

  for (std::size_t i = 0; i < text.size(); ++i) {
    char c = text[i];

    if (c == '\n') {
      // A line comment ends with the line, so we only reset the block flag.
      ++line;
      if (!currentWord.empty()) {
        result.words.push_back(currentWord);
        result.lines.push_back(line - 1);  // the word belongs to the line before
        currentWord.clear();
      }
      continue;
    }

    if (stripComments) {
      // Start or end of a  /* ... */  block comment.
      if (inBlockComment) {
        if (c == '*' && i + 1 < text.size() && text[i + 1] == '/') {
          inBlockComment = false;
          ++i;
        }
        continue;
      }
      if (c == '/' && i + 1 < text.size() && text[i + 1] == '*') {
        inBlockComment = true;
        ++i;
        continue;
      }
      // Everything after // on this line is a comment.
      if (c == '/' && i + 1 < text.size() && text[i + 1] == '/') {
        while (i < text.size() && text[i] != '\n') ++i;
        if (i < text.size()) {  // we stopped on the newline: handle it next loop
          --i;
        }
        continue;
      }
    }

    // A word is a run of letters, digits or underscore.
    bool isWordChar = std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
    if (isWordChar) {
      currentWord.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    } else if (!currentWord.empty()) {
      result.words.push_back(currentWord);
      result.lines.push_back(line);
      currentWord.clear();
    }
  }

  // The file may not end with a newline / separator.
  if (!currentWord.empty()) {
    result.words.push_back(currentWord);
    result.lines.push_back(line);
  }
  return result;
}

}  // namespace dip
