// dir_walker.hpp -- list every indexable file under a root, reproducibly.
// Owner: Anas.  src/index/ is Anas's area.
// STUB (W1 / Step 2): dummy body returns an empty list. Replace at Step 2.
#ifndef PLAG_SRC_INDEX_DIR_WALKER_HPP_
#define PLAG_SRC_INDEX_DIR_WALKER_HPP_

#include "common/dyn_array.hpp"
#include "contract/error_code.hpp"

namespace plag {

struct WalkOptions {
  unsigned max_depth = 32;        // guard against symlink loops
  bool skip_hidden = true;        // leading '.' names
  bool skip_vcs = true;           // .git, build/, out/
};

// Recursive walk, extension whitelist (.txt, .cpp, .hpp, .h, .cc), output
// sorted by path so two runs give byte-identical results.
ErrorCode walk_dir(const char* root, DynArray<char*>& out_paths,
                   const WalkOptions& opts = WalkOptions{});

}  // namespace plag

#endif  // PLAG_SRC_INDEX_DIR_WALKER_HPP_
