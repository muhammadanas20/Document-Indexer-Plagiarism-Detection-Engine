// Implementation of MaxHeap
#include "rank/max_heap.hpp"

#include "search/scorer.hpp"

namespace plag {
namespace {

// Returns true if 'a' has strictly higher priority than 'b'
bool higher_priority(const PairScore& a, const PairScore& b) {
  double ka = suspicion_key(a);
  double kb = suspicion_key(b);
  if (ka != kb) return ka > kb;
  if (a.matchedTokens != b.matchedTokens) {
    return a.matchedTokens > b.matchedTokens;
  }
  if (a.docA != b.docA) return a.docA < b.docA;
  return a.docB < b.docB;
}

}  // namespace

void MaxHeap::push(const PairScore& p) {
  h_.push_back(p);
  sift_up(h_.size() - 1);
}

PairScore MaxHeap::pop() {
  if (h_.empty()) {
    return PairScore{};
  }
  PairScore root = h_[0];
  h_[0] = h_.back();
  h_.pop_back();
  if (!h_.empty()) {
    sift_down(0);
  }
  return root;
}

const PairScore& MaxHeap::top() const {
  static const PairScore kEmpty{};
  return h_.empty() ? kEmpty : h_[0];
}

std::size_t MaxHeap::size() const { return h_.size(); }
bool MaxHeap::empty() const { return h_.empty(); }

std::size_t MaxHeap::top_k(std::size_t k, DynArray<PairScore>& out) const {
  out.clear();
  if (k == 0 || h_.empty()) {
    return 0;
  }

  MaxHeap copy;
  copy.h_ = h_;
  std::size_t count = (k < copy.size()) ? k : copy.size();
  out.reserve(count);
  for (std::size_t i = 0; i < count; ++i) {
    out.push_back(copy.pop());
  }
  return out.size();
}

void MaxHeap::sift_up(std::size_t i) {
  while (i > 0) {
    std::size_t parent = (i - 1) / 2;
    if (higher_priority(h_[i], h_[parent])) {
      PairScore tmp = h_[i];
      h_[i] = h_[parent];
      h_[parent] = tmp;
      i = parent;
    } else {
      break;
    }
  }
}

void MaxHeap::sift_down(std::size_t i) {
  std::size_t n = h_.size();
  while (true) {
    std::size_t left = 2 * i + 1;
    std::size_t right = 2 * i + 2;
    std::size_t best = i;

    if (left < n && higher_priority(h_[left], h_[best])) {
      best = left;
    }
    if (right < n && higher_priority(h_[right], h_[best])) {
      best = right;
    }
    if (best != i) {
      PairScore tmp = h_[i];
      h_[i] = h_[best];
      h_[best] = tmp;
      i = best;
    } else {
      break;
    }
  }
}

}  // namespace plag
