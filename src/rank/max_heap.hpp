// max_heap.hpp -- top-k leaderboard heap.
// Owner: ALYAN.  src/rank/ is Alyan's area.
// STUB (Alyan, W3): dummy bodies.
#ifndef PLAG_SRC_RANK_MAX_HEAP_HPP_
#define PLAG_SRC_RANK_MAX_HEAP_HPP_

#include <cstddef>

#include "common/dyn_array.hpp"
#include "contract/contract.hpp"

namespace plag {

// Binary max-heap over PairScore, ordered by suspicion_key(). No
// std::priority_queue and no std::sort -- banned in engine code.
class MaxHeap {
 public:
  void push(const PairScore& p);
  PairScore pop();
  const PairScore& top() const;
  std::size_t size() const;
  bool empty() const;

  // The k highest-scoring pairs, in descending order.
  std::size_t top_k(std::size_t k, DynArray<PairScore>& out) const;

 private:
  void sift_up(std::size_t i);
  void sift_down(std::size_t i);

  DynArray<PairScore> h_;
};

}  // namespace plag

#endif  // PLAG_SRC_RANK_MAX_HEAP_HPP_
