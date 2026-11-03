// queue.hpp  --  Queue (FIFO)  (Week 9 topic: Queue and its applications)
// Owner: Alyan
//
// Where it is used in the project:
//   - Breadth-First Search in graph.hpp (the BFS queue).
//
// It is a circular array: when the back reaches the end of the array it wraps
// around to index 0, so we never have to shift the elements.

#ifndef DIP_QUEUE_HPP
#define DIP_QUEUE_HPP

#include <cstddef>

namespace dip {

template <typename T>
class Queue {
 public:
  Queue() : data_(nullptr), capacity_(0), front_(0), size_(0) {}
  ~Queue() { delete[] data_; }
  Queue(const Queue&) = delete;
  Queue& operator=(const Queue&) = delete;

  // Add at the back. O(1) (O(n) only when we grow).
  void push(const T& value) {
    if (size_ == capacity_) grow();
    std::size_t back = (front_ + size_) % capacity_;
    data_[back] = value;
    ++size_;
  }

  // Remove from the front and return it. O(1).
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
    // Copy the elements in queue order, starting from the new front = 0.
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

}  // namespace dip

#endif  // DIP_QUEUE_HPP
