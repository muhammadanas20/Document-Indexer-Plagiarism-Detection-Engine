// Unit tests for DynamicBuffer, file_reader, and dir_walker
#include <cstdint>
#include <cstring>
#include <string>

#include "contract/error_code.hpp"
#include "index/dir_walker.hpp"
#include "index/dynamic_buffer.hpp"
#include "index/file_reader.hpp"
#include "harness.hpp"

PLAG_TEST(dynamic_buffer_append_and_trim_cr) {
  plag::DynamicBuffer buf;
  CHECK(buf.empty());

  const char* text = "line1\r\nline2\r\n";
  buf.append(reinterpret_cast<const std::uint8_t*>(text), std::strlen(text));
  CHECK_EQ(buf.size(), std::size_t{14});

  buf.trim_trailing_cr();
  CHECK_EQ(buf.size(), std::size_t{12});
  CHECK(std::memcmp(buf.data(), "line1\nline2\n", 12) == 0);
}

PLAG_TEST(read_file_missing_and_empty) {
  plag::DynamicBuffer buf;
  CHECK(plag::read_file("/nonexistent/file/path.txt", buf) ==
        plag::ErrorCode::NotFound);

  std::string empty_path = std::string(PLAG_FIXTURE_DIR) + "/anas/empty.txt";
  CHECK(plag::read_file(empty_path.c_str(), buf) ==
        plag::ErrorCode::EmptyFile);
}

PLAG_TEST(read_file_crlf_matches_simple_fixture) {
  plag::DynamicBuffer simple_buf;
  plag::DynamicBuffer crlf_buf;

  std::string p1 = std::string(PLAG_FIXTURE_DIR) + "/anas/simple.txt";
  std::string p2 = std::string(PLAG_FIXTURE_DIR) + "/anas/crlf.txt";

  CHECK(plag::read_file(p1.c_str(), simple_buf) == plag::ErrorCode::Ok);
  CHECK(plag::read_file(p2.c_str(), crlf_buf) == plag::ErrorCode::Ok);

  CHECK_EQ(simple_buf.size(), crlf_buf.size());
  CHECK(std::memcmp(simple_buf.data(), crlf_buf.data(), simple_buf.size()) == 0);
}

PLAG_TEST(walk_dir_lists_files_in_sorted_order) {
  plag::DynArray<char*> pathsA;
  plag::DynArray<char*> pathsB;
  std::string root = std::string(PLAG_CORPUS_DIR) + "/anas/small";

  CHECK(plag::walk_dir(root.c_str(), pathsA) == plag::ErrorCode::Ok);
  CHECK(plag::walk_dir(root.c_str(), pathsB) == plag::ErrorCode::Ok);
  CHECK_EQ(pathsA.size(), std::size_t{6});
  CHECK_EQ(pathsA.size(), pathsB.size());

  for (std::size_t i = 0; i < pathsA.size(); ++i) {
    CHECK_EQ(std::string(pathsA[i]), std::string(pathsB[i]));
    if (i > 0) {
      CHECK(std::strcmp(pathsA[i - 1], pathsA[i]) < 0);
    }
  }

  plag::free_walk_paths(pathsA);
  plag::free_walk_paths(pathsB);
}
