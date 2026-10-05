// dynamic_buffer.hpp -- growable byte buffer for file contents.
// Owner: Anas.  src/index/ is Anas's area; do not edit outside your own paths.
// STUB (W1 / Step 2): final signatures, dummy bodies. Replace when Step 2 lands.
#ifndef PLAG_SRC_INDEX_DYNAMIC_BUFFER_HPP_
#define PLAG_SRC_INDEX_DYNAMIC_BUFFER_HPP_

#include <cstddef>
#include <cstdint>

namespace plag {

// Owns a raw uint8_t* buffer. Move-only on purpose: copying a 5 MB corpus
// buffer by accident is the bug this class exists to prevent.
class DynamicBuffer {
 public:
  DynamicBuffer();
  ~DynamicBuffer();

  DynamicBuffer(DynamicBuffer&& other) noexcept;
  DynamicBuffer& operator=(DynamicBuffer&& other) noexcept;
  DynamicBuffer(const DynamicBuffer&) = delete;
  DynamicBuffer& operator=(const DynamicBuffer&) = delete;

  void reserve_at_least(std::size_t need);
  void append(const std::uint8_t* src, std::size_t n);
  void trim_trailing_cr();          // CRLF -> LF for Windows-authored corpora
  void clear();

  std::size_t size() const;
  bool empty() const;
  std::size_t capacity() const;
  const std::uint8_t* data() const;
  std::uint8_t* data();

 private:
  std::uint8_t* d_ = nullptr;
  std::size_t n_ = 0;
  std::size_t c_ = 0;
};

}  // namespace plag

#endif  // PLAG_SRC_INDEX_DYNAMIC_BUFFER_HPP_
