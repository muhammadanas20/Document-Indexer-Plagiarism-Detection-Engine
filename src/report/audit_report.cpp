// Implementation of AuditReport text output
#include "report/audit_report.hpp"

#include "index/index_builder.hpp"
#include "search/verify_mode.hpp"

namespace plag {
namespace {

std::uint32_t token_line(const IndexBuilder* index, DocumentId docId,
                         std::uint32_t tokenPos) {
  if (index == nullptr) return 1u;
  const DocumentView& dv = index->doc(docId);
  if (dv.posToLine.empty()) return 1u;
  if (tokenPos >= dv.posToLine.size()) {
    return dv.posToLine.back();
  }
  return dv.posToLine[tokenPos];
}

const char* doc_name(const IndexBuilder* index, DocumentId docId) {
  if (index == nullptr) return nullptr;
  const DocumentView& dv = index->doc(docId);
  return dv.path;
}

}  // namespace

void AuditReport::set_stats(const IndexStats& st) {
  stats_ = st;
}

void AuditReport::set_index(const IndexBuilder* index) {
  index_ = index;
}

void AuditReport::add_pair(const PairScore& p) {
  pairs_.push_back(p);
}

void AuditReport::add_spans(const Span* spans, std::size_t n) {
  if (spans == nullptr) return;
  for (std::size_t i = 0; i < n; ++i) {
    spans_.push_back(spans[i]);
  }
}

bool AuditReport::write_text(std::FILE* out) const {
  if (out == nullptr) {
    return false;
  }

  std::fprintf(out, "=== Plagiarism Detection Audit Report ===\n");
  std::fprintf(out, "Files indexed : %zu\n", stats_.files);
  std::fprintf(out, "Bytes read    : %zu\n", stats_.bytes);
  std::fprintf(out, "Total tokens  : %zu\n", stats_.tokens);
  std::fprintf(out, "Total k-grams : %zu\n", stats_.kgrams);
  std::fprintf(out, "Distinct keys : %zu\n", stats_.distinct);
  std::fprintf(out, "Hash buckets  : %zu\n", stats_.buckets);
  std::fprintf(out, "Load factor   : %.3f\n", stats_.alpha);
  std::fprintf(out, "Longest chain : %u\n", stats_.longestChain);
  std::fprintf(out, "Index time    : %.2f ms\n\n", stats_.ms);

  std::fprintf(out, "--- Suspicious Document Pairs (%zu) ---\n", pairs_.size());
  for (std::size_t i = 0; i < pairs_.size(); ++i) {
    const PairScore& p = pairs_[i];
    const char* nameA = doc_name(index_, p.docA);
    const char* nameB = doc_name(index_, p.docB);
    Suspicion susp = static_cast<Suspicion>(p.label);

    if (nameA != nullptr && nameB != nullptr) {
      std::fprintf(out, "[%zu] doc %u (%s) <-> doc %u (%s)\n",
                   i + 1, p.docA, nameA, p.docB, nameB);
    } else {
      std::fprintf(out, "[%zu] doc %u <-> doc %u\n",
                   i + 1, p.docA, p.docB);
    }
    std::fprintf(out,
                 "    jaccard=%.3f  containment=%.3f  coverage=%.3f  "
                 "matchedTokens=%u  spans=%u  label=%s\n",
                 p.jaccard, p.containment, p.coverage,
                 p.matchedTokens, p.spans, to_string(susp));
  }

  if (!spans_.empty()) {
    std::fprintf(out, "\n--- Matched Spans (%zu) ---\n", spans_.size());
    for (std::size_t i = 0; i < spans_.size(); ++i) {
      const Span& s = spans_[i];
      std::uint32_t endTokA = (s.endA > 0) ? (s.endA - 1) : s.startA;
      std::uint32_t endTokB = (s.endB > 0) ? (s.endB - 1) : s.startB;
      std::uint32_t lineStartA = token_line(index_, s.docA, s.startA);
      std::uint32_t lineEndA = token_line(index_, s.docA, endTokA);
      std::uint32_t lineStartB = token_line(index_, s.docB, s.startB);
      std::uint32_t lineEndB = token_line(index_, s.docB, endTokB);

      std::fprintf(out,
                   "  doc %u tokens [%u..%u) (lines %u-%u) == "
                   "doc %u tokens [%u..%u) (lines %u-%u) [%u tokens]\n",
                   s.docA, s.startA, s.endA, lineStartA, lineEndA,
                   s.docB, s.startB, s.endB, lineStartB, lineEndB,
                   s.tokens());
    }
  }

  return std::ferror(out) == 0;
}

}  // namespace plag
