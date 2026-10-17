// Command-line argument parser and main pipeline runner
#ifndef PLAG_SRC_REPORT_CLI_HPP_
#define PLAG_SRC_REPORT_CLI_HPP_

namespace plag {

struct CliOptions {
  const char* command = "index";
  const char* corpus = nullptr;
  const char* output = nullptr;
  unsigned k = 15;
  const char* normalize = "case";
  const char* verify_mode = "single";
  const char* retain_docs = "auto";
  const char* format = "text";
  bool stats = false;
};

bool parse_args(int argc, char** argv, CliOptions& out);
void print_usage();
int run(const CliOptions& opts);

}  // namespace plag

#endif  // PLAG_SRC_REPORT_CLI_HPP_
