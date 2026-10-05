// json_writer.cpp -- STUB (Alyan, W6).
#include "report/json_writer.hpp"

namespace plag {

bool write_json(std::FILE* /*out*/, const IndexStats& /*stats*/,
                const PairScore* /*pairs*/, std::size_t /*nPairs*/,
                const Span* /*spans*/, std::size_t /*nSpans*/) {
  return false;  // STUB dummy
}

const char* json_escape_into(char* /*buf*/, std::size_t /*bufSize*/,
                             const char* /*s*/) {
  return "";  // STUB dummy
}

}  // namespace plag
