// Merges overlapping or adjacent token hits into maximal matching spans
#ifndef PLAG_SRC_INDEX_SPAN_MERGE_HPP_
#define PLAG_SRC_INDEX_SPAN_MERGE_HPP_

#include <cstddef>

#include "common/dyn_array.hpp"
#include "contract/span.hpp"

namespace plag {

std::size_t merge_spans(const Span* hits, std::size_t n, std::size_t gap,
                        DynArray<Span>& out);

}  // namespace plag

#endif  // PLAG_SRC_INDEX_SPAN_MERGE_HPP_
