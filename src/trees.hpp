#ifndef PROJECT_TREES_HPP
#define PROJECT_TREES_HPP

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

// bst
class WordTree {
 public:
  struct Node {
    std::string word;
    int documentCount;
    Node* left;
    Node* right;
    Node(const std::string& w, int c)
        : word(w), documentCount(c), left(nullptr), right(nullptr) {}
  };

  WordTree() : root_(nullptr), size_(0) {}

  ~WordTree() { clear(); }
  void clear() {
    destroy(root_);
    root_ = nullptr;
    size_ = 0;
  }
  WordTree(const WordTree&) = delete;
  WordTree& operator=(const WordTree&) = delete;

  void insert(const std::string& word) {
    root_ = insertNode(root_, word);
  }

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

  void inOrder(std::vector<std::pair<std::string, int> >& out) const {
    collect(root_, out);
  }

  std::size_t size() const { return size_; }
  int height() const { return heightOf(root_); }

 private:
  Node* insertNode(Node* node, const std::string& word) {

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

// avl
class Leaderboard {
 public:
  struct Node {
    double score;
    int firstDoc;
    int secondDoc;
    int height;
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

  void insert(double score, int firstDoc, int secondDoc) {
    root_ = insertNode(root_, score, firstDoc, secondDoc);
    ++size_;
  }

  std::vector<Node> rankedDescending() const {
    std::vector<Node> out;
    collectDescending(root_, out);
    return out;
  }

  int height() const { return heightOf(root_); }
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

  static Node* rotateRight(Node* y) {
    Node* x = y->left;
    Node* middle = x->right;
    x->right = y;
    y->left = middle;
    updateHeight(y);
    updateHeight(x);
    return x;
  }

  static Node* rotateLeft(Node* x) {
    Node* y = x->right;
    Node* middle = y->left;
    y->left = x;
    x->right = middle;
    updateHeight(x);
    updateHeight(y);
    return y;
  }

  // turn the tree when one side gets too tall
  static Node* rebalance(Node* node) {
    updateHeight(node);
    int balance = balanceOf(node);

    if (balance > 1) {
      if (balanceOf(node->left) < 0) node->left = rotateLeft(node->left);
      return rotateRight(node);
    }
    if (balance < -1) {
      if (balanceOf(node->right) > 0) node->right = rotateRight(node->right);
      return rotateLeft(node);
    }
    return node;
  }

  static Node* insertNode(Node* node, double score, int firstDoc, int secondDoc) {
    if (node == nullptr) return new Node(score, firstDoc, secondDoc);

    if (score < node->score) {
      node->left = insertNode(node->left, score, firstDoc, secondDoc);
    } else {
      node->right = insertNode(node->right, score, firstDoc, secondDoc);
    }
    return rebalance(node);
  }

  static void collectDescending(Node* node, std::vector<Node>& out) {
    if (node == nullptr) return;
    collectDescending(node->right, out);
    out.push_back(*node);
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

// heap
struct ScoreEntry {
  double score;
  int firstDoc;
  int secondDoc;
};

inline bool operator<(const ScoreEntry& a, const ScoreEntry& b) {
  return a.score < b.score;
}

class MaxHeap {
 public:
  MaxHeap() : data_() {}

  void push(const ScoreEntry& entry) {
    data_.push_back(entry);
    std::size_t index = data_.size() - 1;
    while (index > 0) {
      std::size_t parent = (index - 1) / 2;
      if (data_[parent].score >= data_[index].score) break;
      swap(parent, index);
      index = parent;
    }
  }

  ScoreEntry pop() {
    ScoreEntry biggest = data_[0];
    data_[0] = data_[data_.size() - 1];
    data_.pop_back();
    if (!data_.empty()) siftDown(0);
    return biggest;
  }

  bool empty() const { return data_.empty(); }
  std::size_t size() const { return data_.size(); }

  std::vector<ScoreEntry> topK(std::size_t k) {
    std::vector<ScoreEntry> result;
    while (!empty() && result.size() < k) {
      result.push_back(pop());
    }
    return result;
  }

 private:
  void swap(std::size_t i, std::size_t j) {
    ScoreEntry temporary = data_[i];
    data_[i] = data_[j];
    data_[j] = temporary;
  }

  void siftDown(std::size_t index) {
    std::size_t count = data_.size();
    while (true) {
      std::size_t left = 2 * index + 1;
      std::size_t right = 2 * index + 2;
      std::size_t largest = index;
      if (left < count && data_[left].score > data_[largest].score) largest = left;
      if (right < count && data_[right].score > data_[largest].score) largest = right;
      if (largest == index) break;
      swap(index, largest);
      index = largest;
    }
  }

  std::vector<ScoreEntry> data_;
};

#endif
