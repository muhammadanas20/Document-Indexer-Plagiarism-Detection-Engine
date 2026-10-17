// Entry point for the plag-detect command-line tool
#include <cstdio>
#include <cstring>

#include "report/cli.hpp"

namespace {

constexpr const char* kVersion = "plag-detect 1.0";

bool is_flag(const char* a, const char* b) {
  return std::strcmp(a, b) == 0;
}

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

  plag::CliOptions opts;
  if (!plag::parse_args(argc, argv, opts)) {
    std::fprintf(stderr, "%s: bad argument (see --help)\n", kVersion);
    return 2;
  }
  return plag::run(opts);
}
