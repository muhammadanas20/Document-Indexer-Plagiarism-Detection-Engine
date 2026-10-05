// dynamic_buffer.cpp -- STUB (Anas, W1 / Step 2). Bodies are dummies.
#include "index/dynamic_buffer.hpp"

namespace plag {

DynamicBuffer::DynamicBuffer() = default;

DynamicBuffer::~DynamicBuffer() { delete[] d_; }

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

void DynamicBuffer::reserve_at_least(std::size_t /*need*/) {
  // TODO(Anas W1 Step 2): grow to max(need, c_*2) from an initial 4 KiB.
}

void DynamicBuffer::append(const std::uint8_t* /*src*/, std::size_t /*n*/) {
  // TODO(Anas W1 Step 2): reserve, memcpy, bump n_.
}

void DynamicBuffer::trim_trailing_cr() {
  // TODO(Anas W1 Step 2): drop a single trailing '\r'.
}

void DynamicBuffer::clear() { n_ = 0; }

std::size_t DynamicBuffer::size() const { return n_; }
bool DynamicBuffer::empty() const { return n_ == 0; }
std::size_t DynamicBuffer::capacity() const { return c_; }
const std::uint8_t* DynamicBuffer::data() const { return d_; }
std::uint8_t* DynamicBuffer::data() { return d_; }

}  // namespace plag
