// file_reader.hpp -- read one file into a DynamicBuffer, never throwing.
// Owner: Anas.  src/index/ is Anas's area.
// STUB (W1 / Step 2): dummy body returns NotFound. Replace when Step 2 lands.
#ifndef PLAG_SRC_INDEX_FILE_READER_HPP_
#define PLAG_SRC_INDEX_FILE_READER_HPP_

#include "contract/error_code.hpp"
#include "index/dynamic_buffer.hpp"

namespace plag {

// Binary read in 64 KiB chunks with gcount() checked. Returns an ErrorCode
// instead of throwing: a corpus of 10 000 files always contains a file you
// cannot read, and one bad file must not kill the run.
ErrorCode read_file(const char* path, DynamicBuffer& out);

}  // namespace plag

#endif  // PLAG_SRC_INDEX_FILE_READER_HPP_
