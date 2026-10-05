// test_anas_tokenizer.cpp -- tokens, normalisation, and the token -> line map.
// Owner: Anas.  Step 3 (Week 2).
//
// The expectations below are the ones Alyan's 20 fixtures in
// tests/fixtures/alyan/ encode; they are written down here first so the tests
// land before the implementation and can be watched turning green.
//
// STUB: the tokenizer emits 0 tokens today.
#include <cstdint>

#include "index/dynamic_buffer.hpp"
#include "index/tokenizer.hpp"
#include "harness.hpp"

namespace {
plag::TokenizeResult lex(const char* src, plag::TokenPool& pool, bool lower) {
  plag::DynamicBuffer buf;
  const auto* p = reinterpret_cast<const std::uint8_t*>(src);
  std::size_t n = 0;
  while (src[n] != '\0') ++n;
  buf.reserve_at_least(n);
  buf.append(p, n);
  return plag::tokenize(buf, pool, lower);
}
}  // namespace

PLAG_TEST(tokenizer_stub_emits_nothing_yet) {
  plag::TokenPool pool;
  const plag::TokenizeResult r = lex("int main() { return 0; }", pool, true);
  CHECK_EQ(r.tokens, std::size_t{0});
  CHECK(!r.malformed);
  PLAG_STUB_NOTE("Step 3 -- tokenizer implementation");
}

PLAG_TEST(whitespace_does_not_change_the_normalised_view) {
  // "a+b;" and "a + b ;" must produce the SAME normalised token list and
  // DIFFERENT byte offsets. This is what catches a reformatted copy.
  // TODO(Anas W2 Step 3): assert equal id sequences, equal counts.
  plag::TokenPool a;
  plag::TokenPool b;
  const plag::TokenizeResult ra = lex("a+b;", a, true);
  const plag::TokenizeResult rb = lex("a + b ;", b, true);
  CHECK_EQ(ra.tokens, rb.tokens);   // STUB: both 0 today
  PLAG_STUB_NOTE("Step 3 -- norm view equality");
}

PLAG_TEST(case_normalisation_is_optional) {
  // "int x=1;" vs "int y=1;" are equal ONLY with --normalize=case.
  // TODO(Anas W2 Step 3): assert the two id sequences differ, and that the
  // lower-cased ids of "INT" and "int" match.
  plag::TokenPool a;
  plag::TokenPool b;
  lex("int x=1;", a, true);
  lex("INT X=1;", b, true);
  CHECK_EQ(a.id.size(), b.id.size());   // STUB: both 0 today
  PLAG_STUB_NOTE("Step 3 -- case normalisation");
}

PLAG_TEST(a_comment_only_file_yields_zero_tokens) {
  plag::TokenPool pool;
  const plag::TokenizeResult r = lex("// just a comment\n/* block */\n", pool, true);
  CHECK_EQ(r.tokens, std::size_t{0});   // true even once implemented
  CHECK(!r.malformed);
}

PLAG_TEST(an_unterminated_string_is_one_token_and_sets_malformed) {
  // A string running off the end must produce exactly 1 token and set the
  // malformed flag -- never crash.
  // TODO(Anas W2 Step 3): assert tokens == 1 && malformed == true.
  plag::TokenPool pool;
  const plag::TokenizeResult r = lex("const char* s = \"oops", pool, true);
  CHECK(!r.malformed);   // STUB: flag not set yet
  PLAG_STUB_NOTE("Step 3 -- malformed input handling");
}

PLAG_TEST(token_line_numbers_are_filled_for_the_report) {
  // The token on file line 42 must report 42. Alyan's report reads this and
  // must never re-read a file to get it.
  // TODO(Anas W2 Step 3): build a 42-line fixture, assert line[0] == 42 for the
  // token on that line.
  plag::TokenPool pool;
  lex("int x;\n", pool, true);
  CHECK_EQ(pool.line.size(), std::size_t{0});   // STUB
  PLAG_STUB_NOTE("Step 3 -- posToLine fill");
}
