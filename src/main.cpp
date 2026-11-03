// main.cpp  --  the menu of the program
// Owner: Alyan (the menu only calls what is in the other files)
//
// Every menu line uses a different data structure, and the guide says which:
//   1 load folder      -> stack (folder walk), quick sort (file order)
//   2 statistics       -> hash tables, BST
//   3 search a word    -> hash table (inverted index)
//   4 search a phrase  -> brute force / KMP / Rabin-Karp
//   5 detect plagiarism-> k-gram rolling hash + merge + KMP
//   6 top-K pairs      -> max heap
//   7 leaderboard      -> AVL tree
//   8 clusters         -> graph + BFS + queue
//   9 vocabulary       -> BST in-order + binary search
//   0 exit

#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "avl.hpp"
#include "graph.hpp"
#include "heap.hpp"
#include "index.hpp"
#include "loader.hpp"
#include "report.hpp"
#include "similarity.hpp"
#include "sort.hpp"
#include "string_match.hpp"
#include "tokenizer.hpp"

namespace {

using namespace dip;

struct AppData {
  std::vector<Document> documents;
  std::unique_ptr<Index> index;
  std::vector<PairResult> pairs;
  int kgramSize;
  AppData() : kgramSize(5) {}
  bool ready() const { return index.get() != nullptr; }
};

double millisecondsSince(std::chrono::steady_clock::time_point start) {
  std::chrono::duration<double, std::milli> elapsed =
      std::chrono::steady_clock::now() - start;
  return elapsed.count();
}

void printMenu() {
  printLine(std::cout, '=', 64);
  std::cout << "DOCUMENT INDEXER & PLAGIARISM DETECTION ENGINE\n";
  printLine(std::cout, '=', 64);
  std::cout << " 1) Load a folder and build the index\n"
               " 2) Show index statistics\n"
               " 3) Search a word            (hash table)\n"
               " 4) Search a phrase          (brute force / KMP / Rabin-Karp)\n"
               " 5) Detect plagiarism        (k-gram fingerprints + KMP)\n"
               " 6) Show top-K suspicious pairs (max heap)\n"
               " 7) Show the full leaderboard   (AVL tree)\n"
               " 8) Show plagiarism clusters    (graph + BFS)\n"
               " 9) Show the vocabulary      (BST + binary search)\n"
               " 0) Exit\n";
  printLine(std::cout, '-', 64);
}

int readNumber(const std::string& prompt) {
  std::cout << prompt;
  std::string line;
  if (!std::getline(std::cin, line)) return 0;
  return std::atoi(line.c_str());
}

std::string readLine(const std::string& prompt) {
  std::cout << prompt;
  std::string line;
  std::getline(std::cin, line);
  return line;
}

// ---------------------------------------------------------------- menu 1
void loadFolder(AppData& app, const std::string& folder) {
  std::cout << "Reading folder: " << folder << '\n';

  std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

  std::vector<Document> documents;
  std::string error;
  if (!loadCorpus(folder, documents, error)) {
    std::cout << "  ERROR: " << error << '\n';
    return;
  }

  app.documents.swap(documents);  // the index keeps a pointer to this vector
  app.index.reset(new Index());
  app.index->build(app.documents, app.kgramSize);
  app.pairs.clear();

  std::cout << "  " << app.index->documentCount() << " document(s), "
            << app.index->totalWords() << " words, "
            << app.index->uniqueWordCount() << " distinct words  ("
            << millisecondsSince(start) << " ms)\n";
}

// ---------------------------------------------------------------- menu 3
void searchWord(AppData& app) {
  std::string word = readLine("Word to search: ");
  if (word.empty()) return;

  TokenizedText tokens = tokenize(word, false);  // normalise the same way
  if (tokens.words.empty()) {
    std::cout << "  That is not a word.\n";
    return;
  }

  std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
  std::vector<SearchHit> hits = app.index->searchWord(tokens.words[0]);
  double elapsed = millisecondsSince(start);

  printSearchHits(std::cout, *app.index, tokens.words[0], hits);
  std::cout << "  lookup time: " << elapsed << " ms\n";
}

// ---------------------------------------------------------------- menu 4
void searchPhrase(AppData& app) {
  std::string phrase = readLine("Phrase to search: ");
  TokenizedText tokens = tokenize(phrase, false);
  if (tokens.words.empty()) {
    std::cout << "  That is not a phrase.\n";
    return;
  }

  std::string pattern;
  for (std::size_t i = 0; i < tokens.words.size(); ++i) {
    if (i > 0) pattern += ' ';
    pattern += tokens.words[i];
  }

  const std::vector<Document>& documents = app.index->documents();

  // Run all three algorithms, because the point of the lab is to compare them.
  const char* names[3] = {"brute force", "KMP", "Rabin-Karp"};
  for (int algorithm = 0; algorithm < 3; ++algorithm) {
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    int matches = 0;
    std::string firstDocument;
    int firstLine = 0;

    for (std::size_t d = 0; d < documents.size(); ++d) {
      const std::string& text = documents[d].normalized;
      std::vector<std::size_t> positions;
      if (algorithm == 0) {
        positions = bruteForceSearch(text, pattern);
      } else if (algorithm == 1) {
        positions = kmpSearch(text, pattern);
      } else {
        positions = rabinKarpSearch(text, pattern);
      }
      if (!positions.empty()) {
        ++matches;
        if (firstDocument.empty()) {
          firstDocument = documents[d].path;
          firstLine = documents[d].lineOfPosition(positions[0]);
        }
      }
    }
    double elapsed = millisecondsSince(start);
    std::cout << "  " << names[algorithm] << ":\tdocuments with a match: "
              << matches << "\ttime: " << elapsed << " ms";
    if (!firstDocument.empty()) {
      std::cout << "   (first: " << firstDocument << " line " << firstLine << ")";
    }
    std::cout << '\n';
  }
  std::cout << "  All three must report the same number of matches.\n";
}

// ---------------------------------------------------------------- menu 5
void detectPlagiarism(AppData& app) {
  std::cout << "Comparing " << app.index->documentCount()
            << " documents (k = " << app.index->kgramSize() << ")...\n";

  std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
  app.pairs = compareAll(*app.index, 1);
  double elapsed = millisecondsSince(start);

  // Biggest score first: our own merge sort (O(n log n)).
  std::vector<PairResult> sorted = app.pairs;
  mergeSortPairs(sorted);

  std::cout << "  " << sorted.size() << " pair(s) above the score limit   ("
            << elapsed << " ms)\n";
  printPairs(std::cout, *app.index, sorted, true);

  std::string answer = readLine("Save this report to a file? (y/n): ");
  if (answer == "y" || answer == "Y") {
    std::string fileName = readLine("File name: ");
    if (fileName.empty()) fileName = "report.txt";
    std::ofstream out(fileName.c_str());
    if (!out.is_open()) {
      std::cout << "  Cannot write " << fileName << '\n';
      return;
    }
    out << "Plagiarism report for " << app.index->documentCount()
        << " documents (k = " << app.index->kgramSize() << ")\n";
    printPairs(out, *app.index, sorted, true);
    std::cout << "  Written to " << fileName << '\n';
  }
}

// ---------------------------------------------------------------- menu 6
void showTopPairs(AppData& app) {
  if (app.pairs.empty()) {
    std::cout << "  Run option 5 first.\n";
    return;
  }
  int k = readNumber("How many pairs? ");
  if (k <= 0) k = 5;

  MaxHeap heap;
  for (std::size_t i = 0; i < app.pairs.size(); ++i) {
    ScoreEntry entry;
    entry.score = app.pairs[i].jaccard;
    entry.firstDoc = app.pairs[i].firstDoc;
    entry.secondDoc = app.pairs[i].secondDoc;
    heap.push(entry);
  }
  std::vector<ScoreEntry> best = heap.topK(static_cast<std::size_t>(k));
  insertionSortScores(best);  // K is small, insertion sort is the right tool
  printTopK(std::cout, *app.index, best);
}

// ---------------------------------------------------------------- menu 7
void showLeaderboard(AppData& app) {
  if (app.pairs.empty()) {
    std::cout << "  Run option 5 first.\n";
    return;
  }
  Leaderboard board;  // the AVL tree
  for (std::size_t i = 0; i < app.pairs.size(); ++i) {
    board.insert(app.pairs[i].jaccard, app.pairs[i].firstDoc, app.pairs[i].secondDoc);
  }
  std::vector<Leaderboard::Node> nodes = board.rankedDescending();

  std::vector<std::pair<double, std::pair<int, int> > > ranked;
  for (std::size_t i = 0; i < nodes.size(); ++i) {
    ranked.push_back(std::make_pair(
        nodes[i].score, std::make_pair(nodes[i].firstDoc, nodes[i].secondDoc)));
  }
  printLeaderboard(std::cout, *app.index, ranked);
  std::cout << "  AVL height: " << board.height() << " for " << board.size()
            << " node(s)  (nothing degenerates into a list)\n";
}

// ---------------------------------------------------------------- menu 8
void showClusters(AppData& app) {
  if (app.pairs.empty()) {
    std::cout << "  Run option 5 first.\n";
    return;
  }
  std::cout << "Add an edge between two documents when their score is at least: ";
  std::string line;
  std::getline(std::cin, line);
  double threshold = std::atof(line.c_str());
  if (threshold <= 0.0) threshold = 0.35;

  Graph graph(static_cast<int>(app.index->documentCount()));
  for (std::size_t i = 0; i < app.pairs.size(); ++i) {
    if (app.pairs[i].jaccard >= threshold) {
      graph.addEdge(app.pairs[i].firstDoc, app.pairs[i].secondDoc);
    }
  }
  std::cout << "  similarity graph: " << graph.vertexCount() << " vertices, "
            << graph.edgeCount() << " edges\n";
  printClusters(std::cout, *app.index, graph.clusters());
}

// ---------------------------------------------------------------- menu 9
void showVocabulary(AppData& app) {
  std::vector<std::string> words = app.index->sortedVocabulary();  // BST in-order
  printVocabulary(std::cout, words, 30);

  std::string word = readLine("Check a word with binary search (Enter to skip): ");
  if (word.empty()) return;
  TokenizedText tokens = tokenize(word, false);
  if (tokens.words.empty()) return;

  int position = binarySearchWord(words, tokens.words[0]);
  if (position < 0) {
    std::cout << "  \"" << tokens.words[0] << "\" is NOT in the vocabulary\n";
  } else {
    std::cout << "  \"" << tokens.words[0] << "\" found at position " << position
              << " of " << words.size() << " (binary search), used in "
              << app.index->documentFrequency(tokens.words[0])
              << " document(s) (BST lookup)\n";
  }
}

}  // namespace

int main(int argc, char** argv) {
  AppData app;
  std::cout << "Document Indexer & Plagiarism Detection Engine\n";
  std::cout << "Data Structures Lab project - C++17\n\n";

  if (argc > 1) {
    loadFolder(app, argv[1]);
  }

  while (true) {
    printMenu();
    int choice = readNumber("Choice: ");
    if (choice == 0) {
      std::cout << "Bye.\n";
      break;
    }
    if (choice != 1 && !app.ready()) {
      std::cout << "Load a folder first (option 1).\n";
      continue;
    }
    switch (choice) {
      case 1: {
        std::string folder = (argc > 1) ? std::string(argv[1]) : readLine("Folder: ");
        loadFolder(app, folder);
        break;
      }
      case 2: printIndexStats(std::cout, *app.index); break;
      case 3: searchWord(app); break;
      case 4: searchPhrase(app); break;
      case 5: detectPlagiarism(app); break;
      case 6: showTopPairs(app); break;
      case 7: showLeaderboard(app); break;
      case 8: showClusters(app); break;
      case 9: showVocabulary(app); break;
      default: std::cout << "Unknown choice.\n"; break;
    }
  }
  return 0;
}
