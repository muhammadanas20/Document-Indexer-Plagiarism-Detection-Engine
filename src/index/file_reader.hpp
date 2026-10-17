// Read a file from disk into a DynamicBuffer
#ifndef PLAG_SRC_INDEX_FILE_READER_HPP_
#define PLAG_SRC_INDEX_FILE_READER_HPP_

#include "contract/error_code.hpp"
#include "index/dynamic_buffer.hpp"

namespace plag {

ErrorCode read_file(const char* path, DynamicBuffer& out);

}  // namespace plag

#endif  // PLAG_SRC_INDEX_FILE_READER_HPP_
