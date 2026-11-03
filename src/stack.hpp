// stack.hpp  --  Stack (LIFO)  (Week 9 topic: Stack and its applications)
// Owner: Anas
//
// Where it is used in the project:
//   The folder walker in loader.cpp. Instead of calling itself recursively for
//   every sub-folder (which could overflow the call stack on a deep folder),
//   it keeps the folders it still has to visit on a stack and pops them one by
//   one (iterative depth-first search).

#ifndef DIP_STACK_HPP
#define DIP_STACK_HPP

#include <cstddef>

namespace dip {

template <typename T>
class Stack {
 public:
  Stack() : data_(nullptr), capacity_(0), size_(0) {}
  ~Stack() { delete[] data_; }
  Stack(const Stack&) = delete;
  Stack& operator=(const Stack&) = delete;

  // Put an item on top of the stack. O(1) (O(n) only when we have to grow).
  void push(const T& value) {
    if (size_ == capacity_) grow();
    data_[size_] = value;
    ++size_;
  }

  // Remove the item on top and return it. O(1).
  T pop() {
    --size_;
    return data_[size_];
  }

  // Look at the top item without removing it. O(1).
  T& top() { return data_[size_ - 1]; }

  bool empty() const { return size_ == 0; }
  std::size_t size() const { return size_; }

 private:
  // Double the array when it is full (start with 4 slots).
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

}  // namespace dip

#endif  // DIP_STACK_HPP
