// sort.hpp  --  Sorting algorithms  (Weeks 5-7 topics: elementary sorting,
// merge sort, quick sort)  +  Binary search (Week 5)
// Owner: Anas
//
// We are not allowed to use std::sort, so we wrote the three sorts the course
// asks for. Each one is used where it fits best:
//
//   mergeSortFingerprints  - sorts the k-gram fingerprints of a document.
//                            merge sort is O(n log n) in every case and stable,
//                            and we then walk two sorted lists in O(n) to count
//                            common fingerprints.
//   quickSortStrings       - sorts the list of file paths before reading them,
//                            so the document numbers are the same on every run.
//   insertionSortScores    - sorts the small top-K list (K is ~10), where the
//                            simple O(n^2) sort is the fastest in practice.
//   binarySearchWord       - searches a word in the sorted vocabulary.

#ifndef DIP_SORT_HPP
#define DIP_SORT_HPP

#include <string>
#include <vector>

#include "heap.hpp"
#include "similarity.hpp"

namespace dip {

// Merge sort on numbers. O(n log n) always, O(n) extra memory.
void mergeSortFingerprints(std::vector<unsigned long long>& keys);

// Quick sort on text. Average O(n log n), worst case O(n^2) if the pivot is
// always the smallest/largest element. We take the middle element as pivot.
void quickSortStrings(std::vector<std::string>& items);

// Merge sort on the detected pairs: biggest score first. O(n log n).
// We use it for the report, because a whole corpus can produce many pairs.
void mergeSortPairs(std::vector<PairResult>& pairs);

// Insertion sort on the best-K list, biggest score first. O(k^2).
void insertionSortScores(std::vector<ScoreEntry>& entries);

// Binary search in an alphabetically sorted word list. O(log n).
// Returns the index of the word, or -1 if it is not there.
int binarySearchWord(const std::vector<std::string>& sortedWords,
                     const std::string& word);

}  // namespace dip

#endif  // DIP_SORT_HPP
