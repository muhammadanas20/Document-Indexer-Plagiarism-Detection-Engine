// Implementation of JSON report writer and string escaping
#include "report/json_writer.hpp"

#include "search/verify_mode.hpp"

namespace plag {

const char* json_escape_into(char* buf, std::size_t bufSize, const char* s) {
  if (buf == nullptr || bufSize == 0) {
    return "";
  }
  if (s == nullptr) {
    buf[0] = '\0';
    return buf;
  }

  std::size_t out_idx = 0;
  for (std::size_t i = 0; s[i] != '\0'; ++i) {
    char c = s[i];
    const char* rep = nullptr;
    switch (c) {
      case '"':
        rep = "\\\"";
        break;
      case '\\':
        rep = "\\\\";
        break;
      case '\n':
        rep = "\\n";
        break;
      case '\r':
        rep = "\\r";
        break;
      case '\t':
        rep = "\\t";
        break;
      default:
        break;
    }

    if (rep != nullptr) {
      if (out_idx + 2 >= bufSize) break;
      buf[out_idx++] = rep[0];
      buf[out_idx++] = rep[1];
    } else {
      if (out_idx + 1 >= bufSize) break;
      buf[out_idx++] = c;
    }
  }
  buf[out_idx] = '\0';
  return buf;
}

bool write_json(std::FILE* out, const IndexStats& stats,
                const PairScore* pairs, std::size_t nPairs,
                const Span* spans, std::size_t nSpans) {
  if (out == nullptr) {
    return false;
  }

  std::fprintf(out, "{\n");
  std::fprintf(out, "  \"stats\": {\n");
  std::fprintf(out, "    \"files\": %zu,\n", stats.files);
  std::fprintf(out, "    \"bytes\": %zu,\n", stats.bytes);
  std::fprintf(out, "    \"tokens\": %zu,\n", stats.tokens);
  std::fprintf(out, "    \"kgrams\": %zu,\n", stats.kgrams);
  std::fprintf(out, "    \"distinct\": %zu,\n", stats.distinct);
  std::fprintf(out, "    \"buckets\": %zu,\n", stats.buckets);
  std::fprintf(out, "    \"alpha\": %.4f,\n", stats.alpha);
  std::fprintf(out, "    \"longestChain\": %u,\n", stats.longestChain);
  std::fprintf(out, "    \"ms\": %.2f\n", stats.ms);
  std::fprintf(out, "  },\n");

  std::fprintf(out, "  \"pairs\": [\n");
  if (pairs != nullptr) {
    for (std::size_t i = 0; i < nPairs; ++i) {
      const PairScore& p = pairs[i];
      const char* comma = (i + 1 < nPairs) ? "," : "";
      Suspicion susp = static_cast<Suspicion>(p.label);
      std::fprintf(out,
                   "    {\"docA\": %u, \"docB\": %u, \"jaccard\": %.4f, "
                   "\"containment\": %.4f, \"coverage\": %.4f, "
                   "\"matchedTokens\": %u, \"spans\": %u, \"label\": \"%s\"}%s\n",
                   p.docA, p.docB, p.jaccard, p.containment, p.coverage,
                   p.matchedTokens, p.spans, to_string(susp), comma);
    }
  }
  std::fprintf(out, "  ],\n");

  std::fprintf(out, "  \"spans\": [\n");
  if (spans != nullptr) {
    for (std::size_t i = 0; i < nSpans; ++i) {
      const Span& s = spans[i];
      const char* comma = (i + 1 < nSpans) ? "," : "";
      std::fprintf(out,
                   "    {\"docA\": %u, \"startA\": %u, \"endA\": %u, "
                   "\"docB\": %u, \"startB\": %u, \"endB\": %u, "
                   "\"tokens\": %u}%s\n",
                   s.docA, s.startA, s.endA, s.docB, s.startB, s.endB,
                   s.tokens(), comma);
    }
  }
  std::fprintf(out, "  ]\n");
  std::fprintf(out, "}\n");

  return std::ferror(out) == 0;
}

}  // namespace plag
