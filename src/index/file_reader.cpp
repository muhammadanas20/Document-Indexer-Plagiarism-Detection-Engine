// file_reader.cpp -- STUB (Anas, W1 / Step 2).
#include "index/file_reader.hpp"

namespace plag {

ErrorCode read_file(const char* /*path*/, DynamicBuffer& /*out*/) {
  // TODO(Anas W1 Step 2): ifstream binary|ate, tellg() size, reserve_at_least,
  // read() in 64 KiB chunks, trim_trailing_cr(), return Ok.
  return ErrorCode::NotFound;  // STUB dummy: nothing is read yet.
}

}  // namespace plag
