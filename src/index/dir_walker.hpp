// Directory walker that collects source and text files in sorted order
#ifndef PLAG_SRC_INDEX_DIR_WALKER_HPP_
#define PLAG_SRC_INDEX_DIR_WALKER_HPP_

#include "common/dyn_array.hpp"
#include "contract/error_code.hpp"

namespace plag {

struct WalkOptions {
  unsigned max_depth = 32;
  bool skip_hidden = true;
  bool skip_vcs = true;
};

ErrorCode walk_dir(const char* root, DynArray<char*>& out_paths,
                   const WalkOptions& opts = WalkOptions{});

void free_walk_paths(DynArray<char*>& paths);

}  // namespace plag

#endif  // PLAG_SRC_INDEX_DIR_WALKER_HPP_
