// Implementation of directory traversal and path sorting
#include "index/dir_walker.hpp"

#include <cstring>
#include <filesystem>
#include <string>
#include <system_error>

namespace plag {
namespace {

bool is_supported_extension(const std::string& ext) {
  return ext == ".txt" || ext == ".cpp" || ext == ".hpp" ||
         ext == ".h" || ext == ".cc" || ext == ".c";
}

bool should_skip_dir(const std::string& name, const WalkOptions& opts) {
  if (opts.skip_hidden && !name.empty() && name[0] == '.') {
    return true;
  }
  if (opts.skip_vcs) {
    if (name == ".git" || name == "build" || name == "out") {
      return true;
    }
  }
  return false;
}

char* duplicate_str(const std::string& s) {
  char* copy = new char[s.size() + 1];
  std::memcpy(copy, s.c_str(), s.size() + 1);
  return copy;
}

void walk_recursive(const std::filesystem::path& dir, unsigned depth,
                    const WalkOptions& opts, DynArray<char*>& out_paths) {
  if (depth > opts.max_depth) return;

  std::error_code ec;
  std::filesystem::directory_iterator it(dir, ec);
  if (ec) return;

  for (const auto& entry : it) {
    std::string name = entry.path().filename().string();
    if (opts.skip_hidden && !name.empty() && name[0] == '.') {
      continue;
    }

    std::error_code type_ec;
    if (entry.is_symlink(type_ec)) {
      continue;
    }

    if (entry.is_directory(type_ec)) {
      if (!should_skip_dir(name, opts)) {
        walk_recursive(entry.path(), depth + 1, opts, out_paths);
      }
    } else if (entry.is_regular_file(type_ec)) {
      std::string ext = entry.path().extension().string();
      if (is_supported_extension(ext)) {
        out_paths.push_back(duplicate_str(entry.path().string()));
      }
    }
  }
}

// Simple insertion sort to order paths alphabetically without std::sort
void sort_paths(DynArray<char*>& paths) {
  for (std::size_t i = 1; i < paths.size(); ++i) {
    char* key = paths[i];
    std::size_t j = i;
    while (j > 0 && std::strcmp(paths[j - 1], key) > 0) {
      paths[j] = paths[j - 1];
      --j;
    }
    paths[j] = key;
  }
}

}  // namespace

ErrorCode walk_dir(const char* root, DynArray<char*>& out_paths,
                   const WalkOptions& opts) {
  free_walk_paths(out_paths);
  if (root == nullptr || root[0] == '\0') {
    return ErrorCode::BadArg;
  }

  std::error_code ec;
  std::filesystem::file_status st = std::filesystem::status(root, ec);
  if (ec || !std::filesystem::exists(st)) {
    return ErrorCode::NotFound;
  }
  if (!std::filesystem::is_directory(st)) {
    return ErrorCode::BadArg;
  }

  walk_recursive(std::filesystem::path(root), 0, opts, out_paths);
  sort_paths(out_paths);
  return ErrorCode::Ok;
}

void free_walk_paths(DynArray<char*>& paths) {
  for (std::size_t i = 0; i < paths.size(); ++i) {
    delete[] paths[i];
  }
  paths.clear();
}

}  // namespace plag
