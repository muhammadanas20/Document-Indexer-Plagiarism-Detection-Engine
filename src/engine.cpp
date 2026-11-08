#include "engine.hpp"
#include "algorithms.hpp"
#include <cstddef>

// index
Index::~Index() { freePostings(); }

void Index::freePostings() {
  for (std::size_t b = 0; b < wordTable_.bucketCount(); ++b) {
    typename LinkedList<std::pair<std::string, LinkedList<Occurrence>*> >::Node* node =
        wordTable_.bucket(b)->head();
    for (; node != nullptr; node = node->next) {
      delete node->value.second;
    }
  }
}

void Index::addToPostings(const std::string& word, int documentId, int line) {
  // each word points to a list of files that contain it
  LinkedList<Occurrence>** slot = wordTable_.get(word);
  if (slot == nullptr) {

    LinkedList<Occurrence>* list = new LinkedList<Occurrence>();
    list->pushBack(Occurrence{documentId, line, 1});
    wordTable_.put(word, list);
    vocabulary_.insert(word);
    return;
  }

  LinkedList<Occurrence>* list = *slot;
  for (LinkedList<Occurrence>::Node* node = list->head(); node != nullptr;
       node = node->next) {
    if (node->value.documentId == documentId) {
      ++node->value.count;
      return;
    }
  }
  list->pushBack(Occurrence{documentId, line, 1});
  vocabulary_.insert(word);
}

void Index::build(const std::vector<Document>& documents, int k) {
  // clear the old index before loading another set of files
  freePostings();
  wordTable_.clear();
  vocabulary_.clear();
  documents_ = &documents;
  kgramSize_ = k;
  totalWords_ = 0;
  fingerprints_.clear();
  fingerprints_.resize(documents.size());

  double probeSum = 0.0;
  std::size_t documentCounter = 0;

  for (std::size_t d = 0; d < documents.size(); ++d) {
    const Document& document = documents[d];
    const int documentId = static_cast<int>(d);
    totalWords_ += document.words.size();

    LinearProbeTable counters(32);
    for (std::size_t i = 0; i < document.words.size(); ++i) {
      counters.add(document.words[i]);
      addToPostings(document.words[i], documentId, document.wordLines[i]);
    }
    if (counters.size() > 0) {
      probeSum += counters.averageProbes();
      ++documentCounter;
    }

    fingerprints_[d] = computeFingerprints(document.words, kgramSize_);
  }

  averageProbes_ = (documentCounter == 0) ? 0.0 : probeSum / static_cast<double>(documentCounter);
}

std::vector<SearchHit> Index::searchWord(const std::string& word) const {
  std::vector<SearchHit> hits;

  LinkedList<Occurrence>* const* slot = wordTable_.get(word);
  if (slot == nullptr) return hits;

  for (LinkedList<Occurrence>::Node* node = (*slot)->head(); node != nullptr;
       node = node->next) {
    SearchHit hit;
    hit.documentId = node->value.documentId;
    hit.firstLine = node->value.firstLine;
    hit.count = node->value.count;
    hits.push_back(hit);
  }
  return hits;
}

int Index::documentFrequency(const std::string& word) const {
  return vocabulary_.documentCount(word);
}

std::vector<std::string> Index::sortedVocabulary() const {
  std::vector<std::pair<std::string, int> > entries;
  vocabulary_.inOrder(entries);
  std::vector<std::string> words;
  words.reserve(entries.size());
  for (std::size_t i = 0; i < entries.size(); ++i) words.push_back(entries[i].first);
  return words;
}

std::size_t Index::fingerprintTotal() const {
  std::size_t total = 0;
  for (std::size_t d = 0; d < fingerprints_.size(); ++d) {
    total += fingerprints_[d].size();
  }
  return total;
}

// fingerprint
const unsigned long long kBase = 131ULL;
const unsigned long long kMod = 1000000007ULL;

std::vector<unsigned long long> computeFingerprints(
    const std::vector<std::string>& words, int k) {
  std::vector<unsigned long long> fingerprints;
  if (k <= 0) return fingerprints;
  const std::size_t window = static_cast<std::size_t>(k);
  if (words.size() < window) return fingerprints;

  HashTable<unsigned long long, int> seen(64);

  // make a hash for each group of k words
  for (std::size_t start = 0; start + window <= words.size(); ++start) {
    unsigned long long hash = 0;
    for (std::size_t i = start; i < start + window; ++i) {
      hash = (hash * kBase + hashKey(words[i]) % kMod) % kMod;
    }
    seen.put(hash, 1);
  }

  // keep each hash once and sort the list for comparison
  fingerprints.reserve(seen.size());
  for (std::size_t b = 0; b < seen.bucketCount(); ++b) {
    for (typename LinkedList<std::pair<unsigned long long, int> >::Node* n =
             seen.bucket(b)->head();
         n != nullptr; n = n->next) {
      fingerprints.push_back(n->value.first);
    }
  }

  mergeSortFingerprints(fingerprints);
  return fingerprints;
}

// similarity
const int kChunkWords = 6;
const double kMinScore = 0.08;

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

int countSharedFingerprints(const std::vector<unsigned long long>& first,
                            const std::vector<unsigned long long>& second) {

  // both lists are sorted, so move through them together
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

    // only keep matches that start and end at a word boundary
    std::vector<std::size_t> hits;
    std::vector<std::size_t> found = kmpSearch(second.normalized, pattern);
    for (std::size_t h = 0; h < found.size(); ++h) {
      std::size_t pos = found[h];
      std::size_t end = pos + pattern.size();
      if ((pos == 0 || second.normalized[pos - 1] == ' ') &&
          (end == second.normalized.size() || second.normalized[end] == ' ')) {
        hits.push_back(pos);
      }
    }

    if (hits.empty()) {

      if (open) {
        spans.push_back(makeSpan(first, second, openFirstStart, openFirstEnd,
                                 openSecondStart, openSecondEnd));
        open = false;
      }
      continue;
    }

    // join chunks when they follow the same words in both files
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
