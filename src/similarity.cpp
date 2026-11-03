// similarity.cpp  --  see similarity.hpp
// Owner: Alyan

#include "similarity.hpp"

#include <cstddef>

#include "string_match.hpp"

namespace dip {
namespace {

const int kChunkWords = 6;      // how many words we search in one KMP call
const double kMinScore = 0.08;  // pairs below this are not reported

// Turn word positions into line ranges and pack them into one MatchedSpan.
MatchedSpan makeSpan(const Document& first, const Document& second,
                     std::size_t firstStart, std::size_t firstEnd,
                     std::size_t secondStart, std::size_t secondEnd) {
  MatchedSpan span;
  span.firstStartLine = first.wordLines[firstStart];
  span.firstEndLine = first.wordLines[firstEnd];
  span.secondStartLine = second.wordLines[secondStart];
  span.secondEndLine = second.wordLines[secondEnd];
  span.words = static_cast<int>(firstEnd - firstStart + 1);
  return span;
}

}  // namespace

int countSharedFingerprints(const std::vector<unsigned long long>& first,
                            const std::vector<unsigned long long>& second) {
  // Both lists are sorted, so one pass is enough (the merge step of merge sort).
  std::size_t i = 0;
  std::size_t j = 0;
  int shared = 0;
  while (i < first.size() && j < second.size()) {
    if (first[i] == second[j]) {
      ++shared;
      ++i;
      ++j;
    } else if (first[i] < second[j]) {
      ++i;
    } else {
      ++j;
    }
  }
  return shared;
}

double jaccardSimilarity(const std::vector<unsigned long long>& first,
                         const std::vector<unsigned long long>& second) {
  int shared = countSharedFingerprints(first, second);
  std::size_t total = first.size() + second.size() - static_cast<std::size_t>(shared);
  if (total == 0) return 0.0;
  return static_cast<double>(shared) / static_cast<double>(total);
}

std::vector<MatchedSpan> findCopiedSpans(const Document& first,
                                         const Document& second,
                                         int chunkWords) {
  std::vector<MatchedSpan> spans;
  if (chunkWords <= 0) return spans;
  const std::size_t chunk = static_cast<std::size_t>(chunkWords);
  if (first.words.size() < chunk || second.words.size() < chunk) return spans;

  // The open span: it grows while the two documents keep agreeing.
  bool open = false;
  std::size_t openFirstStart = 0;
  std::size_t openFirstEnd = 0;
  std::size_t openSecondStart = 0;
  std::size_t openSecondEnd = 0;

  for (std::size_t start = 0; start + chunk <= first.words.size(); ++start) {
    const std::size_t lastWord = start + chunk - 1;
    const std::size_t beginChar = first.wordStart[start];
    const std::size_t endChar =
        first.wordStart[lastWord] + first.words[lastWord].size();
    const std::string pattern =
        first.normalized.substr(beginChar, endChar - beginChar);

    // STEP 2 of the pipeline: KMP finds this chunk inside the second document.
    const std::vector<std::size_t> hits = kmpSearch(second.normalized, pattern);

    if (hits.empty()) {
      // The copy stopped here: close the span we had open.
      if (open) {
        spans.push_back(makeSpan(first, second, openFirstStart, openFirstEnd,
                                 openSecondStart, openSecondEnd));
        open = false;
      }
      continue;
    }

    // Is this chunk the continuation of the copy we are following?  The chunks
    // overlap by (chunk - 1) words, so the hit has to move by the same amount.
    bool continued = false;
    std::size_t hitWord = 0;
    for (std::size_t h = 0; h < hits.size(); ++h) {
      const std::size_t candidate = second.wordIndexOfPosition(hits[h]);
      if (open && candidate == openSecondStart + (start - openFirstStart)) {
        hitWord = candidate;
        continued = true;
        break;
      }
    }

    if (!continued) {
      // A new copied block starts here: close the previous one first.
      if (open) {
        spans.push_back(makeSpan(first, second, openFirstStart, openFirstEnd,
                                 openSecondStart, openSecondEnd));
      }
      hitWord = second.wordIndexOfPosition(hits[0]);
      open = true;
      openFirstStart = start;
      openSecondStart = hitWord;
    }

    openFirstEnd = lastWord;
    openSecondEnd = hitWord + chunk - 1;
  }

  if (open) {
    spans.push_back(makeSpan(first, second, openFirstStart, openFirstEnd,
                             openSecondStart, openSecondEnd));
  }
  return spans;
}

std::vector<PairResult> compareAll(const Index& index, int minSharedFingerprints) {
  std::vector<PairResult> results;
  const std::vector<std::vector<unsigned long long> >& fingerprints = index.fingerprints();
  const std::vector<Document>& documents = index.documents();

  for (std::size_t a = 0; a < fingerprints.size(); ++a) {
    for (std::size_t b = a + 1; b < fingerprints.size(); ++b) {
      int shared = countSharedFingerprints(fingerprints[a], fingerprints[b]);
      if (shared < minSharedFingerprints) continue;

      PairResult result;
      result.firstDoc = static_cast<int>(a);
      result.secondDoc = static_cast<int>(b);
      result.sharedFingerprints = shared;
      result.jaccard = jaccardSimilarity(fingerprints[a], fingerprints[b]);
      if (result.jaccard < kMinScore) continue;

      std::size_t smaller = fingerprints[a].size();
      if (fingerprints[b].size() < smaller) smaller = fingerprints[b].size();
      result.containment = (smaller == 0)
                               ? 0.0
                               : static_cast<double>(shared) / static_cast<double>(smaller);

      // Step 2: prove it with KMP and collect the line numbers.
      result.spans = findCopiedSpans(documents[a], documents[b], kChunkWords);
      result.matchedWords = 0;
      for (std::size_t s = 0; s < result.spans.size(); ++s) {
        result.matchedWords += result.spans[s].words;
      }
      results.push_back(result);
    }
  }
  return results;
}

const char* suspicionLabel(double score) {
  if (score >= 0.75) return "CRITICAL";
  if (score >= 0.40) return "HIGH";
  if (score >= 0.20) return "MEDIUM";
  if (score >= 0.08) return "LOW";
  return "NONE";
}

}  // namespace dip
