// Implementation of AVL tree leaderboard
#include "rank/rank_tree.hpp"

#include "search/scorer.hpp"

namespace plag {
namespace {

// Returns true if 'a' should be placed to the left of 'b' (lower score)
bool score_less(const PairScore& a, const PairScore& b) {
  double ka = suspicion_key(a);
  double kb = suspicion_key(b);
  if (ka != kb) return ka < kb;
  if (a.matchedTokens != b.matchedTokens) {
    return a.matchedTokens < b.matchedTokens;
  }
  if (a.docA != b.docA) return a.docA > b.docA;
  return a.docB > b.docB;
}

}  // namespace

RankTree::RankTree() = default;

RankTree::~RankTree() {
  destroy(root_);
}

void RankTree::insert(const PairScore& p) {
  root_ = insert_node(root_, p);
}

std::size_t RankTree::size() const { return size_; }

std::size_t RankTree::height() const {
  return static_cast<std::size_t>(height_of(root_));
}

std::size_t RankTree::top_k(std::size_t k, DynArray<PairScore>& out) const {
  out.clear();
  if (k == 0 || root_ == nullptr) {
    return 0;
  }
  collect_descending(root_, k, out);
  return out.size();
}

void RankTree::destroy(Node* n) {
  if (n == nullptr) return;
  destroy(n->left);
  destroy(n->right);
  delete n;
}

int RankTree::height_of(const Node* n) const {
  return (n == nullptr) ? 0 : n->height;
}

int RankTree::balance_factor(const Node* n) const {
  if (n == nullptr) return 0;
  return height_of(n->left) - height_of(n->right);
}

void RankTree::update_height(Node* n) {
  int lh = height_of(n->left);
  int rh = height_of(n->right);
  n->height = 1 + (lh > rh ? lh : rh);
}

RankTree::Node* RankTree::rotate_right(Node* y) {
  Node* x = y->left;
  Node* t2 = x->right;

  x->right = y;
  y->left = t2;

  update_height(y);
  update_height(x);
  return x;
}

RankTree::Node* RankTree::rotate_left(Node* x) {
  Node* y = x->right;
  Node* t2 = y->left;

  y->left = x;
  x->right = t2;

  update_height(x);
  update_height(y);
  return y;
}

RankTree::Node* RankTree::insert_node(Node* node, const PairScore& p) {
  if (node == nullptr) {
    Node* created = new Node{};
    created->val = p;
    created->height = 1;
    ++size_;
    return created;
  }

  if (score_less(p, node->val)) {
    node->left = insert_node(node->left, p);
  } else {
    node->right = insert_node(node->right, p);
  }

  update_height(node);
  int bf = balance_factor(node);

  // Left-Left case
  if (bf > 1 && balance_factor(node->left) >= 0) {
    return rotate_right(node);
  }
  // Left-Right case
  if (bf > 1 && balance_factor(node->left) < 0) {
    node->left = rotate_left(node->left);
    return rotate_right(node);
  }
  // Right-Right case
  if (bf < -1 && balance_factor(node->right) <= 0) {
    return rotate_left(node);
  }
  // Right-Left case
  if (bf < -1 && balance_factor(node->right) > 0) {
    node->right = rotate_right(node->right);
    return rotate_left(node);
  }

  return node;
}

void RankTree::collect_descending(const Node* n, std::size_t k,
                                  DynArray<PairScore>& out) const {
  if (n == nullptr || out.size() >= k) {
    return;
  }
  collect_descending(n->right, k, out);
  if (out.size() < k) {
    out.push_back(n->val);
  }
  collect_descending(n->left, k, out);
}

}  // namespace plag
