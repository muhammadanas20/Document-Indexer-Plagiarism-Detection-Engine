// test_trees_heap.cpp  --  BST, AVL leaderboard and max heap
// Owner: Alyan

#include <string>
#include <vector>

#include "avl.hpp"
#include "bst.hpp"
#include "heap.hpp"
#include "test_util.hpp"

using namespace dip;

int main() {
  // ---- BST: the vocabulary ------------------------------------------------
  WordTree tree;
  tree.insert("sum");
  tree.insert("total");
  tree.insert("loop");
  tree.insert("sum");   // same word in a second document
  tree.insert("array");

  CHECK(tree.size() == 4);                  // 4 distinct words
  CHECK(tree.documentCount("sum") == 2);    // it is in two documents
  CHECK(tree.documentCount("loop") == 1);
  CHECK(tree.documentCount("nothing") == 0);
  CHECK(tree.contains("array"));
  CHECK(!tree.contains("zzz"));

  std::vector<std::pair<std::string, int> > words;
  tree.inOrder(words);                      // in-order = alphabetical
  CHECK(words.size() == 4);
  CHECK(words[0].first == "array");
  CHECK(words[1].first == "loop");
  CHECK(words[2].first == "sum");
  CHECK(words[3].first == "total");         // sorted!
  CHECK(tree.height() == 3);

  // ---- AVL: the leaderboard ----------------------------------------------
  Leaderboard board;
  for (int i = 0; i < 1000; ++i) {
    board.insert(static_cast<double>(i) / 1000.0, i, i + 1);
  }
  CHECK(board.size() == 1000);

  // A plain BST would become a 1000-deep line for sorted input. The AVL tree
  // has to stay close to log2(1000) ~ 10 levels.
  CHECK(board.height() <= 16);

  std::vector<Leaderboard::Node> ranked = board.rankedDescending();
  CHECK(ranked.size() == 1000);
  CHECK(ranked[0].score > ranked[1].score);          // biggest score first
  CHECK(ranked[999].score < ranked[0].score);
  bool sorted = true;
  for (std::size_t i = 1; i < ranked.size(); ++i) {
    if (ranked[i - 1].score < ranked[i].score) sorted = false;
  }
  CHECK(sorted);

  Leaderboard small;
  small.insert(0.5, 0, 1);
  small.insert(0.9, 1, 2);
  small.insert(0.1, 2, 3);
  CHECK(small.height() == 2);
  std::vector<Leaderboard::Node> three = small.rankedDescending();
  CHECK(three.size() == 3);
  CHECK(three[0].score == 0.9);
  CHECK(three[2].score == 0.1);

  // ---- max heap -----------------------------------------------------------
  MaxHeap heap;
  CHECK(heap.empty());
  double scores[6] = {0.35, 0.90, 0.10, 0.75, 0.55, 0.62};
  for (int i = 0; i < 6; ++i) {
    ScoreEntry entry;
    entry.score = scores[i];
    entry.firstDoc = i;
    entry.secondDoc = i + 1;
    heap.push(entry);
  }
  CHECK(heap.size() == 6);

  std::vector<ScoreEntry> best = heap.topK(3);
  CHECK(best.size() == 3);
  CHECK(best[0].score == 0.90);      // the biggest one comes out first
  CHECK(best[1].score == 0.75);
  CHECK(best[2].score == 0.62);
  CHECK(heap.size() == 3);           // we only popped three

  ScoreEntry next = heap.pop();
  CHECK(next.score == 0.55);         // the heap is still valid

  MaxHeap single;
  ScoreEntry only;
  only.score = 1.0;
  only.firstDoc = 0;
  only.secondDoc = 1;
  single.push(only);
  CHECK(single.pop().score == 1.0);
  CHECK(single.empty());

  return testutil::summary("test_trees_heap");
}
