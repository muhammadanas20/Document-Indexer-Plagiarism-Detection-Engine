// Growable byte buffer for storing raw file contents
#ifndef PLAG_SRC_INDEX_DYNAMIC_BUFFER_HPP_
#define PLAG_SRC_INDEX_DYNAMIC_BUFFER_HPP_

#include <cstddef>
#include <cstdint>

namespace plag {

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
  void trim_trailing_cr();
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
