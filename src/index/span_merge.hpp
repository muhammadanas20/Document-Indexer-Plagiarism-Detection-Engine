// span_merge.hpp -- scattered k-gram hits -> disjoint maximal copied spans.
// Owner: Anas.  src/index/ is Anas's area.
// STUB (W5 / Step 7c): dummy body emits no spans.
#ifndef PLAG_SRC_INDEX_SPAN_MERGE_HPP_
#define PLAG_SRC_INDEX_SPAN_MERGE_HPP_

#include <cstddef>

#include "common/dyn_array.hpp"
#include "contract/span.hpp"

namespace plag {

// Sort by (docA, startA), then extend the current span while the next hit
// continues it in BOTH files at the same offset -- a different offset in B
// starts a new span. Output is disjoint and sorted.
std::size_t merge_spans(const Span* hits, std::size_t n, std::size_t gap,
                        DynArray<Span>& out);

}  // namespace plag

#endif  // PLAG_SRC_INDEX_SPAN_MERGE_HPP_
