// list.hpp  --  Singly linked list  (Week 3 topic: List / Singly Linked List)
// Owner: Anas
//
// Where it is used in the project:
//   1. Inside the hash table: every bucket is a linked list (separate chaining).
//   2. Inside the graph: every node keeps a linked list of its neighbours.
//   3. Inside the index: the posting list of one word is a linked list of
//      (document, line, count) records.
//
// We do NOT use std::list / std::vector for this, because this is the data
// structure we are supposed to build and explain ourselves.

#ifndef DIP_LIST_HPP
#define DIP_LIST_HPP

#include <cstddef>

namespace dip {

template <typename T>
class LinkedList {
 public:
  // One node = one value + a pointer to the next node.
  struct Node {
    T value;
    Node* next;
    Node(const T& v, Node* n) : value(v), next(n) {}
  };

  LinkedList() : head_(nullptr), tail_(nullptr), size_(0) {}

  // Rule of Three: we free every node in the destructor.
  ~LinkedList() { clear(); }
  LinkedList(const LinkedList&) = delete;             // never copied by accident
  LinkedList& operator=(const LinkedList&) = delete;  // (copying would alias nodes)

  // Insert at the front. O(1).
  void pushFront(const T& value) {
    head_ = new Node(value, head_);
    if (tail_ == nullptr) tail_ = head_;
    ++size_;
  }

  // Insert at the end. O(1) because we keep a tail pointer.
  void pushBack(const T& value) {
    Node* node = new Node(value, nullptr);
    if (tail_ == nullptr) {
      head_ = node;
      tail_ = node;
    } else {
      tail_->next = node;
      tail_ = node;
    }
    ++size_;
  }

  // Search + remove the first node that holds `value`. O(n).
  // The hash table needs this for delete-by-key (chaining).
  bool remove(const T& value) {
    Node* previous = nullptr;
    Node* current = head_;
    while (current != nullptr) {
      if (current->value == value) {
        if (previous == nullptr) {
          head_ = current->next;
        } else {
          previous->next = current->next;
        }
        if (current == tail_) tail_ = previous;
        delete current;
        --size_;
        return true;
      }
      previous = current;
      current = current->next;
    }
    return false;
  }

  // Delete every node. O(n).
  void clear() {
    Node* current = head_;
    while (current != nullptr) {
      Node* next = current->next;
      delete current;
      current = next;
    }
    head_ = nullptr;
    tail_ = nullptr;
    size_ = 0;
  }

  Node* head() const { return head_; }
  std::size_t size() const { return size_; }
  bool empty() const { return size_ == 0; }

 private:
  Node* head_;  // first node
  Node* tail_;  // last node (so pushBack is O(1))
  std::size_t size_;
};

}  // namespace dip

#endif  // DIP_LIST_HPP
