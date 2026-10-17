// Implementation of span_merge
#include "index/span_merge.hpp"

#include <cassert>
#include <cstdint>

namespace plag {
namespace {

std::int64_t diag_offset(const Span& s) {
  return static_cast<std::int64_t>(s.startB) -
         static_cast<std::int64_t>(s.startA);
}

bool span_less(const Span& a, const Span& b) {
  if (a.docA != b.docA) return a.docA < b.docA;
  if (a.docB != b.docB) return a.docB < b.docB;
  std::int64_t da = diag_offset(a);
  std::int64_t db = diag_offset(b);
  if (da != db) return da < db;
  if (a.startA != b.startA) return a.startA < b.startA;
  return a.endA < b.endA;
}

// Simple insertion sort over spans (avoids std::sort)
void sort_spans(DynArray<Span>& arr) {
  for (std::size_t i = 1; i < arr.size(); ++i) {
    Span key = arr[i];
    std::size_t j = i;
    while (j > 0 && span_less(key, arr[j - 1])) {
      arr[j] = arr[j - 1];
      --j;
    }
    arr[j] = key;
  }
}

}  // namespace

std::size_t merge_spans(const Span* hits, std::size_t n, std::size_t gap,
                        DynArray<Span>& out) {
  out.clear();
  if (hits == nullptr || n == 0) {
    return 0;
  }

  DynArray<Span> sorted;
  sorted.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    if (hits[i].endA > hits[i].startA &&
        (hits[i].endA - hits[i].startA) == (hits[i].endB - hits[i].startB)) {
      sorted.push_back(hits[i]);
    }
  }

  if (sorted.empty()) {
    return 0;
  }

  sort_spans(sorted);

  Span cur = sorted[0];
  for (std::size_t i = 1; i < sorted.size(); ++i) {
    const Span& next = sorted[i];
    bool same_docs = (cur.docA == next.docA && cur.docB == next.docB);
    bool same_diag = (diag_offset(cur) == diag_offset(next));
    bool within_gap = (next.startA <= cur.endA + static_cast<std::uint32_t>(gap));

    if (same_docs && same_diag && within_gap) {
      if (next.endA > cur.endA) {
        cur.endA = next.endA;
        cur.endB = next.endB;
      }
    } else {
      assert((cur.endA - cur.startA) == (cur.endB - cur.startB));
      out.push_back(cur);
      cur = next;
    }
  }

  assert((cur.endA - cur.startA) == (cur.endB - cur.startB));
  out.push_back(cur);
  return out.size();
}

}  // namespace plag
