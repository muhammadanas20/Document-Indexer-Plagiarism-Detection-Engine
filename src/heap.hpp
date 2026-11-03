// heap.hpp  --  Max Heap / Priority Queue  (Week 13 topic: Priority Queues, Heaps)
// Owner: Alyan
//
// Where it is used in the project:
//   "Show me the TOP-K most similar document pairs". We push every pair into
//   the heap and pop the K biggest scores. We do not need to sort all pairs,
//   and that is exactly what a priority queue is good for.
//
// A heap is a complete binary tree stored in an array:
//   parent of i = (i-1)/2,  left child = 2*i+1,  right child = 2*i+2
//   and every parent is >= both of its children (max heap).

#ifndef DIP_HEAP_HPP
#define DIP_HEAP_HPP

#include <cstddef>
#include <vector>

namespace dip {

// One entry of the "most suspicious pairs" list.
struct ScoreEntry {
  double score;   // similarity score, bigger = more suspicious
  int firstDoc;   // index of the first document
  int secondDoc;  // index of the second document
};

inline bool operator<(const ScoreEntry& a, const ScoreEntry& b) {
  return a.score < b.score;
}

class MaxHeap {
 public:
  MaxHeap() : data_() {}

  // Insert. O(log n): put the item at the end, then bubble it up.
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

  // Remove and return the biggest item. O(log n): move the last item to the
  // root, then push it down until the heap property holds again.
  ScoreEntry pop() {
    ScoreEntry biggest = data_[0];
    data_[0] = data_[data_.size() - 1];
    data_.pop_back();
    if (!data_.empty()) siftDown(0);
    return biggest;
  }

  bool empty() const { return data_.empty(); }
  std::size_t size() const { return data_.size(); }

  // The K biggest entries, already in decreasing order.
  // O(k log n) instead of O(n log n) for a full sort.
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

}  // namespace dip

#endif  // DIP_HEAP_HPP
