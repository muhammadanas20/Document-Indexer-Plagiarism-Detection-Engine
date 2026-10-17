// Robustness and determinism tests for file reading, tokenization, and indexing
#include <cstdint>

#include "contract/error_code.hpp"
#include "index/dynamic_buffer.hpp"
#include "index/file_reader.hpp"
#include "index/index_builder.hpp"
#include "index/tokenizer.hpp"
#include "harness.hpp"

PLAG_TEST(missing_file_returns_not_found) {
  plag::DynamicBuffer b;
  CHECK(plag::read_file("/no/such/file", b) == plag::ErrorCode::NotFound);
}

PLAG_TEST(directory_passed_as_file_returns_not_readable) {
  plag::DynamicBuffer b;
  plag::ErrorCode rc = plag::read_file(PLAG_SOURCE_DIR, b);
  CHECK(rc == plag::ErrorCode::NotReadable);
}

PLAG_TEST(arbitrary_high_bytes_are_tokenized_safely) {
  const std::uint8_t raw[8] = {'a', 'b', 0xFF, 0xFE, ' ', 'x', 'y', '\n'};
  plag::DynamicBuffer buf;
  buf.append(raw, 8);

  plag::TokenPool pool;
  plag::TokenizeResult r = plag::tokenize(buf, pool, true);
  CHECK(r.tokens >= 2);
}

PLAG_TEST(indexing_is_deterministic_across_two_runs) {
  const std::uint64_t doc1[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  const std::uint64_t doc2[8] = {3, 4, 5, 6, 9, 10, 11, 12};

  plag::IndexBuilder idxA;
  idxA.add_document(0, "d1.txt", doc1, 8, 4);
  idxA.add_document(1, "d2.txt", doc2, 8, 4);

  plag::IndexBuilder idxB;
  idxB.add_document(0, "d1.txt", doc1, 8, 4);
  idxB.add_document(1, "d2.txt", doc2, 8, 4);

  CHECK_EQ(idxA.stats().files, idxB.stats().files);
  CHECK_EQ(idxA.stats().tokens, idxB.stats().tokens);
  CHECK_EQ(idxA.stats().kgrams, idxB.stats().kgrams);
  CHECK_EQ(idxA.stats().distinct, idxB.stats().distinct);
  CHECK_EQ(idxA.stats().longestChain, idxB.stats().longestChain);
}
