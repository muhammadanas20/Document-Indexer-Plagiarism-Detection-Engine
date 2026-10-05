// rank_tree.hpp -- AVL tree leaderboard.
// Owner: ALYAN.  src/rank/ is Alyan's area. This is the viva risk -- reviewed
// for 45 minutes, not 5.
// STUB (Alyan, W4): dummy bodies.
#ifndef PLAG_SRC_RANK_RANK_TREE_HPP_
#define PLAG_SRC_RANK_RANK_TREE_HPP_

#include <cstddef>

#include "common/dyn_array.hpp"
#include "contract/contract.hpp"

namespace plag {

// Self-balancing BST keyed by suspicion_key(). Rotations keep the height at
// O(log n) so the leaderboard never degrades into a list on sorted input.
class RankTree {
 public:
  RankTree();
  ~RankTree();
  RankTree(const RankTree&) = delete;
  RankTree& operator=(const RankTree&) = delete;

  void insert(const PairScore& p);
  std::size_t size() const;
  std::size_t height() const;

  // In-order walk, highest first.
  std::size_t top_k(std::size_t k, DynArray<PairScore>& out) const;

 private:
  struct Node {
    PairScore val;
    Node* left = nullptr;
    Node* right = nullptr;
    int balance = 0;
  };

  void destroy(Node* n);
  int height_of(const Node* n) const;

  Node* root_ = nullptr;
  std::size_t size_ = 0;
};

}  // namespace plag

#endif  // PLAG_SRC_RANK_RANK_TREE_HPP_
