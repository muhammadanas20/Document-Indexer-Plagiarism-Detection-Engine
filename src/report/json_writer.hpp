// Writes detection statistics, pairs, and spans in JSON format
#ifndef PLAG_SRC_REPORT_JSON_WRITER_HPP_
#define PLAG_SRC_REPORT_JSON_WRITER_HPP_

#include <cstddef>
#include <cstdio>

#include "common/dyn_array.hpp"
#include "contract/contract.hpp"

namespace plag {

bool write_json(std::FILE* out, const IndexStats& stats,
                const PairScore* pairs, std::size_t nPairs,
                const Span* spans, std::size_t nSpans);

const char* json_escape_into(char* buf, std::size_t bufSize, const char* s);

}  // namespace plag

#endif  // PLAG_SRC_REPORT_JSON_WRITER_HPP_
