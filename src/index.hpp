// index.hpp  --  the inverted index
// (Week 13: hashing  +  Week 10: BST  +  Week 12: rolling hash)
// Owner: Anas
//
// The inverted index is the main data structure of a "document indexer".
// Instead of asking every document "do you contain the word sum?" (a linear
// search over the whole corpus), we ask the hash table directly:
//
//        "sum"  ->  [ ali.cpp line 4 x3 ] -> [ bilal.cpp line 7 x1 ] -> null
//
//   key   = the word
//   value = a linked list of occurrences (which document, which line, how often)
//
// So one search costs O(1) average instead of O(total words).
//
// The same build also fills:
//   * a BST with the vocabulary (word -> in how many documents), so we can
//     print the vocabulary alphabetically and binary-search it,
//   * one linear-probing table per document, only to collect statistics
//     (distinct words, average probes) and to compare the two collision
//     resolution methods in the report,
//   * the k-gram fingerprints of every document (used by similarity.cpp).

#ifndef DIP_INDEX_HPP
#define DIP_INDEX_HPP

#include <cstddef>
#include <string>
#include <vector>

#include "bst.hpp"
#include "document.hpp"
#include "hash_table.hpp"
#include "list.hpp"

namespace dip {

// One document that contains a word.
struct Occurrence {
  int documentId;
  int firstLine;  // 1-based line of the first appearance in that document
  int count;      // how many times the word appears in that document
};

// Result of "search a word" - one row of the answer.
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

  // Build everything from the loaded documents. k = k-gram window size.
  void build(const std::vector<Document>& documents, int k);

  // ---- searching ---------------------------------------------------------
  // All documents that contain `word`, in the order they were indexed.
  std::vector<SearchHit> searchWord(const std::string& word) const;

  // In how many documents does the word appear? (BST lookup, O(log n))
  int documentFrequency(const std::string& word) const;

  // The vocabulary in alphabetical order (BST in-order traversal).
  std::vector<std::string> sortedVocabulary() const;

  // ---- data the report and the similarity module need --------------------
  const std::vector<Document>& documents() const { return *documents_; }
  const std::vector<std::vector<unsigned long long> >& fingerprints() const {
    return fingerprints_;
  }
  int kgramSize() const { return kgramSize_; }

  // ---- statistics --------------------------------------------------------
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
  // Add the word once for document `documentId` (insert or ++count).
  void addToPostings(const std::string& word, int documentId, int line);
  void freePostings();

  HashTable<std::string, LinkedList<Occurrence>*> wordTable_;
  WordTree vocabulary_;
  const std::vector<Document>* documents_;  // points at the caller's vector
  std::vector<std::vector<unsigned long long> > fingerprints_;
  int kgramSize_;
  std::size_t totalWords_;
  double averageProbes_;
};

}  // namespace dip

#endif  // DIP_INDEX_HPP
