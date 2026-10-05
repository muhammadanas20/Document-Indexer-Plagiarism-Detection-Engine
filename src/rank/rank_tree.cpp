// rank_tree.cpp -- STUB (Alyan, W4).
#include "rank/rank_tree.hpp"

namespace plag {

RankTree::RankTree() = default;

RankTree::~RankTree() { destroy(root_); }

void RankTree::insert(const PairScore& /*p*/) {
  // TODO(Alyan W4): BST insert + rebalance (LL, RR, LR, RL).
}

std::size_t RankTree::size() const { return size_; }
std::size_t RankTree::height() const { return static_cast<std::size_t>(height_of(root_)); }

std::size_t RankTree::top_k(std::size_t /*k*/, DynArray<PairScore>& /*out*/) const {
  return 0;  // STUB dummy
}

void RankTree::destroy(Node* n) {
  if (n == nullptr) return;
  destroy(n->left);
  destroy(n->right);
  delete n;
}

int RankTree::height_of(const Node* n) const {
  if (n == nullptr) return 0;
  const int l = height_of(n->left);
  const int r = height_of(n->right);
  return 1 + (l > r ? l : r);
}

}  // namespace plag
