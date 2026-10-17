// Builds the k-gram inverted index over tokenized documents
#ifndef PLAG_SRC_INDEX_INDEX_BUILDER_HPP_
#define PLAG_SRC_INDEX_INDEX_BUILDER_HPP_

#include <cstddef>
#include <cstdint>

#include "common/dyn_array.hpp"
#include "contract/contract.hpp"
#include "index/hash_table.hpp"
#include "index/tokenizer.hpp"

namespace plag {

struct DocumentView {
  DocumentId id = 0;
  const char* path = nullptr;
  DynArray<char> pathBuf;
  DynArray<std::uint64_t> norm;
  DynArray<std::uint32_t> posToLine;
};

class IndexBuilder {
 public:
  void add_document(DocumentId id, const char* path, const std::uint64_t* ids,
                    std::size_t n, std::size_t k);

  void add_tokenized_document(DocumentId id, const char* path,
                              const TokenPool& pool, std::size_t k,
                              std::size_t file_bytes, bool malformed);

  const Occurrence* lookup(std::uint64_t h, std::size_t* countOut) const;

  const DocumentView& doc(DocumentId id) const;
  std::size_t doc_count() const { return docs_.size(); }
  const DynArray<DocumentView>& docs() const { return docs_; }
  const HashTable<std::uint64_t, OccurrenceList>& table() const { return table_; }

  const IndexStats& stats() const;
  void set_elapsed_ms(double ms) { st_.ms = ms; }

 private:
  HashTable<std::uint64_t, OccurrenceList> table_;
  DynArray<DocumentView> docs_;
  IndexStats st_{};
};

}  // namespace plag

#endif  // PLAG_SRC_INDEX_INDEX_BUILDER_HPP_
