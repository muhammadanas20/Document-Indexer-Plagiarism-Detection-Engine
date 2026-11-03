// string_match.hpp  --  Pattern matching in text  (Week 12 topic: String search
// - brute force, Rabin Karp, Knuth Morris Pratt)
// Owner: Alyan
//
// Where it is used in the project:
//   * "Search a phrase" in the menu: we search the phrase in every document
//     with the algorithm the user picks, so all three can be compared.
//   * "Detect plagiarism": after the k-gram filter says that two documents are
//     similar, we split one document's text into chunks of words and use KMP to
//     find exactly where each chunk appears in the other document. Those hits
//     give us the reported line numbers.
//
// All three functions return the starting positions of every match.

#ifndef DIP_STRING_MATCH_HPP
#define DIP_STRING_MATCH_HPP

#include <cstddef>
#include <string>
#include <vector>

namespace dip {

// Naive/brute force: try the pattern at every position. O(n*m).
std::vector<std::size_t> bruteForceSearch(const std::string& text,
                                          const std::string& pattern);

// KMP: build the prefix (LPS) table once, then never move backwards in the
// text. O(n + m).
std::vector<std::size_t> kmpSearch(const std::string& text,
                                   const std::string& pattern);

// Rabin-Karp: compare the hash of the window with the hash of the pattern and
// slide the window in O(1). Average O(n + m), worst case O(n*m) on collisions.
std::vector<std::size_t> rabinKarpSearch(const std::string& text,
                                         const std::string& pattern);

// The prefix table used by KMP, exposed so the report and the tests can show it.
std::vector<std::size_t> buildPrefixTable(const std::string& pattern);

}  // namespace dip

#endif  // DIP_STRING_MATCH_HPP
