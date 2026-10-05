// cli.hpp -- flags, subcommands, exit codes.
// Owner: ALYAN.  src/report/ is Alyan's area.
// STUB (Alyan, W6): dummy bodies.
#ifndef PLAG_SRC_REPORT_CLI_HPP_
#define PLAG_SRC_REPORT_CLI_HPP_

namespace plag {

struct CliOptions {
  const char* command = "index";    // index | detect | report
  const char* corpus = nullptr;
  const char* output = nullptr;
  unsigned k = 15;
  const char* normalize = "case";
  const char* verify_mode = "single";
  const char* retain_docs = "auto";  // auto | yes | no
  const char* format = "text";       // text | json
  bool stats = false;
};

// Returns false and prints usage on a bad argument (ErrorCode::BadArg).
bool parse_args(int argc, char** argv, CliOptions& out);
void print_usage();

// Exit codes: 0 ok, 1 no findings, 2 bad argument, 3 io error.
int run(const CliOptions& opts);

}  // namespace plag

#endif  // PLAG_SRC_REPORT_CLI_HPP_
