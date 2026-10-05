// dyn_array.hpp -- our own growable array. No std::vector in engine code.
// Owner: Anas (shared, FROZEN after Week 1). Alyan: read and use it; if you
// need a method that is not here, add src/rank/dyn_ext.hpp in your own area.
//
// WORKING and TESTED (Week 0 homework, tests/test_anas_common.cpp).
// Geometric growth makes push_back O(1) amortised: after n pushes the total
// copying is 1 + 2 + 4 + ... < 2n, so the average cost per push stays constant.
#ifndef PLAG_SRC_COMMON_DYN_ARRAY_HPP_
#define PLAG_SRC_COMMON_DYN_ARRAY_HPP_

#include <cassert>
#include <cstddef>
#include <utility>

namespace plag {

// Move-only: a container holding a corpus-sized buffer must not be copied by
// accident. Copying is deleted, not merely expensive.
template <class T>
class DynArray {
 public:
  DynArray() = default;
  explicit DynArray(std::size_t n) { reserve(n); }
  ~DynArray() { delete[] d_; }

  DynArray(DynArray&& o) noexcept
      : d_(o.d_), n_(o.n_), c_(o.c_) {
    o.d_ = nullptr;
    o.n_ = 0;
    o.c_ = 0;
  }

  DynArray& operator=(DynArray&& o) noexcept {
    if (this != &o) {
      delete[] d_;
      d_ = o.d_;
      n_ = o.n_;
      c_ = o.c_;
      o.d_ = nullptr;
      o.n_ = 0;
      o.c_ = 0;
    }
    return *this;
  }

  DynArray(const DynArray&) = delete;
  DynArray& operator=(const DynArray&) = delete;

  void reserve(std::size_t need) {
    if (need <= c_) return;
    T* p = new T[need];
    for (std::size_t i = 0; i < n_; ++i) p[i] = std::move(d_[i]);
    delete[] d_;
    d_ = p;
    c_ = need;
  }

  void push_back(const T& v) {
    if (n_ == c_) grow(n_ + 1);
    d_[n_++] = v;
  }

  void push_back(T&& v) {
    if (n_ == c_) grow(n_ + 1);
    d_[n_++] = std::move(v);
  }

  void pop_back() {
    assert(n_ > 0);
    --n_;
  }

  void clear() { n_ = 0; }

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
  // c_ = max(need, c_*2) -- doubling is what keeps the amortised cost O(1).
  void grow(std::size_t need) { reserve(need > c_ * 2 ? need : c_ * 2); }

  T* d_ = nullptr;
  std::size_t n_ = 0;
  std::size_t c_ = 0;
};

}  // namespace plag

#endif  // PLAG_SRC_COMMON_DYN_ARRAY_HPP_
