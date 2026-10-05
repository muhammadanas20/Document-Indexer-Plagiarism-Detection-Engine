// tokenizer.hpp -- bytes -> token list, in exact and normalised views.
// Owner: Anas.  src/index/ is Anas's area.
// STUB (W2 / Step 3): dummy body emits 0 tokens. Replace when Step 3 lands.
#ifndef PLAG_SRC_INDEX_TOKENIZER_HPP_
#define PLAG_SRC_INDEX_TOKENIZER_HPP_

#include <cstddef>
#include <cstdint>

#include "common/dyn_array.hpp"
#include "index/dynamic_buffer.hpp"

namespace plag {

// Every token is stored once as {offset,len} into the file bytes; the exact
// and normalised views are slices of that single allocation.
struct TokenPool {
  DynArray<std::uint32_t> off;    // byte offset in the source buffer
  DynArray<std::uint32_t> len;    // lexeme length
  DynArray<std::uint64_t> id;     // FNV-1a of the lexeme (the roller wants ids)
  DynArray<std::uint32_t> line;   // token -> 1-based line, for the report
};

struct TokenizeResult {
  bool malformed = false;         // unbalanced quote / unterminated comment
  std::size_t tokens = 0;
};

// Single pass. Comments produce nothing (not whitespace tokens).
TokenizeResult tokenize(const DynamicBuffer& src, TokenPool& pool,
                        bool lower_case_ids);

}  // namespace plag

#endif  // PLAG_SRC_INDEX_TOKENIZER_HPP_
