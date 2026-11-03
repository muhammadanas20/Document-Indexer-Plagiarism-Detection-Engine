// similarity.hpp  --  how similar are two documents?
// (Week 12: KMP  +  Week 6: merge sort / two-pointer merge  +  Week 7: quick)
// Owner: Alyan
//
// The pipeline has two steps, and this is the part that makes the program fast:
//
//   STEP 1  cheap filter (fingerprints)
//           Every document has a sorted list of k-word fingerprints. Two
//           documents are compared by walking both sorted lists once
//           (the same merge idea as merge sort), counting the common numbers.
//                    Jaccard = |A and B| / |A or B|
//           This costs O(n + m) and needs no strings at all.
//
//   STEP 2  expensive proof (KMP)
//           Only for the pairs that survived step 1: we use KMP to find the
//           exact places where the words of A appear inside B, and we report
//           them as line ranges (that is what a teacher wants to see).
//
// Without step 1 we would run KMP on every pair: with 100 documents that is
// 4950 comparisons instead of a few.

#ifndef DIP_SIMILARITY_HPP
#define DIP_SIMILARITY_HPP

#include <string>
#include <vector>

#include "document.hpp"
#include "index.hpp"

namespace dip {

// A piece of text that appears in both documents.
struct MatchedSpan {
  int firstStartLine;   // lines in the first document
  int firstEndLine;
  int secondStartLine;  // the same text in the second document
  int secondEndLine;
  int words;            // how many words were copied
};

// The result for one pair of documents.
struct PairResult {
  int firstDoc;
  int secondDoc;
  double jaccard;             // |A and B| / |A or B|   (the score we rank by)
  double containment;         // |A and B| / min(|A|, |B|)
  int sharedFingerprints;
  int matchedWords;           // words confirmed by KMP
  std::vector<MatchedSpan> spans;
};

// Compare every pair (O(n^2) pairs, each one cheap) and keep the interesting
// ones. minSharedFingerprints filters out pairs that share almost nothing.
std::vector<PairResult> compareAll(const Index& index, int minSharedFingerprints);

// Jaccard similarity of two SORTED fingerprint lists: one walk, O(n + m).
double jaccardSimilarity(const std::vector<unsigned long long>& first,
                         const std::vector<unsigned long long>& second);

// How many fingerprints the two sorted lists have in common.
int countSharedFingerprints(const std::vector<unsigned long long>& first,
                           const std::vector<unsigned long long>& second);

// STEP 2: where exactly does document `first` appear inside `second`?
// We take every chunk of `chunkWords` words of `first` and search it in
// `second` with KMP, then merge the chunks that follow each other.
std::vector<MatchedSpan> findCopiedSpans(const Document& first,
                                         const Document& second,
                                         int chunkWords);

// "none" / "low" / "medium" / "high" / "critical"
const char* suspicionLabel(double score);

}  // namespace dip

#endif  // DIP_SIMILARITY_HPP
