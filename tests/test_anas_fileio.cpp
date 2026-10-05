// test_anas_fileio.cpp -- DynamicBuffer, file_reader, dir_walker.
// Owner: Anas.  Step 2 (Week 1). Fixtures live in tests/fixtures/anas/.
//
// STUB: reader and walker are not implemented. The missing-file case below is
// already the real expected behaviour (NotFound); the success paths are marked
// TODO and land with Step 2.
#include <cstdint>
#include <string>

#include "contract/error_code.hpp"
#include "index/dir_walker.hpp"
#include "index/dynamic_buffer.hpp"
#include "index/file_reader.hpp"
#include "harness.hpp"

PLAG_TEST(dynamic_buffer_starts_empty_and_moves_cleanly) {
  plag::DynamicBuffer b;
  CHECK_EQ(b.size(), std::size_t{0});
  CHECK(b.empty());

  plag::DynamicBuffer moved(std::move(b));
  CHECK_EQ(moved.size(), std::size_t{0});
  CHECK_EQ(b.size(), std::size_t{0});
}

PLAG_TEST(read_file_reports_not_found_for_a_missing_file) {
  plag::DynamicBuffer b;
  const plag::ErrorCode rc = plag::read_file("/definitely/not/here/nope.txt", b);
  CHECK(rc == plag::ErrorCode::NotFound);
  CHECK_EQ(b.size(), std::size_t{0});
}

PLAG_TEST(read_file_reads_a_fixture_byte_for_byte) {
  // TODO(Anas W1 Step 2): bytes read == stat -c%s minus the \r count, for every
  // fixture in tests/fixtures/anas/.
  plag::DynamicBuffer b;
  const std::string path = std::string(PLAG_FIXTURE_DIR) + "/anas/simple.txt";
  const plag::ErrorCode rc = plag::read_file(path.c_str(), b);
  // STUB: the reader is not implemented, so nothing is read yet.
  CHECK(rc == plag::ErrorCode::NotFound);
  CHECK_EQ(b.size(), std::size_t{0});
  PLAG_STUB_NOTE("Step 2 -- golden byte-count test per fixture");
}

PLAG_TEST(walk_dir_lists_indexable_files_in_sorted_order) {
  // TODO(Anas W1 Step 2): output must equal `find . -type f | sort` filtered by
  // the extension whitelist, and be identical across two consecutive runs.
  plag::DynArray<char*> paths;
  const std::string root = std::string(PLAG_CORPUS_DIR) + "/anas/small";
  const plag::ErrorCode rc = plag::walk_dir(root.c_str(), paths);
  CHECK(rc == plag::ErrorCode::NotFound);   // STUB: nothing walked yet
  CHECK_EQ(paths.size(), std::size_t{0});
  PLAG_STUB_NOTE("Step 2 -- walker reproducibility test");
}
