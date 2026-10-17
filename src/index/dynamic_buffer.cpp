// Implementation of DynamicBuffer
#include "index/dynamic_buffer.hpp"

#include <cstring>

namespace plag {

DynamicBuffer::DynamicBuffer() = default;

DynamicBuffer::~DynamicBuffer() {
  delete[] d_;
}

DynamicBuffer::DynamicBuffer(DynamicBuffer&& other) noexcept
    : d_(other.d_), n_(other.n_), c_(other.c_) {
  other.d_ = nullptr;
  other.n_ = 0;
  other.c_ = 0;
}

DynamicBuffer& DynamicBuffer::operator=(DynamicBuffer&& other) noexcept {
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

void DynamicBuffer::reserve_at_least(std::size_t need) {
  if (need <= c_) return;
  std::size_t new_cap = (c_ == 0) ? 4096 : c_;
  while (new_cap < need) {
    new_cap *= 2;
  }
  std::uint8_t* new_buf = new std::uint8_t[new_cap];
  if (n_ > 0 && d_ != nullptr) {
    std::memcpy(new_buf, d_, n_);
  }
  delete[] d_;
  d_ = new_buf;
  c_ = new_cap;
}

void DynamicBuffer::append(const std::uint8_t* src, std::size_t n) {
  if (src == nullptr || n == 0) return;
  reserve_at_least(n_ + n);
  std::memcpy(d_ + n_, src, n);
  n_ += n;
}

void DynamicBuffer::trim_trailing_cr() {
  if (d_ == nullptr || n_ == 0) return;
  std::size_t write_pos = 0;
  for (std::size_t read_pos = 0; read_pos < n_; ++read_pos) {
    if (d_[read_pos] != static_cast<std::uint8_t>('\r')) {
      d_[write_pos++] = d_[read_pos];
    }
  }
  n_ = write_pos;
}

void DynamicBuffer::clear() {
  n_ = 0;
}

std::size_t DynamicBuffer::size() const { return n_; }
bool DynamicBuffer::empty() const { return n_ == 0; }
std::size_t DynamicBuffer::capacity() const { return c_; }
const std::uint8_t* DynamicBuffer::data() const { return d_; }
std::uint8_t* DynamicBuffer::data() { return d_; }

}  // namespace plag
