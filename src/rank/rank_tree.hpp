// Self-balancing AVL tree for maintaining a sorted leaderboard of pairs
#ifndef PLAG_SRC_RANK_RANK_TREE_HPP_
#define PLAG_SRC_RANK_RANK_TREE_HPP_

#include <cstddef>

#include "common/dyn_array.hpp"
#include "contract/contract.hpp"

namespace plag {

class RankTree {
 public:
  RankTree();
  ~RankTree();
  RankTree(const RankTree&) = delete;
  RankTree& operator=(const RankTree&) = delete;

  void insert(const PairScore& p);
  std::size_t size() const;
  std::size_t height() const;

  std::size_t top_k(std::size_t k, DynArray<PairScore>& out) const;

 private:
  struct Node {
    PairScore val;
    Node* left = nullptr;
    Node* right = nullptr;
    int height = 1;
  };

  void destroy(Node* n);
  int height_of(const Node* n) const;
  int balance_factor(const Node* n) const;
  void update_height(Node* n);
  Node* rotate_right(Node* y);
  Node* rotate_left(Node* x);
  Node* insert_node(Node* node, const PairScore& p);
  void collect_descending(const Node* n, std::size_t k,
                          DynArray<PairScore>& out) const;

  Node* root_ = nullptr;
  std::size_t size_ = 0;
};

}  // namespace plag

#endif  // PLAG_SRC_RANK_RANK_TREE_HPP_
