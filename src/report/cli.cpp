// Implementation of CLI argument parsing and pipeline execution
#include "report/cli.hpp"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "index/dir_walker.hpp"
#include "index/dynamic_buffer.hpp"
#include "index/file_reader.hpp"
#include "index/index_builder.hpp"
#include "index/span_merge.hpp"
#include "index/tokenizer.hpp"
#include "rank/max_heap.hpp"
#include "rank/rank_tree.hpp"
#include "report/audit_report.hpp"
#include "report/json_writer.hpp"
#include "search/candidate_generator.hpp"
#include "search/kmp.hpp"
#include "search/scorer.hpp"
#include "search/verify_mode.hpp"

namespace plag {
namespace {

bool str_eq(const char* a, const char* b) {
  return a != nullptr && b != nullptr && std::strcmp(a, b) == 0;
}

bool starts_with(const char* s, const char* prefix) {
  if (s == nullptr || prefix == nullptr) return false;
  std::size_t plen = std::strlen(prefix);
  return std::strncmp(s, prefix, plen) == 0;
}

bool parse_uint(const char* s, unsigned& out) {
  if (s == nullptr || s[0] == '\0') return false;
  char* end = nullptr;
  unsigned long val = std::strtoul(s, &end, 10);
  if (end == s || *end != '\0' || val == 0 || val > 10000) {
    return false;
  }
  out = static_cast<unsigned>(val);
  return true;
}

void print_stats_block(const IndexStats& st) {
  std::printf("files:         %zu\n", st.files);
  std::printf("bytes:         %zu\n", st.bytes);
  std::printf("tokens:        %zu\n", st.tokens);
  std::printf("kgrams:        %zu\n", st.kgrams);
  std::printf("distinct:      %zu\n", st.distinct);
  std::printf("buckets:       %zu\n", st.buckets);
  std::printf("alpha:         %.3f\n", st.alpha);
  std::printf("mean_chain:    %.3f\n", st.meanChain);
  std::printf("longest_chain: %u\n", st.longestChain);
  std::printf("time_ms:       %.2f\n", st.ms);
}

// Verify candidate pair using KMP over token arrays and merge into spans
std::size_t verify_pair_spans(const DocumentView& docA,
                              const DocumentView& docB,
                              std::size_t k,
                              DynArray<Span>& merged_out) {
  merged_out.clear();
  if (k == 0 || docA.norm.size() < k || docB.norm.size() < k) {
    return 0;
  }

  DynArray<Span> raw_hits;
  std::size_t windowsA = docA.norm.size() - k + 1;

  for (std::size_t i = 0; i < windowsA; ++i) {
    DynArray<std::uint32_t> matchesB;
    kmp_find_all(docB.norm.data(), docB.norm.size(),
                 docA.norm.data() + i, k, matchesB);
    for (std::size_t m = 0; m < matchesB.size(); ++m) {
      Span hit;
      hit.docA = docA.id;
      hit.startA = static_cast<std::uint32_t>(i);
      hit.endA = static_cast<std::uint32_t>(i + k);
      hit.docB = docB.id;
      hit.startB = matchesB[m];
      hit.endB = matchesB[m] + static_cast<std::uint32_t>(k);
      raw_hits.push_back(hit);
    }
  }

  merge_spans(raw_hits.data(), raw_hits.size(), 0, merged_out);

  // Count distinct matched token positions in docA
  DynArray<std::uint8_t> covered(docA.norm.size());
  for (std::size_t i = 0; i < docA.norm.size(); ++i) {
    covered.push_back(0u);
  }
  for (std::size_t i = 0; i < merged_out.size(); ++i) {
    std::uint32_t endPos = merged_out[i].endA;
    if (endPos > docA.norm.size()) {
      endPos = static_cast<std::uint32_t>(docA.norm.size());
    }
    for (std::uint32_t p = merged_out[i].startA; p < endPos; ++p) {
      covered[p] = 1u;
    }
  }

  std::size_t total_matched = 0;
  for (std::size_t i = 0; i < covered.size(); ++i) {
    if (covered[i] != 0u) {
      ++total_matched;
    }
  }
  return total_matched;
}

}  // namespace

bool parse_args(int argc, char** argv, CliOptions& out) {
  out = CliOptions{};
  if (argc < 2) {
    return false;
  }

  int i = 1;
  if (str_eq(argv[i], "index") || str_eq(argv[i], "detect") ||
      str_eq(argv[i], "report")) {
    out.command = argv[i];
    ++i;
  } else if (argv[i][0] != '-') {
    out.command = "detect";
    out.corpus = argv[i];
    ++i;
  } else {
    return false;
  }

  while (i < argc) {
    const char* arg = argv[i];
    if (str_eq(arg, "--stats")) {
      out.stats = true;
      ++i;
    } else if (str_eq(arg, "-o") || str_eq(arg, "--output")) {
      if (i + 1 >= argc) return false;
      out.output = argv[i + 1];
      i += 2;
    } else if (str_eq(arg, "--k")) {
      if (i + 1 >= argc || !parse_uint(argv[i + 1], out.k)) return false;
      i += 2;
    } else if (starts_with(arg, "--k=")) {
      if (!parse_uint(arg + 4, out.k)) return false;
      ++i;
    } else if (str_eq(arg, "--normalize")) {
      if (i + 1 >= argc) return false;
      out.normalize = argv[i + 1];
      i += 2;
    } else if (starts_with(arg, "--normalize=")) {
      out.normalize = arg + 12;
      ++i;
    } else if (str_eq(arg, "--verify-mode")) {
      if (i + 1 >= argc) return false;
      out.verify_mode = argv[i + 1];
      i += 2;
    } else if (starts_with(arg, "--verify-mode=")) {
      out.verify_mode = arg + 14;
      ++i;
    } else if (str_eq(arg, "--retain-docs")) {
      if (i + 1 >= argc) return false;
      out.retain_docs = argv[i + 1];
      i += 2;
    } else if (starts_with(arg, "--retain-docs=")) {
      out.retain_docs = arg + 14;
      ++i;
    } else if (str_eq(arg, "--format")) {
      if (i + 1 >= argc) return false;
      out.format = argv[i + 1];
      i += 2;
    } else if (starts_with(arg, "--format=")) {
      out.format = arg + 9;
      ++i;
    } else if (arg[0] != '-' && out.corpus == nullptr) {
      out.corpus = arg;
      ++i;
    } else {
      return false;
    }
  }

  if (out.corpus == nullptr) {
    return false;
  }
  if (!str_eq(out.normalize, "case") && !str_eq(out.normalize, "none")) {
    return false;
  }
  if (!str_eq(out.format, "text") && !str_eq(out.format, "json")) {
    return false;
  }
  if (!str_eq(out.verify_mode, "single") &&
      !str_eq(out.verify_mode, "double") &&
      !str_eq(out.verify_mode, "exact")) {
    return false;
  }
  return true;
}

void print_usage() {
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

int run(const CliOptions& opts) {
  if (str_eq(opts.command, "report")) {
    DynamicBuffer buf;
    ErrorCode rc = read_file(opts.corpus, buf);
    if (rc != ErrorCode::Ok) {
      return 3;
    }
    std::fwrite(buf.data(), 1, buf.size(), stdout);
    return 0;
  }

  DynArray<char*> paths;
  ErrorCode wrc = walk_dir(opts.corpus, paths);
  if (wrc != ErrorCode::Ok) {
    return 3;
  }

  const bool lower_case = str_eq(opts.normalize, "case");
  IndexBuilder index;

  auto t0 = std::chrono::steady_clock::now();
  DocumentId next_doc_id = 0;

  for (std::size_t i = 0; i < paths.size(); ++i) {
    DynamicBuffer buf;
    ErrorCode frc = read_file(paths[i], buf);
    if (frc == ErrorCode::Ok) {
      TokenPool pool;
      TokenizeResult tr = tokenize(buf, pool, lower_case);
      index.add_tokenized_document(next_doc_id, paths[i], pool, opts.k,
                                   buf.size(), tr.malformed);
      ++next_doc_id;
    } else if (frc == ErrorCode::EmptyFile) {
      TokenPool empty_pool;
      index.add_tokenized_document(next_doc_id, paths[i], empty_pool, opts.k,
                                   0, false);
      ++next_doc_id;
    }
  }

  auto t1 = std::chrono::steady_clock::now();
  double elapsed_ms =
      std::chrono::duration<double, std::milli>(t1 - t0).count();
  index.set_elapsed_ms(elapsed_ms);

  if (opts.stats || str_eq(opts.command, "index")) {
    print_stats_block(index.stats());
  }

  if (str_eq(opts.command, "index")) {
    free_walk_paths(paths);
    return 0;
  }

  // Candidate generation and verification
  DynArray<CandidatePair> candidates;
  generate_candidates(index, 1, candidates);

  MaxHeap heap;
  RankTree tree;
  DynArray<Span> all_spans;

  for (std::size_t i = 0; i < candidates.size(); ++i) {
    const CandidatePair& cp = candidates[i];
    const DocumentView& docA = index.doc(cp.docA);
    const DocumentView& docB = index.doc(cp.docB);

    DynArray<Span> pair_spans;
    std::size_t matched_tokens =
        verify_pair_spans(docA, docB, opts.k, pair_spans);

    if (matched_tokens > 0) {
      PairScore ps = score_pair(
          cp.docA, cp.docB, cp.sharedKgrams,
          docA.norm.size(), docB.norm.size(),
          matched_tokens, static_cast<std::uint32_t>(pair_spans.size()));
      heap.push(ps);
      tree.insert(ps);
      for (std::size_t s = 0; s < pair_spans.size(); ++s) {
        all_spans.push_back(pair_spans[s]);
      }
    }
  }

  DynArray<PairScore> ranked_pairs;
  heap.top_k(heap.size(), ranked_pairs);

  std::FILE* out_stream = stdout;
  if (opts.output != nullptr) {
    out_stream = std::fopen(opts.output, "w");
    if (out_stream == nullptr) {
      free_walk_paths(paths);
      return 3;
    }
  }

  bool write_ok = false;
  if (str_eq(opts.format, "json")) {
    write_ok = write_json(out_stream, index.stats(),
                          ranked_pairs.data(), ranked_pairs.size(),
                          all_spans.data(), all_spans.size());
  } else {
    AuditReport report;
    report.set_stats(index.stats());
    report.set_index(&index);
    for (std::size_t i = 0; i < ranked_pairs.size(); ++i) {
      report.add_pair(ranked_pairs[i]);
    }
    report.add_spans(all_spans.data(), all_spans.size());
    write_ok = report.write_text(out_stream);
  }

  if (opts.output != nullptr) {
    std::fclose(out_stream);
  }

  free_walk_paths(paths);
  if (!write_ok) {
    return 3;
  }
  return ranked_pairs.empty() ? 1 : 0;
}

}  // namespace plag
