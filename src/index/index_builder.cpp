// index_builder.cpp -- STUB (Anas, W4 / Step 6).
#include "index/index_builder.hpp"

namespace plag {

void IndexBuilder::add_document(DocumentId /*id*/, const char* /*path*/,
                                const std::uint64_t* /*ids*/, std::size_t /*n*/,
                                std::size_t /*k*/) {
  // TODO(Anas W4 Step 6): roll the window, insert every fingerprint, and keep
  // per-document counters (tokens, k-grams, distinct -- a repeat inside the
  // same document is not a new fingerprint; check the last occurrence's docId).
}

const Occurrence* IndexBuilder::lookup(std::uint64_t /*h*/,
                                       std::size_t* countOut) const {
  if (countOut != nullptr) *countOut = 0;
  return nullptr;  // STUB dummy: index is empty.
}

const DocumentView& IndexBuilder::doc(DocumentId /*id*/) const {
  static const DocumentView kEmpty{};
  return kEmpty;  // STUB dummy
}

const IndexStats& IndexBuilder::stats() const { return st_; }

}  // namespace plag
