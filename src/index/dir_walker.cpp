// dir_walker.cpp -- STUB (Anas, W1 / Step 2).
#include "index/dir_walker.hpp"

namespace plag {

ErrorCode walk_dir(const char* /*root*/, DynArray<char*>& /*out_paths*/,
                   const WalkOptions& /*opts*/) {
  // TODO(Anas W1 Step 2): std::filesystem::recursive_directory_iterator,
  // extension whitelist, depth cap, sort by path.
  return ErrorCode::NotFound;  // STUB dummy: nothing is walked yet.
}

}  // namespace plag
