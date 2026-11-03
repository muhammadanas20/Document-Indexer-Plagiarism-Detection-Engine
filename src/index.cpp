// index.cpp  --  see index.hpp
// Owner: Anas

#include "index.hpp"

#include <cstddef>

#include "fingerprint.hpp"
#include "probing.hpp"

namespace dip {

Index::~Index() { freePostings(); }

void Index::freePostings() {
  for (std::size_t b = 0; b < wordTable_.bucketCount(); ++b) {
    typename LinkedList<std::pair<std::string, LinkedList<Occurrence>*> >::Node* node =
        wordTable_.bucket(b)->head();
    for (; node != nullptr; node = node->next) {
      delete node->value.second;  // the posting list of that word
    }
  }
}

void Index::addToPostings(const std::string& word, int documentId, int line) {
  LinkedList<Occurrence>** slot = wordTable_.get(word);
  if (slot == nullptr) {
    // First time we see this word: create its posting list.
    LinkedList<Occurrence>* list = new LinkedList<Occurrence>();
    list->pushBack(Occurrence{documentId, line, 1});
    wordTable_.put(word, list);
    vocabulary_.insert(word);  // one more document contains this word
    return;
  }

  LinkedList<Occurrence>* list = *slot;
  for (LinkedList<Occurrence>::Node* node = list->head(); node != nullptr;
       node = node->next) {
    if (node->value.documentId == documentId) {
      ++node->value.count;  // same word again in the same document
      return;
    }
  }
  list->pushBack(Occurrence{documentId, line, 1});
  vocabulary_.insert(word);  // the word appears in one more document
}

void Index::build(const std::vector<Document>& documents, int k) {
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

    // A linear-probing table counts the words of THIS document only.
    // We use it for the statistics and to compare it with chaining.
    LinearProbeTable counters(32);
    for (std::size_t i = 0; i < document.words.size(); ++i) {
      counters.add(document.words[i]);
      addToPostings(document.words[i], documentId, document.wordLines[i]);
    }
    if (counters.size() > 0) {
      probeSum += counters.averageProbes();
      ++documentCounter;
    }

    // The k-gram fingerprints of this document (rolling hash).
    fingerprints_[d] = computeFingerprints(document.words, kgramSize_);
  }

  averageProbes_ = (documentCounter == 0) ? 0.0 : probeSum / static_cast<double>(documentCounter);
}

std::vector<SearchHit> Index::searchWord(const std::string& word) const {
  std::vector<SearchHit> hits;
  // get() needs a non-const table pointer, so we cast away constness of the
  // table here on purpose: we only read from it.
  HashTable<std::string, LinkedList<Occurrence>*>& table =
      const_cast<HashTable<std::string, LinkedList<Occurrence>*>&>(wordTable_);
  LinkedList<Occurrence>** slot = table.get(word);
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
  vocabulary_.inOrder(entries);  // BST in-order = alphabetical
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

}  // namespace dip
