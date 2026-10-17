// Implementation of IndexBuilder
#include "index/index_builder.hpp"

#include <cstring>

#include "index/rolling_hash.hpp"

namespace plag {

void IndexBuilder::add_document(DocumentId id, const char* path,
                                const std::uint64_t* ids, std::size_t n,
                                std::size_t k) {
  DocumentView dv;
  dv.id = id;
  if (path != nullptr) {
    std::size_t len = std::strlen(path);
    dv.pathBuf.reserve(len + 1);
    for (std::size_t i = 0; i <= len; ++i) {
      dv.pathBuf.push_back(path[i]);
    }
    dv.path = dv.pathBuf.data();
  }

  dv.norm.reserve(n);
  dv.posToLine.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    dv.norm.push_back(ids[i]);
    dv.posToLine.push_back(1u);
  }
  docs_.push_back(std::move(dv));

  ++st_.files;
  st_.tokens += n;

  if (k > 0 && n >= k && ids != nullptr) {
    RollingHash rh;
    rh.init(ids, k, RollingHash::random_base(), RollingHash::default_modulus());

    std::size_t num_windows = n - k + 1;
    for (std::size_t pos = 0; pos < num_windows; ++pos) {
      std::uint64_t h = rh.hash();
      Occurrence occ{id, static_cast<std::uint32_t>(pos)};

      OccurrenceList* list = table_.find(h);
      if (list == nullptr) {
        OccurrenceList fresh;
        fresh.push_back(occ);
        table_.insert(h, fresh);
      } else {
        list->push_back(occ);
      }

      ++st_.kgrams;
      if (pos + k < n) {
        rh.roll(ids[pos], ids[pos + k]);
      }
    }
  }

  st_.distinct = table_.size();
  st_.buckets = table_.bucketCount();
  st_.alpha = table_.meanChainLength();
  st_.meanChain = table_.meanChainLength();
  st_.longestChain = static_cast<std::uint32_t>(table_.longestChain());
}

void IndexBuilder::add_tokenized_document(DocumentId id, const char* path,
                                          const TokenPool& pool, std::size_t k,
                                          std::size_t file_bytes,
                                          bool malformed) {
  add_document(id, path, pool.id.data(), pool.id.size(), k);
  st_.bytes += file_bytes;
  if (malformed) {
    st_.malformedSeen = true;
  }

  // Copy real line numbers from the tokenizer pool into the stored DocumentView
  for (std::size_t i = 0; i < docs_.size(); ++i) {
    if (docs_[i].id == id) {
      docs_[i].posToLine.clear();
      docs_[i].posToLine.reserve(pool.line.size());
      for (std::size_t j = 0; j < pool.line.size(); ++j) {
        docs_[i].posToLine.push_back(pool.line[j]);
      }
      break;
    }
  }
}

const Occurrence* IndexBuilder::lookup(std::uint64_t h,
                                       std::size_t* countOut) const {
  const OccurrenceList* list = table_.find(h);
  if (list == nullptr || list->empty()) {
    if (countOut != nullptr) {
      *countOut = 0;
    }
    return nullptr;
  }
  if (countOut != nullptr) {
    *countOut = list->size();
  }
  return list->data();
}

const DocumentView& IndexBuilder::doc(DocumentId id) const {
  for (std::size_t i = 0; i < docs_.size(); ++i) {
    if (docs_[i].id == id) {
      return docs_[i];
    }
  }
  static const DocumentView kEmpty{};
  return kEmpty;
}

const IndexStats& IndexBuilder::stats() const {
  return st_;
}

}  // namespace plag
