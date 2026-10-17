// Tokenizer that splits file bytes into tokens and computes token IDs
#ifndef PLAG_SRC_INDEX_TOKENIZER_HPP_
#define PLAG_SRC_INDEX_TOKENIZER_HPP_

#include <cstddef>
#include <cstdint>

#include "common/dyn_array.hpp"
#include "index/dynamic_buffer.hpp"

namespace plag {

struct TokenPool {
  DynArray<std::uint32_t> off;
  DynArray<std::uint32_t> len;
  DynArray<std::uint64_t> id;
  DynArray<std::uint32_t> line;
};

struct TokenizeResult {
  bool malformed = false;
  std::size_t tokens = 0;
};

TokenizeResult tokenize(const DynamicBuffer& src, TokenPool& pool,
                        bool lower_case_ids);

}  // namespace plag

#endif  // PLAG_SRC_INDEX_TOKENIZER_HPP_
