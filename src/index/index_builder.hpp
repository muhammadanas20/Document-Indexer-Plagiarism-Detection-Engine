// index_builder.hpp -- file -> tokens -> k-grams -> hash table; Alyan's API.
// Owner: Anas.  src/index/ is Anas's area.
// STUB (W4 / Step 6): dummy bodies. Hard deadline Sun 8 Nov -- Alyan's
// candidate finder compiles against lookup() from that day.
#ifndef PLAG_SRC_INDEX_INDEX_BUILDER_HPP_
#define PLAG_SRC_INDEX_INDEX_BUILDER_HPP_

#include <cstddef>
#include <cstdint>

#include "common/dyn_array.hpp"
#include "contract/contract.hpp"
#include "index/hash_table.hpp"

namespace plag {

// Kept alive per document so the report can print excerpts and line numbers
// without re-reading files.
struct DocumentView {
  DocumentId id = 0;
  const char* path = nullptr;
  DynArray<std::uint64_t> norm;   // normalised token ids (what we compare)
  DynArray<std::uint32_t> posToLine;  // token index -> 1-based line
};

class IndexBuilder {
 public:
  // One loop: roll(), then table_.insert(h, {id,pos}). No nested loops, which
  // is why the whole corpus costs O(total tokens) whatever k is.
  void add_document(DocumentId id, const char* path, const std::uint64_t* ids,
                    std::size_t n, std::size_t k);

  // Returns a pointer into the bucket -- no copying, because Alyan calls this
  // once per k-gram of every document.
  const Occurrence* lookup(std::uint64_t h, std::size_t* countOut) const;

  const DocumentView& doc(DocumentId id) const;   // norm tokens + posToLine
  const IndexStats& stats() const;                // for --stats and the report

 private:
  HashTable<std::uint64_t, OccurrenceList> table_;
  DynArray<DocumentView> docs_;
  IndexStats st_{};
};

}  // namespace plag

#endif  // PLAG_SRC_INDEX_INDEX_BUILDER_HPP_
