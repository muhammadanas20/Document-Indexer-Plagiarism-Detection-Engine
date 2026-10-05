// audit_report.hpp -- the human-readable audit report.
// Owner: ALYAN.  src/report/ is Alyan's area.
// STUB (Alyan, W6): dummy bodies.
#ifndef PLAG_SRC_REPORT_AUDIT_REPORT_HPP_
#define PLAG_SRC_REPORT_AUDIT_REPORT_HPP_

#include <cstddef>
#include <cstdio>

#include "common/dyn_array.hpp"
#include "contract/contract.hpp"

namespace plag {

// Reads line numbers from Anas's DocumentView::posToLine -- it must never
// re-read a file to print an excerpt.
class AuditReport {
 public:
  void set_stats(const IndexStats& st);
  void add_pair(const PairScore& p);
  void add_spans(const Span* spans, std::size_t n);
  bool write_text(std::FILE* out) const;

 private:
  IndexStats stats_{};
  DynArray<PairScore> pairs_;
  DynArray<Span> spans_;
};

}  // namespace plag

#endif  // PLAG_SRC_REPORT_AUDIT_REPORT_HPP_
