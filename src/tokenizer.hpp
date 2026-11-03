// tokenizer.hpp  --  turns the raw text of a file into words
// (Week 1-2 topics: ADT, pointers, string handling)
// Owner: Anas
//
// "Tokenizing" = cutting the file into tokens (words) and remembering the line
// number of every token, so the report can say "line 12 of bilal.cpp is copied".
//
// Normalisation we do (so that copying with small changes is still caught):
//   * letters are converted to lower case          -> Sum != sum
//   * punctuation, spaces and tabs are thrown away -> "sum(a, b)" == "sum a b"
//   * comments (// and /* ... */) are skipped      -> commenting the code out
//                                                     does not hide the copy

#ifndef DIP_TOKENIZER_HPP
#define DIP_TOKENIZER_HPP

#include <string>
#include <vector>

namespace dip {

struct TokenizedText {
  std::vector<std::string> words;  // the tokens, in the order they appear
  std::vector<int> lines;          // lines[i] = 1-based line of words[i]
};

// stripComments = true for source code files (.cpp, .h, ...), false for .txt
// where "//" could be normal text.
TokenizedText tokenize(const std::string& text, bool stripComments);

}  // namespace dip

#endif  // DIP_TOKENIZER_HPP
