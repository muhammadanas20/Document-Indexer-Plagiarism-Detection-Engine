// Binary max-heap for extracting top-k most suspicious document pairs
#ifndef PLAG_SRC_RANK_MAX_HEAP_HPP_
#define PLAG_SRC_RANK_MAX_HEAP_HPP_

#include <cstddef>

#include "common/dyn_array.hpp"
#include "contract/contract.hpp"

namespace plag {

class MaxHeap {
 public:
  void push(const PairScore& p);
  PairScore pop();
  const PairScore& top() const;
  std::size_t size() const;
  bool empty() const;

  std::size_t top_k(std::size_t k, DynArray<PairScore>& out) const;

 private:
  void sift_up(std::size_t i);
  void sift_down(std::size_t i);

  DynArray<PairScore> h_;
};

}  // namespace plag

#endif  // PLAG_SRC_RANK_MAX_HEAP_HPP_
