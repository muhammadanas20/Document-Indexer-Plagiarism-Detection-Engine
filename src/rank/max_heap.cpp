// max_heap.cpp -- STUB (Alyan, W3).
#include "rank/max_heap.hpp"

#include "search/scorer.hpp"

namespace plag {

void MaxHeap::push(const PairScore& /*p*/) {
  // TODO(Alyan W3): append + sift_up.
}

PairScore MaxHeap::pop() {
  // TODO(Alyan W3): swap root with last, shrink, sift_down.
  return PairScore{};  // STUB dummy
}

const PairScore& MaxHeap::top() const {
  static const PairScore kEmpty{};
  return h_.empty() ? kEmpty : h_[0];
}

std::size_t MaxHeap::size() const { return h_.size(); }
bool MaxHeap::empty() const { return h_.empty(); }

std::size_t MaxHeap::top_k(std::size_t /*k*/, DynArray<PairScore>& /*out*/) const {
  return 0;  // STUB dummy
}

void MaxHeap::sift_up(std::size_t /*i*/) {}
void MaxHeap::sift_down(std::size_t /*i*/) {}

}  // namespace plag
