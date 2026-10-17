// Unit tests for AuditReport, json_writer, and CLI parsing
#include <cstdio>
#include <cstring>
#include <string>

#include "index/index_builder.hpp"
#include "report/audit_report.hpp"
#include "report/cli.hpp"
#include "report/json_writer.hpp"
#include "search/scorer.hpp"
#include "harness.hpp"

PLAG_TEST(audit_report_writes_text_output) {
  plag::IndexStats st{};
  st.files = 2;
  st.tokens = 40;
  st.kgrams = 30;
  st.distinct = 20;

  plag::AuditReport rep;
  rep.set_stats(st);
  rep.add_pair(plag::score_pair(0, 1, 10, 20, 20, 15, 1));

  plag::Span sp{0, 0, 15, 1, 0, 15};
  rep.add_spans(&sp, 1);

  std::FILE* tmp = std::tmpfile();
  CHECK(tmp != nullptr);
  if (tmp != nullptr) {
    CHECK(rep.write_text(tmp));
    std::fseek(tmp, 0, SEEK_END);
    long sz = std::ftell(tmp);
    CHECK(sz > 50);
    std::fclose(tmp);
  }
}

PLAG_TEST(json_escape_and_write) {
  char buf[64];
  const char* escaped = plag::json_escape_into(buf, sizeof(buf), "a\"b\\c\n");
  CHECK_EQ(std::string(escaped), "a\\\"b\\\\c\\n");

  plag::IndexStats st{};
  st.files = 2;
  plag::PairScore ps = plag::score_pair(0, 1, 5, 10, 10, 8, 1);
  plag::Span sp{0, 0, 8, 1, 0, 8};

  std::FILE* tmp = std::tmpfile();
  CHECK(tmp != nullptr);
  if (tmp != nullptr) {
    CHECK(plag::write_json(tmp, st, &ps, 1, &sp, 1));
    std::fseek(tmp, 0, SEEK_END);
    CHECK(std::ftell(tmp) > 50);
    std::fclose(tmp);
  }
}

PLAG_TEST(cli_parses_index_and_detect_flags) {
  const char* argv_idx[] = {"plag-detect", "index", "tests/corpus/anas/small",
                            "--k", "5", "--stats"};
  plag::CliOptions opts;
  CHECK(plag::parse_args(6, const_cast<char**>(argv_idx), opts));
  CHECK_EQ(std::string(opts.command), "index");
  CHECK_EQ(opts.k, 5u);
  CHECK(opts.stats);
}
