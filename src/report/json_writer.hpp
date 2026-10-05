// json_writer.hpp -- --format=json output.
// Owner: ALYAN.  src/report/ is Alyan's area. Lowest-priority scope cut if the
// project runs late (see plan section 10).
// STUB (Alyan, W6): dummy bodies.
#ifndef PLAG_SRC_REPORT_JSON_WRITER_HPP_
#define PLAG_SRC_REPORT_JSON_WRITER_HPP_

#include <cstddef>
#include <cstdio>

#include "common/dyn_array.hpp"
#include "contract/contract.hpp"

namespace plag {

// Hand-rolled: escaping is the only real work, and no STL container may be
// used in engine code.
bool write_json(std::FILE* out, const IndexStats& stats,
                const PairScore* pairs, std::size_t nPairs,
                const Span* spans, std::size_t nSpans);

const char* json_escape_into(char* buf, std::size_t bufSize, const char* s);

}  // namespace plag

#endif  // PLAG_SRC_REPORT_JSON_WRITER_HPP_
