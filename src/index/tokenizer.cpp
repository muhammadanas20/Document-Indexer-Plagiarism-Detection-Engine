// tokenizer.cpp -- STUB (Anas, W2 / Step 3).
#include "index/tokenizer.hpp"

namespace plag {

TokenizeResult tokenize(const DynamicBuffer& /*src*/, TokenPool& /*pool*/,
                        bool /*lower_case_ids*/) {
  // TODO(Anas W2 Step 3): one loop, one index -- skip whitespace; identifier
  // on letter/underscore; number on digit; quoted string with backslash
  // escapes; operators longest-first; // and /* */ comments produce nothing.
  return TokenizeResult{};  // STUB dummy: 0 tokens, not malformed.
}

}  // namespace plag
