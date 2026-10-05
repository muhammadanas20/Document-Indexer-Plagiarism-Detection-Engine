// cli.cpp -- STUB (Alyan, W6).
#include "report/cli.hpp"

#include <cstdio>

namespace plag {

bool parse_args(int /*argc*/, char** /*argv*/, CliOptions& /*out*/) {
  // TODO(Alyan W6): flag parsing; no config files beyond the agreed flags.
  return false;  // STUB dummy
}

void print_usage() {
  // The version banner is printed by main.cpp; usage only prints usage, so
  // `plag-detect --help` does not repeat the version line.
  std::printf(
      "usage: plag-detect <index|detect|report> <corpus> [flags]\n"
      "\n"
      "  index  <corpus>   build the index; --stats prints IndexStats\n"
      "  detect <corpus>   find and rank suspicious pairs; -o <file>\n"
      "  report <file>     re-render an existing audit report\n"
      "\n"
      "  --k <n>              window size in tokens (default 15)\n"
      "  --normalize <mode>   case | none (default case)\n"
      "  --verify-mode <m>    single | double | exact\n"
      "  --retain-docs <m>    auto | yes | no (default auto)\n"
      "  --format <f>         text | json\n"
      "  --stats              print the IndexStats block\n"
      "\n"
      "exit codes: 0 ok, 1 no findings, 2 bad argument, 3 io error\n");
}

int run(const CliOptions& /*opts*/) { return 0; }  // STUB dummy

}  // namespace plag
