#ifndef PROJECT_ENGINE_HPP
#define PROJECT_ENGINE_HPP

#include <cstddef>
#include <string>
#include <vector>
#include "documents.hpp"
#include "tables.hpp"
#include "trees.hpp"

// index
struct Occurrence {
  int documentId;
  int firstLine;
  int count;
};

struct SearchHit {
  int documentId;
  int firstLine;
  int count;
};

class Index {
 public:
  Index() : documents_(nullptr), kgramSize_(5), totalWords_(0), averageProbes_(0.0) {}
  ~Index();

  Index(const Index&) = delete;
  Index& operator=(const Index&) = delete;

  void build(const std::vector<Document>& documents, int k);

  std::vector<SearchHit> searchWord(const std::string& word) const;

  int documentFrequency(const std::string& word) const;

  std::vector<std::string> sortedVocabulary() const;

  const std::vector<Document>& documents() const { return *documents_; }
  const std::vector<std::vector<unsigned long long> >& fingerprints() const {
    return fingerprints_;
  }
  int kgramSize() const { return kgramSize_; }

  std::size_t documentCount() const { return documents_ == nullptr ? 0 : documents_->size(); }
  std::size_t uniqueWordCount() const { return wordTable_.size(); }
  std::size_t totalWords() const { return totalWords_; }
  std::size_t bucketCount() const { return wordTable_.bucketCount(); }
  double loadFactor() const { return wordTable_.loadFactor(); }
  std::size_t longestChain() const { return wordTable_.longestChain(); }
  double averageProbes() const { return averageProbes_; }
  int vocabularyHeight() const { return vocabulary_.height(); }
  std::size_t fingerprintTotal() const;

 private:

  void addToPostings(const std::string& word, int documentId, int line);
  void freePostings();

  HashTable<std::string, LinkedList<Occurrence>*> wordTable_;
  WordTree vocabulary_;
  const std::vector<Document>* documents_;
  std::vector<std::vector<unsigned long long> > fingerprints_;
  int kgramSize_;
  std::size_t totalWords_;
  double averageProbes_;
};

// fingerprint
std::vector<unsigned long long> computeFingerprints(
    const std::vector<std::string>& words, int k);

// similarity
struct MatchedSpan {
  int firstStartLine;
  int firstEndLine;
  int secondStartLine;
  int secondEndLine;
  int words;
};

struct PairResult {
  int firstDoc;
  int secondDoc;
  double jaccard;
  double containment;
  int sharedFingerprints;
  int matchedWords;
  std::vector<MatchedSpan> spans;
};

std::vector<PairResult> compareAll(const Index& index, int minSharedFingerprints);

double jaccardSimilarity(const std::vector<unsigned long long>& first,
                         const std::vector<unsigned long long>& second);

int countSharedFingerprints(const std::vector<unsigned long long>& first,
                           const std::vector<unsigned long long>& second);

std::vector<MatchedSpan> findCopiedSpans(const Document& first,
                                         const Document& second,
                                         int chunkWords);

const char* suspicionLabel(double score);

#endif
