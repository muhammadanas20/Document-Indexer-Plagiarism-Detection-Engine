// Unit tests for the tokenizer and line number mapping
#include <cstdint>
#include <cstring>
#include <string>

#include "index/dynamic_buffer.hpp"
#include "index/file_reader.hpp"
#include "index/tokenizer.hpp"
#include "harness.hpp"

namespace {

plag::TokenizeResult lex_str(const char* src, plag::TokenPool& pool,
                             bool lower) {
  plag::DynamicBuffer buf;
  std::size_t n = std::strlen(src);
  buf.append(reinterpret_cast<const std::uint8_t*>(src), n);
  return plag::tokenize(buf, pool, lower);
}

}  // namespace

PLAG_TEST(tokenize_simple_cpp_snippet) {
  plag::TokenPool pool;
  plag::TokenizeResult r = lex_str("int main() { return 0; }", pool, true);
  CHECK_EQ(r.tokens, std::size_t{9});
  CHECK(!r.malformed);
}

PLAG_TEST(whitespace_does_not_change_token_ids) {
  plag::TokenPool a;
  plag::TokenPool b;
  plag::TokenizeResult ra = lex_str("a+b;", a, true);
  plag::TokenizeResult rb = lex_str("a + b ;", b, true);

  CHECK_EQ(ra.tokens, std::size_t{4});
  CHECK_EQ(ra.tokens, rb.tokens);
  for (std::size_t i = 0; i < ra.tokens; ++i) {
    CHECK_EQ(a.id[i], b.id[i]);
  }
}

PLAG_TEST(case_normalisation_toggle) {
  plag::TokenPool lowerA;
  plag::TokenPool lowerB;
  lex_str("int x = 1;", lowerA, true);
  lex_str("INT X = 1;", lowerB, true);
  CHECK_EQ(lowerA.id.size(), lowerB.id.size());
  for (std::size_t i = 0; i < lowerA.id.size(); ++i) {
    CHECK_EQ(lowerA.id[i], lowerB.id[i]);
  }

  plag::TokenPool exactA;
  plag::TokenPool exactB;
  lex_str("int x = 1;", exactA, false);
  lex_str("INT X = 1;", exactB, false);
  CHECK(exactA.id[0] != exactB.id[0]);
}

PLAG_TEST(comments_produce_no_tokens) {
  plag::TokenPool pool;
  plag::TokenizeResult r =
      lex_str("// line comment\n/* block comment */\n", pool, true);
  CHECK_EQ(r.tokens, std::size_t{0});
  CHECK(!r.malformed);
}

PLAG_TEST(unterminated_string_sets_malformed_flag) {
  plag::TokenPool pool;
  plag::TokenizeResult r = lex_str("const char* s = \"oops", pool, true);
  CHECK(r.malformed);
  CHECK(r.tokens > 0);
}

PLAG_TEST(line42_fixture_maps_to_line_42) {
  plag::DynamicBuffer buf;
  std::string path = std::string(PLAG_FIXTURE_DIR) + "/anas/line42.cpp";
  CHECK(plag::read_file(path.c_str(), buf) == plag::ErrorCode::Ok);

  plag::TokenPool pool;
  plag::TokenizeResult r = plag::tokenize(buf, pool, true);
  CHECK(r.tokens > 0);
  CHECK_EQ(pool.line[0], 42u);
}
