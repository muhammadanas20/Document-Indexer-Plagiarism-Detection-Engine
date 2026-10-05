// test_anas_stress.cpp -- hardening: huge corpora, weird bytes, tight memory.
// Owner: Anas.  Step 8 (Week 6).
//
// STUB: every robustness case must assert a documented ErrorCode, never a
// crash. Determinism is the claim: the stress binary runs twice with identical
// output.
#include <cstdint>

#include "contract/error_code.hpp"
#include "index/dynamic_buffer.hpp"
#include "index/file_reader.hpp"
#include "harness.hpp"

PLAG_TEST(missing_file_returns_a_documented_error_code) {
  plag::DynamicBuffer b;
  CHECK(plag::read_file("/no/such/file", b) == plag::ErrorCode::NotFound);
}

PLAG_TEST(malformed_utf8_is_tokenised_not_rejected) {
  // TODO(Anas W6 Step 8): invalid UTF-8 bytes are ordinary bytes to us; the
  // tokenizer must not reject the file.
  PLAG_STUB_NOTE("Step 8 -- malformed UTF-8");
  CHECK(true);
}

PLAG_TEST(a_directory_passed_as_a_file_is_an_error_not_a_crash) {
  // TODO(Anas W6 Step 8): assert a documented ErrorCode.
  plag::DynamicBuffer b;
  const plag::ErrorCode rc = plag::read_file(PLAG_SOURCE_DIR, b);
  CHECK(rc == plag::ErrorCode::NotFound);   // STUB: reader not implemented
  PLAG_STUB_NOTE("Step 8 -- directory-as-file");
}

PLAG_TEST(indexing_is_deterministic_across_two_runs) {
  // The 1 GB synthetic corpus (Alyan's generator, --files 10000 --seed 11)
  // indexed twice must give byte-identical --stats output.
  // TODO(Anas W6 Step 8): run tools/anas_bench_hash twice and diff.
  PLAG_STUB_NOTE("Step 8 -- determinism");
  CHECK(true);
}
