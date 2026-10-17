// Implementation of candidate pair generation
#include "search/candidate_generator.hpp"

#include "index/hash_table.hpp"
#include "index/index_builder.hpp"

namespace plag {
namespace {

std::uint64_t pack_pair(DocumentId a, DocumentId b) {
  if (a > b) {
    DocumentId tmp = a;
    a = b;
    b = tmp;
  }
  return (static_cast<std::uint64_t>(a) << 32) | static_cast<std::uint64_t>(b);
}

bool contains_doc(const DynArray<DocumentId>& docs, DocumentId d) {
  for (std::size_t i = 0; i < docs.size(); ++i) {
    if (docs[i] == d) return true;
  }
  return false;
}

bool cand_less(const CandidatePair& x, const CandidatePair& y) {
  if (x.sharedKgrams != y.sharedKgrams) {
    return x.sharedKgrams > y.sharedKgrams;
  }
  if (x.docA != y.docA) return x.docA < y.docA;
  return x.docB < y.docB;
}

void sort_candidates(DynArray<CandidatePair>& arr) {
  for (std::size_t i = 1; i < arr.size(); ++i) {
    CandidatePair key = arr[i];
    std::size_t j = i;
    while (j > 0 && cand_less(key, arr[j - 1])) {
      arr[j] = arr[j - 1];
      --j;
    }
    arr[j] = key;
  }
}

}  // namespace

std::size_t generate_candidates(const IndexBuilder& index, std::size_t minShared,
                                DynArray<CandidatePair>& out) {
  out.clear();
  if (minShared == 0) {
    minShared = 1;
  }

  HashTable<std::uint64_t, std::uint32_t> pair_counts;

  index.table().for_each_bucket(
      [&pair_counts](std::uint64_t /*hash*/, const OccurrenceList& list) {
        if (list.size() < 2) return;

        // Collect unique document IDs for this fingerprint
        DynArray<DocumentId> unique_docs;
        for (std::size_t i = 0; i < list.size(); ++i) {
          if (!contains_doc(unique_docs, list[i].docId)) {
            unique_docs.push_back(list[i].docId);
          }
        }

        // Increment shared fingerprint count for each distinct pair of documents
        for (std::size_t i = 0; i < unique_docs.size(); ++i) {
          for (std::size_t j = i + 1; j < unique_docs.size(); ++j) {
            std::uint64_t key = pack_pair(unique_docs[i], unique_docs[j]);
            std::uint32_t* cnt = pair_counts.find(key);
            if (cnt == nullptr) {
              pair_counts.insert(key, 1u);
            } else {
              ++(*cnt);
            }
          }
        }
      });

  pair_counts.for_each_bucket(
      [&out, minShared](std::uint64_t key, std::uint32_t count) {
        if (count >= minShared) {
          CandidatePair cp;
          cp.docA = static_cast<DocumentId>(key >> 32);
          cp.docB = static_cast<DocumentId>(key & 0xFFFFFFFFULL);
          cp.sharedKgrams = count;
          out.push_back(cp);
        }
      });

  sort_candidates(out);
  return out.size();
}

}  // namespace plag
