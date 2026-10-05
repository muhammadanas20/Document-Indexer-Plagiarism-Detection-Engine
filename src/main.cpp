// main.cpp -- the plag-detect entry point.
// Owner: Anas.  Command wiring lands in the week docs/tasks.md lists
// main.cpp; Alyan requests the wiring or wires it himself that week.
//
// WEEK 0 STUB: prints the version banner and usage, then exits 0. Real
// subcommands (index / detect / report) arrive with Step 6 (Anas) and Step 8
// (Alyan's CLI).
#include <cstdio>
#include <cstring>

#include "report/cli.hpp"

namespace {

constexpr const char* kVersion = "plag-detect 0.1 (stub)";

bool is_flag(const char* a, const char* b) { return std::strcmp(a, b) == 0; }

}  // namespace

int main(int argc, char** argv) {
  if (argc < 2) {
    std::printf("%s\n", kVersion);
    plag::print_usage();
    return 0;
  }

  if (is_flag(argv[1], "--version") || is_flag(argv[1], "-v")) {
    std::printf("%s\n", kVersion);
    return 0;
  }

  if (is_flag(argv[1], "--help") || is_flag(argv[1], "-h")) {
    std::printf("%s\n", kVersion);
    plag::print_usage();
    return 0;
  }

  // TODO(Anas W4 / Step 6): `index <corpus> --stats`
  // TODO(Alyan W6 / Step 8): parse_args() + run() + exit codes 0/1/2/3.
  plag::CliOptions opts;
  if (!plag::parse_args(argc, argv, opts)) {
    std::fprintf(stderr, "%s: bad argument (see --help)\n", kVersion);
    return 2;  // ErrorCode::BadArg -> exit 2
  }
  return plag::run(opts);
}
