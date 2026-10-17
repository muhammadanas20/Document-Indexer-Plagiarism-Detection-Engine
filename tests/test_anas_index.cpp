// Unit tests for IndexBuilder
#include <cstdint>
#include <string>

#include "index/index_builder.hpp"
#include "index/rolling_hash.hpp"
#include "harness.hpp"

PLAG_TEST(lookup_on_empty_index) {
  plag::IndexBuilder idx;
  std::size_t count = 999;
  const plag::Occurrence* occ = idx.lookup(12345ULL, &count);
  CHECK(occ == nullptr);
  CHECK_EQ(count, std::size_t{0});
}

PLAG_TEST(stats_start_at_zero) {
  plag::IndexBuilder idx;
  const plag::IndexStats& st = idx.stats();
  CHECK_EQ(st.files, std::size_t{0});
  CHECK_EQ(st.tokens, std::size_t{0});
  CHECK_EQ(st.kgrams, std::size_t{0});
  CHECK_EQ(st.distinct, std::size_t{0});
}

PLAG_TEST(identical_documents_share_every_kgram) {
  plag::IndexBuilder idx;
  const std::uint64_t ids[6] = {10, 20, 30, 40, 50, 60};
  idx.add_document(0, "a.txt", ids, 6, 4);
  idx.add_document(1, "b.txt", ids, 6, 4);

  CHECK_EQ(idx.stats().files, std::size_t{2});
  CHECK_EQ(idx.stats().tokens, std::size_t{12});
  CHECK_EQ(idx.stats().kgrams, std::size_t{6});
  CHECK_EQ(idx.stats().distinct, std::size_t{3});

  plag::RollingHash rh;
  rh.init(ids, 4, plag::RollingHash::random_base(),
          plag::RollingHash::default_modulus());

  std::size_t count = 0;
  const plag::Occurrence* occ = idx.lookup(rh.hash(), &count);
  CHECK(occ != nullptr);
  CHECK_EQ(count, std::size_t{2});
}

PLAG_TEST(document_view_retains_tokens_and_lines) {
  plag::IndexBuilder idx;
  const std::uint64_t ids[4] = {1, 2, 3, 4};
  idx.add_document(0, "doc0.txt", ids, 4, 2);

  const plag::DocumentView& dv = idx.doc(0);
  CHECK_EQ(dv.norm.size(), std::size_t{4});
  CHECK_EQ(dv.posToLine.size(), std::size_t{4});
  CHECK_EQ(std::string(dv.path), "doc0.txt");
}
