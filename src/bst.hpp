// bst.hpp  --  Binary Search Tree  (Week 10 topic: Binary Search Trees)
// Owner: Alyan
//
// Where it is used in the project:
//   The vocabulary of the corpus: every distinct word with the number of
//   documents that contain it. In-order traversal prints the vocabulary in
//   alphabetical order (that is the property of a BST), and we can search a
//   word in O(height) instead of walking a list.
//
// A plain BST is NOT balanced: if we insert sorted words the tree becomes a
// line and searching costs O(n). We accept that here (the words come from the
// text, so they are not sorted) and we show balancing separately in avl.hpp.

#ifndef DIP_BST_HPP
#define DIP_BST_HPP

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace dip {

class WordTree {
 public:
  struct Node {
    std::string word;
    int documentCount;  // in how many documents this word appears
    Node* left;
    Node* right;
    Node(const std::string& w, int c)
        : word(w), documentCount(c), left(nullptr), right(nullptr) {}
  };

  WordTree() : root_(nullptr), size_(0) {}

  // Rule of Three: delete the whole tree in the destructor.
  ~WordTree() { destroy(root_); }
  WordTree(const WordTree&) = delete;
  WordTree& operator=(const WordTree&) = delete;

  // Insert a word or increase its counter if it is already in the tree.
  // Average O(log n) for a random order of insertions, worst case O(n).
  void insert(const std::string& word) {
    root_ = insertNode(root_, word);
  }

  // How many documents contain this word? 0 if the word is unknown.
  // Average O(log n).
  int documentCount(const std::string& word) const {
    Node* current = root_;
    while (current != nullptr) {
      if (word == current->word) return current->documentCount;
      if (word < current->word) {
        current = current->left;
      } else {
        current = current->right;
      }
    }
    return 0;
  }

  bool contains(const std::string& word) const { return documentCount(word) > 0; }

  // In-order traversal (left, node, right) gives the words in sorted order.
  void inOrder(std::vector<std::pair<std::string, int> >& out) const {
    collect(root_, out);
  }

  std::size_t size() const { return size_; }
  int height() const { return heightOf(root_); }

 private:
  Node* insertNode(Node* node, const std::string& word) {
    // Recursion stops when we reach an empty place.
    if (node == nullptr) {
      ++size_;
      return new Node(word, 1);
    }
    if (word == node->word) {
      ++node->documentCount;
    } else if (word < node->word) {
      node->left = insertNode(node->left, word);
    } else {
      node->right = insertNode(node->right, word);
    }
    return node;
  }

  static void collect(Node* node, std::vector<std::pair<std::string, int> >& out) {
    if (node == nullptr) return;
    collect(node->left, out);
    out.push_back(std::make_pair(node->word, node->documentCount));
    collect(node->right, out);
  }

  static int heightOf(Node* node) {
    if (node == nullptr) return 0;
    int leftHeight = heightOf(node->left);
    int rightHeight = heightOf(node->right);
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
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

#endif  // DIP_BST_HPP
