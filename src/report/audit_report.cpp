// audit_report.cpp -- STUB (Alyan, W6).
#include "report/audit_report.hpp"

namespace plag {

void AuditReport::set_stats(const IndexStats& st) { stats_ = st; }

void AuditReport::add_pair(const PairScore& p) { pairs_.push_back(p); }

void AuditReport::add_spans(const Span* spans, std::size_t n) {
  for (std::size_t i = 0; i < n; ++i) spans_.push_back(spans[i]);
}

bool AuditReport::write_text(std::FILE* /*out*/) const {
  // TODO(Alyan W6): header from stats_, ranked pairs, excerpted spans.
  return false;  // STUB dummy
}

}  // namespace plag
