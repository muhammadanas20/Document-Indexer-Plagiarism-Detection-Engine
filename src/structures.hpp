#ifndef PROJECT_STRUCTURES_HPP
#define PROJECT_STRUCTURES_HPP

#include <cstddef>
#include <utility>
#include <vector>

// list
template <typename T>
class LinkedList {
 public:

  struct Node {
    T value;
    Node* next;
    Node(const T& v, Node* n) : value(v), next(n) {}
  };

  LinkedList() : head_(nullptr), tail_(nullptr), size_(0) {}

  ~LinkedList() { clear(); }
  LinkedList(const LinkedList&) = delete;
  LinkedList& operator=(const LinkedList&) = delete;

  void pushFront(const T& value) {
    head_ = new Node(value, head_);
    if (tail_ == nullptr) tail_ = head_;
    ++size_;
  }

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
  Node* head_;
  Node* tail_;
  std::size_t size_;
};

// stack
template <typename T>
class Stack {
 public:
  Stack() : data_(nullptr), capacity_(0), size_(0) {}
  ~Stack() { delete[] data_; }
  Stack(const Stack&) = delete;
  Stack& operator=(const Stack&) = delete;

  void push(const T& value) {
    if (size_ == capacity_) grow();
    data_[size_] = value;
    ++size_;
  }

  T pop() {
    --size_;
    return data_[size_];
  }

  T& top() { return data_[size_ - 1]; }

  bool empty() const { return size_ == 0; }
  std::size_t size() const { return size_; }

 private:

  void grow() {
    std::size_t newCapacity = (capacity_ == 0) ? 4 : capacity_ * 2;
    T* newData = new T[newCapacity];
    for (std::size_t i = 0; i < size_; ++i) newData[i] = data_[i];
    delete[] data_;
    data_ = newData;
    capacity_ = newCapacity;
  }

  T* data_;
  std::size_t capacity_;
  std::size_t size_;
};

// queue
template <typename T>
class Queue {
 public:
  Queue() : data_(nullptr), capacity_(0), front_(0), size_(0) {}
  ~Queue() { delete[] data_; }
  Queue(const Queue&) = delete;
  Queue& operator=(const Queue&) = delete;

  void push(const T& value) {
    if (size_ == capacity_) grow();
    std::size_t back = (front_ + size_) % capacity_;
    data_[back] = value;
    ++size_;
  }

  T pop() {
    T value = data_[front_];
    front_ = (front_ + 1) % capacity_;
    --size_;
    return value;
  }

  T& front() { return data_[front_]; }

  bool empty() const { return size_ == 0; }
  std::size_t size() const { return size_; }

 private:
  void grow() {
    std::size_t newCapacity = (capacity_ == 0) ? 4 : capacity_ * 2;
    T* newData = new T[newCapacity];

    for (std::size_t i = 0; i < size_; ++i) {
      newData[i] = data_[(front_ + i) % capacity_];
    }
    delete[] data_;
    data_ = newData;
    capacity_ = newCapacity;
    front_ = 0;
  }

  T* data_;
  std::size_t capacity_;
  std::size_t front_;
  std::size_t size_;
};

// graph
class Graph {
 public:
  explicit Graph(int vertexCount)
      : adjacency_(nullptr), vertexCount_(vertexCount), edgeCount_(0) {
    adjacency_ = new LinkedList<int>*[vertexCount_];
    for (int i = 0; i < vertexCount_; ++i) adjacency_[i] = new LinkedList<int>();
  }

  ~Graph() {
    for (int i = 0; i < vertexCount_; ++i) delete adjacency_[i];
    delete[] adjacency_;
  }
  Graph(const Graph&) = delete;
  Graph& operator=(const Graph&) = delete;

  void addEdge(int a, int b) {
    adjacency_[a]->pushBack(b);
    adjacency_[b]->pushBack(a);
    ++edgeCount_;
  }

  int degree(int vertex) const {
    return static_cast<int>(adjacency_[vertex]->size());
  }
  int vertexCount() const { return vertexCount_; }
  int edgeCount() const { return edgeCount_; }

  std::vector<int> bfs(int start) const {
    std::vector<int> visited;
    std::vector<bool> seen(static_cast<std::size_t>(vertexCount_), false);

    Queue<int> queue;
    queue.push(start);
    seen[static_cast<std::size_t>(start)] = true;

    while (!queue.empty()) {
      int current = queue.pop();
      visited.push_back(current);

      for (LinkedList<int>::Node* n = adjacency_[current]->head(); n != nullptr;
           n = n->next) {
        int neighbour = n->value;
        if (!seen[static_cast<std::size_t>(neighbour)]) {
          seen[static_cast<std::size_t>(neighbour)] = true;
          queue.push(neighbour);
        }
      }
    }
    return visited;
  }

  std::vector<std::vector<int> > clusters() const {
    std::vector<std::vector<int> > result;
    std::vector<bool> seen(static_cast<std::size_t>(vertexCount_), false);

    for (int start = 0; start < vertexCount_; ++start) {
      if (seen[static_cast<std::size_t>(start)]) continue;
      std::vector<int> cluster = bfs(start);
      for (std::size_t i = 0; i < cluster.size(); ++i) {
        seen[static_cast<std::size_t>(cluster[i])] = true;
      }

      if (cluster.size() > 1) result.push_back(cluster);
    }
    return result;
  }

 private:
  LinkedList<int>** adjacency_;
  int vertexCount_;
  int edgeCount_;
};

#endif
