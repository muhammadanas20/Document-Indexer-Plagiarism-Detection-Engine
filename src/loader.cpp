// loader.cpp  --  see loader.hpp
// Owner: Anas

#include "loader.hpp"

#include <dirent.h>
#include <sys/stat.h>

#include <cstddef>
#include <fstream>
#include <sstream>

#include "sort.hpp"
#include "stack.hpp"
#include "tokenizer.hpp"

namespace dip {
namespace {

const std::size_t kMaxFileBytes = 2 * 1024 * 1024;  // ignore files bigger than 2 MB

std::string extensionOf(const std::string& path) {
  std::size_t dot = path.find_last_of('.');
  std::size_t slash = path.find_last_of('/');
  if (dot == std::string::npos) return "";
  if (slash != std::string::npos && dot < slash) return "";  // dot is in a folder name
  return path.substr(dot);
}

// Read the whole file into one string. Returns false if it cannot be opened.
bool readWholeFile(const std::string& path, std::string& content) {
  std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
  if (!file.is_open()) return false;

  std::ostringstream buffer;
  buffer << file.rdbuf();
  content = buffer.str();
  return true;
}

}  // namespace

bool isSourceCodeFile(const std::string& path) {
  std::string ext = extensionOf(path);
  return ext == ".cpp" || ext == ".hpp" || ext == ".h" || ext == ".c" || ext == ".cc";
}

bool isSupportedFile(const std::string& path) {
  if (isSourceCodeFile(path)) return true;
  std::string ext = extensionOf(path);
  return ext == ".txt" || ext == ".md";
}

bool loadCorpus(const std::string& folder, std::vector<Document>& documents,
                std::string& error) {
  documents.clear();
  error.clear();

  std::vector<std::string> files;

  // ---- 1. walk the folder with a stack (depth-first, no recursion) --------
  Stack<std::string> folders;
  folders.push(folder);

  while (!folders.empty()) {
    std::string current = folders.pop();
    DIR* dir = opendir(current.c_str());
    if (dir == nullptr) {
      if (current == folder) {
        error = "cannot open folder: " + folder;
        return false;
      }
      continue;  // a sub-folder we are not allowed to read: just skip it
    }

    struct dirent* entry = nullptr;
    while ((entry = readdir(dir)) != nullptr) {
      std::string name = entry->d_name;
      if (name == "." || name == "..") continue;
      if (name[0] == '.') continue;  // hidden files, .git, ...
      if (name == "build") continue; // never index our own build output

      std::string fullPath = current + "/" + name;
      struct stat info;
      if (stat(fullPath.c_str(), &info) != 0) continue;

      if (S_ISDIR(info.st_mode)) {
        folders.push(fullPath);
      } else if (S_ISREG(info.st_mode) && isSupportedFile(fullPath)) {
        if (static_cast<std::size_t>(info.st_size) <= kMaxFileBytes) {
          files.push_back(fullPath);
        }
      }
    }
    closedir(dir);
  }

  if (files.empty()) {
    error = "no .cpp/.h/.txt files found in: " + folder;
    return false;
  }

  // ---- 2. sort the file list so the output is always the same -------------
  quickSortStrings(files);

  // ---- 3. read and tokenize every file ------------------------------------
  for (std::size_t i = 0; i < files.size(); ++i) {
    std::string content;
    if (!readWholeFile(files[i], content)) continue;

    bool stripComments = isSourceCodeFile(files[i]);
    TokenizedText tokens = tokenize(content, stripComments);
    if (tokens.words.empty()) continue;  // empty file / only comments

    Document document;
    document.path = files[i];
    document.words = tokens.words;
    document.wordLines = tokens.lines;
    document.buildNormalized();
    documents.push_back(document);
  }

  if (documents.empty()) {
    error = "the folder has files but all of them are empty";
    return false;
  }
  return true;
}

}  // namespace dip
