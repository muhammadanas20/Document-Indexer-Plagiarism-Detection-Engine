// avl.hpp  --  AVL tree (self-balancing BST)  (Week 11 topic: AVL trees)
// Owner: Alyan
//
// Where it is used in the project:
//   The leaderboard of document pairs. We insert every pair with its
//   similarity score, and the in-order traversal gives the whole leaderboard
//   already sorted from the highest score down.
//
// Why an AVL tree and not the plain BST from bst.hpp?
//   Here the keys arrive one by one in no particular order, and the tree must
//   stay fast for many insertions. An AVL tree keeps |height(left) -
//   height(right)| <= 1 for every node by rotating after an insertion, so the
//   height stays about 1.44 * log2(n) and insert/search stay O(log n).

#ifndef DIP_AVL_HPP
#define DIP_AVL_HPP

#include <cstddef>
#include <vector>

namespace dip {

class Leaderboard {
 public:
  struct Node {
    double score;
    int firstDoc;
    int secondDoc;
    int height;  // height of the subtree that starts at this node
    Node* left;
    Node* right;
    Node(double s, int a, int b)
        : score(s), firstDoc(a), secondDoc(b), height(1),
          left(nullptr), right(nullptr) {}
  };

  Leaderboard() : root_(nullptr), size_(0) {}
  ~Leaderboard() { destroy(root_); }
  Leaderboard(const Leaderboard&) = delete;
  Leaderboard& operator=(const Leaderboard&) = delete;

  // Insert one pair. O(log n).
  void insert(double score, int firstDoc, int secondDoc) {
    root_ = insertNode(root_, score, firstDoc, secondDoc);
    ++size_;
  }

  // All pairs, sorted from the biggest score to the smallest.
  // Reverse in-order traversal: right subtree first, then the node, then left.
  std::vector<Node> rankedDescending() const {
    std::vector<Node> out;
    collectDescending(root_, out);
    return out;
  }

  int height() const { return heightOf(root_); }  // balanced => about log2(n)
  std::size_t size() const { return size_; }

 private:
  static int heightOf(Node* node) { return node == nullptr ? 0 : node->height; }
  static int balanceOf(Node* node) {
    return node == nullptr ? 0 : heightOf(node->left) - heightOf(node->right);
  }
  static void updateHeight(Node* node) {
    int leftHeight = heightOf(node->left);
    int rightHeight = heightOf(node->right);
    node->height = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
  }

  // ---- the four rotations (the heart of the AVL tree) ----------------------
  // Right rotation. Used when the left side became too tall (Left-Left case).
  static Node* rotateRight(Node* y) {
    Node* x = y->left;
    Node* middle = x->right;
    x->right = y;
    y->left = middle;
    updateHeight(y);
    updateHeight(x);
    return x;  // x is the new root of this subtree
  }

  // Left rotation. Used when the right side became too tall (Right-Right case).
  static Node* rotateLeft(Node* x) {
    Node* y = x->right;
    Node* middle = y->left;
    y->left = x;
    x->right = middle;
    updateHeight(x);
    updateHeight(y);
    return y;
  }

  // Rebalance one node after an insertion: pick the rotation from the balance
  // factor. Left-Right and Right-Left need two rotations.
  static Node* rebalance(Node* node) {
    updateHeight(node);
    int balance = balanceOf(node);

    if (balance > 1) {  // left side too tall
      if (balanceOf(node->left) < 0) node->left = rotateLeft(node->left);  // LR
      return rotateRight(node);
    }
    if (balance < -1) {  // right side too tall
      if (balanceOf(node->right) > 0) node->right = rotateRight(node->right);  // RL
      return rotateLeft(node);
    }
    return node;  // already balanced
  }

  static Node* insertNode(Node* node, double score, int firstDoc, int secondDoc) {
    if (node == nullptr) return new Node(score, firstDoc, secondDoc);

    // Same score goes to the right; the tree sorts by score only.
    if (score < node->score) {
      node->left = insertNode(node->left, score, firstDoc, secondDoc);
    } else {
      node->right = insertNode(node->right, score, firstDoc, secondDoc);
    }
    return rebalance(node);  // walk back up and fix the heights
  }

  static void collectDescending(Node* node, std::vector<Node>& out) {
    if (node == nullptr) return;
    collectDescending(node->right, out);
    out.push_back(*node);  // copies the data, not the pointers
    collectDescending(node->left, out);
  }

  static void destroy(Node* node) {
    if (node == nullptr) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
  }

  Node* root_;
  std::size_t size_;
};

}  // namespace dip

#endif  // DIP_AVL_HPP
