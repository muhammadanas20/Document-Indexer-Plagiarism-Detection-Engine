// report.cpp  --  see report.hpp
// Owner: Alyan

#include "report.hpp"

#include <cstddef>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace dip {
namespace {

std::string shortPath(const std::string& path) {
  std::size_t slash = path.find_last_of('/');
  return (slash == std::string::npos) ? path : path.substr(slash + 1);
}

std::string scoreText(double value) {
  std::ostringstream text;
  text << std::fixed << std::setprecision(3) << value;
  return text.str();
}

}  // namespace

void printLine(std::ostream& out, char character, int count) {
  for (int i = 0; i < count; ++i) out << character;
  out << '\n';
}

void printIndexStats(std::ostream& out, const Index& index) {
  printLine(out, '=', 64);
  out << "INDEX STATISTICS\n";
  printLine(out, '=', 64);
  out << "Documents indexed      : " << index.documentCount() << '\n';
  out << "Total words (tokens)   : " << index.totalWords() << '\n';
  out << "Distinct words         : " << index.uniqueWordCount() << '\n';
  out << "k-gram size used       : " << index.kgramSize() << '\n';
  out << "Total k-gram hashes    : " << index.fingerprintTotal() << '\n';
  printLine(out, '-', 64);
  out << "HASH TABLE (separate chaining)\n";
  out << "  buckets              : " << index.bucketCount() << '\n';
  out << "  stored words         : " << index.uniqueWordCount() << '\n';
  out << "  load factor          : " << scoreText(index.loadFactor())
      << "   (we rehash when it passes 0.75)\n";
  out << "  longest chain        : " << index.longestChain() << '\n';
  out << "  avg chain length     : "
      << scoreText(static_cast<double>(index.uniqueWordCount()) /
                   static_cast<double>(index.bucketCount()))
      << '\n';
  printLine(out, '-', 64);
  out << "HASH TABLE (linear probing, per document)\n";
  out << "  average probes per insert : " << scoreText(index.averageProbes())
      << "   (1.0 = no collision at all)\n";
  printLine(out, '-', 64);
  out << "BINARY SEARCH TREE (vocabulary)\n";
  out << "  nodes                : " << index.uniqueWordCount() << '\n';
  out << "  height               : " << index.vocabularyHeight() << '\n';
  if (index.uniqueWordCount() > 1) {
    double balanced = std::log(static_cast<double>(index.uniqueWordCount())) /
                      std::log(2.0);
    out << "  what a perfectly balanced tree would have : about "
        << static_cast<int>(balanced + 0.5) << " levels\n";
  }
  out << "  (the words arrive in text order, so the tree is not balanced -\n"
         "   that is why the leaderboard uses the AVL tree instead)\n";
  printLine(out, '=', 64);
}

void printSearchHits(std::ostream& out, const Index& index,
                     const std::string& word,
                     const std::vector<SearchHit>& hits) {
  const std::vector<Document>& documents = index.documents();
  out << "Word \"" << word << "\" found in " << hits.size()
      << " document(s) [hash table lookup]\n";
  if (hits.empty()) {
    out << "  (the vocabulary tree says it appears in "
        << index.documentFrequency(word) << " document(s))\n";
    return;
  }
  for (std::size_t i = 0; i < hits.size(); ++i) {
    out << "  " << shortPath(documents[static_cast<std::size_t>(hits[i].documentId)].path)
        << "  line " << hits[i].firstLine << "  x" << hits[i].count << '\n';
  }
}

void printPairs(std::ostream& out, const Index& index,
                const std::vector<PairResult>& pairs, bool withSpans) {
  const std::vector<Document>& documents = index.documents();
  printLine(out, '-', 64);
  out << "SUSPICIOUS PAIRS: " << pairs.size() << " pair(s) above the score limit\n";
  if (pairs.empty()) {
    out << "  Nothing suspicious found.\n";
    return;
  }

  out << std::left << std::setw(4) << "#" << std::setw(34) << "PAIR"
      << std::setw(8) << "SCORE" << std::setw(13) << "CONTAINMENT"
      << std::setw(9) << "WORDS" << "VERDICT" << '\n';
  for (std::size_t i = 0; i < pairs.size(); ++i) {
    const PairResult& pair = pairs[i];
    std::string names =
        shortPath(documents[static_cast<std::size_t>(pair.firstDoc)].path) + " / " +
        shortPath(documents[static_cast<std::size_t>(pair.secondDoc)].path);
    if (names.size() > 33) names = names.substr(0, 30) + "...";
    out << std::left << std::setw(4) << (i + 1) << std::setw(34) << names
        << std::setw(8) << scoreText(pair.jaccard) << std::setw(13)
        << scoreText(pair.containment) << std::setw(9) << pair.matchedWords
        << suspicionLabel(pair.jaccard) << '\n';

    if (withSpans && !pair.spans.empty()) {
      const std::size_t maxSpans = 5;
      for (std::size_t s = 0; s < pair.spans.size() && s < maxSpans; ++s) {
        const MatchedSpan& span = pair.spans[s];
        out << "        copied: " << shortPath(documents[static_cast<std::size_t>(pair.firstDoc)].path)
            << " lines " << span.firstStartLine << "-" << span.firstEndLine
            << "  ==  "
            << shortPath(documents[static_cast<std::size_t>(pair.secondDoc)].path)
            << " lines " << span.secondStartLine << "-" << span.secondEndLine
            << "   (" << span.words << " words, found with KMP)\n";
      }
      if (pair.spans.size() > maxSpans) {
        out << "        ... " << (pair.spans.size() - maxSpans)
            << " more matched blocks\n";
      }
    }
  }
  printLine(out, '-', 64);
}

void printTopK(std::ostream& out, const Index& index,
               const std::vector<ScoreEntry>& entries) {
  const std::vector<Document>& documents = index.documents();
  out << "TOP " << entries.size() << " most similar pairs (taken from the max heap)\n";
  if (entries.empty()) {
    out << "  The heap is empty - run 'Detect plagiarism' first.\n";
    return;
  }
  for (std::size_t i = 0; i < entries.size(); ++i) {
    out << "  " << (i + 1) << ". " << scoreText(entries[i].score) << "  "
        << shortPath(documents[static_cast<std::size_t>(entries[i].firstDoc)].path)
        << " / "
        << shortPath(documents[static_cast<std::size_t>(entries[i].secondDoc)].path)
        << "   [" << suspicionLabel(entries[i].score) << "]\n";
  }
}

void printLeaderboard(
    std::ostream& out, const Index& index,
    const std::vector<std::pair<double, std::pair<int, int> > >& ranked) {
  const std::vector<Document>& documents = index.documents();
  out << "FULL LEADERBOARD from the AVL tree, " << ranked.size() << " pair(s)\n";
  out << "(reverse in-order traversal: the tree gives them already sorted)\n";
  if (ranked.empty()) {
    out << "  The leaderboard is empty - run 'Detect plagiarism' first.\n";
    return;
  }
  for (std::size_t i = 0; i < ranked.size(); ++i) {
    int first = ranked[i].second.first;
    int second = ranked[i].second.second;
    out << "  " << (i + 1) << ". " << scoreText(ranked[i].first) << "  "
        << shortPath(documents[static_cast<std::size_t>(first)].path) << " / "
        << shortPath(documents[static_cast<std::size_t>(second)].path) << '\n';
  }
}

void printClusters(std::ostream& out, const Index& index,
                   const std::vector<std::vector<int> >& clusters) {
  const std::vector<Document>& documents = index.documents();
  out << "PLAGIARISM CLUSTERS found with BFS on the similarity graph: "
      << clusters.size() << '\n';
  if (clusters.empty()) {
    out << "  No document is linked to another, so there are no clusters.\n";
    return;
  }
  for (std::size_t c = 0; c < clusters.size(); ++c) {
    out << "  Cluster " << (c + 1) << " (" << clusters[c].size() << " documents)\n";
    for (std::size_t i = 0; i < clusters[c].size(); ++i) {
      out << "     - "
          << documents[static_cast<std::size_t>(clusters[c][i])].path << '\n';
    }
  }
}

void printVocabulary(std::ostream& out, const std::vector<std::string>& words,
                     std::size_t limit) {
  out << "Vocabulary in alphabetical order (BST in-order traversal), "
      << words.size() << " words\n";
  std::size_t shown = (limit == 0 || limit > words.size()) ? words.size() : limit;
  for (std::size_t i = 0; i < shown; ++i) {
    out << "  " << words[i];
    if ((i + 1) % 6 == 0) out << '\n';
  }
  if (shown < words.size()) {
    out << "\n  ... " << (words.size() - shown) << " more\n";
  } else {
    out << '\n';
  }
}

}  // namespace dip
