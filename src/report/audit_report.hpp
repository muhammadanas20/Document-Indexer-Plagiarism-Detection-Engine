// Generates human-readable text audit reports for plagiarism detection
#ifndef PLAG_SRC_REPORT_AUDIT_REPORT_HPP_
#define PLAG_SRC_REPORT_AUDIT_REPORT_HPP_

#include <cstddef>
#include <cstdio>

#include "common/dyn_array.hpp"
#include "contract/contract.hpp"

namespace plag {

class IndexBuilder;

class AuditReport {
 public:
  void set_stats(const IndexStats& st);
  void set_index(const IndexBuilder* index);
  void add_pair(const PairScore& p);
  void add_spans(const Span* spans, std::size_t n);
  bool write_text(std::FILE* out) const;

 private:
  IndexStats stats_{};
  const IndexBuilder* index_ = nullptr;
  DynArray<PairScore> pairs_;
  DynArray<Span> spans_;
};

}  // namespace plag

#endif  // PLAG_SRC_REPORT_AUDIT_REPORT_HPP_
