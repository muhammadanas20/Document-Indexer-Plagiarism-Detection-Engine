// Simple dynamic array template class (replaces std::vector in engine code)
#ifndef PLAG_SRC_COMMON_DYN_ARRAY_HPP_
#define PLAG_SRC_COMMON_DYN_ARRAY_HPP_

#include <cassert>
#include <cstddef>
#include <utility>

namespace plag {

template <typename T>
class DynArray {
 public:
  DynArray() : d_(nullptr), n_(0), c_(0) {}

  explicit DynArray(std::size_t initial_cap) : d_(nullptr), n_(0), c_(0) {
    reserve(initial_cap);
  }

  ~DynArray() {
    delete[] d_;
  }

  // Copy constructor
  DynArray(const DynArray& other) : d_(nullptr), n_(0), c_(0) {
    if (other.n_ > 0) {
      reserve(other.n_);
      for (std::size_t i = 0; i < other.n_; ++i) {
        d_[i] = other.d_[i];
      }
      n_ = other.n_;
    }
  }

  // Copy assignment
  DynArray& operator=(const DynArray& other) {
    if (this != &other) {
      clear();
      if (other.n_ > 0) {
        reserve(other.n_);
        for (std::size_t i = 0; i < other.n_; ++i) {
          d_[i] = other.d_[i];
        }
        n_ = other.n_;
      }
    }
    return *this;
  }

  // Move constructor
  DynArray(DynArray&& other) noexcept
      : d_(other.d_), n_(other.n_), c_(other.c_) {
    other.d_ = nullptr;
    other.n_ = 0;
    other.c_ = 0;
  }

  // Move assignment
  DynArray& operator=(DynArray&& other) noexcept {
    if (this != &other) {
      delete[] d_;
      d_ = other.d_;
      n_ = other.n_;
      c_ = other.c_;
      other.d_ = nullptr;
      other.n_ = 0;
      other.c_ = 0;
    }
    return *this;
  }

  void reserve(std::size_t need) {
    if (need <= c_) return;
    T* new_data = new T[need];
    for (std::size_t i = 0; i < n_; ++i) {
      new_data[i] = std::move(d_[i]);
    }
    delete[] d_;
    d_ = new_data;
    c_ = need;
  }

  void push_back(const T& val) {
    if (n_ == c_) {
      grow(n_ + 1);
    }
    d_[n_++] = val;
  }

  void push_back(T&& val) {
    if (n_ == c_) {
      grow(n_ + 1);
    }
    d_[n_++] = std::move(val);
  }

  void pop_back() {
    assert(n_ > 0);
    --n_;
  }

  void clear() {
    n_ = 0;
  }

  std::size_t size() const { return n_; }
  std::size_t capacity() const { return c_; }
  bool empty() const { return n_ == 0; }

  T& operator[](std::size_t i) {
    assert(i < n_);
    return d_[i];
  }

  const T& operator[](std::size_t i) const {
    assert(i < n_);
    return d_[i];
  }

  T& back() {
    assert(n_ > 0);
    return d_[n_ - 1];
  }

  const T& back() const {
    assert(n_ > 0);
    return d_[n_ - 1];
  }

  T* data() { return d_; }
  const T* data() const { return d_; }

  T* begin() { return d_; }
  T* end() { return d_ + n_; }
  const T* begin() const { return d_; }
  const T* end() const { return d_ + n_; }

 private:
  void grow(std::size_t min_cap) {
    std::size_t next = (c_ == 0) ? 8 : c_ * 2;
    while (next < min_cap) {
      next *= 2;
    }
    reserve(next);
  }

  T* d_;
  std::size_t n_;
  std::size_t c_;
};

}  // namespace plag

#endif  // PLAG_SRC_COMMON_DYN_ARRAY_HPP_
