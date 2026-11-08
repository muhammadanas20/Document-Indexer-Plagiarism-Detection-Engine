#include "documents.hpp"
#include "structures.hpp"
#include "algorithms.hpp"
#include <cctype>
#include <cstddef>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <sstream>

// tokenizer
TokenizedText tokenize(const std::string& text, bool stripComments) {
  TokenizedText result;
  std::string currentWord;
  int line = 1;
  bool inBlockComment = false;

  for (std::size_t i = 0; i < text.size(); ++i) {
    char c = text[i];

    if (c == '\n') {

      ++line;
      if (!currentWord.empty()) {
        result.words.push_back(currentWord);
        result.lines.push_back(line - 1);
        currentWord.clear();
      }
      continue;
    }

    if (stripComments) {

      if (inBlockComment) {
        if (c == '*' && i + 1 < text.size() && text[i + 1] == '/') {
          inBlockComment = false;
          ++i;
        }
        continue;
      }
      if (c == '/' && i + 1 < text.size() && (text[i + 1] == '*' || text[i + 1] == '/')) {
        // a comment also ends the word before it
        if (!currentWord.empty()) {
          result.words.push_back(currentWord);
          result.lines.push_back(line);
          currentWord.clear();
        }
      }
      if (c == '/' && i + 1 < text.size() && text[i + 1] == '*') {
        inBlockComment = true;
        ++i;
        continue;
      }

      if (c == '/' && i + 1 < text.size() && text[i + 1] == '/') {
        while (i < text.size() && text[i] != '\n') ++i;
        if (i < text.size()) {
          --i;
        }
        continue;
      }
    }

    bool isWordChar = std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
    if (isWordChar) {
      currentWord.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    } else if (!currentWord.empty()) {
      result.words.push_back(currentWord);
      result.lines.push_back(line);
      currentWord.clear();
    }
  }

  if (!currentWord.empty()) {
    result.words.push_back(currentWord);
    result.lines.push_back(line);
  }
  return result;
}

// loader
const std::size_t kMaxFileBytes = 2 * 1024 * 1024;

std::string extensionOf(const std::string& path) {
  std::size_t dot = path.find_last_of('.');
  std::size_t slash = path.find_last_of('/');
  if (dot == std::string::npos) return "";
  if (slash != std::string::npos && dot < slash) return "";
  return path.substr(dot);
}

bool readWholeFile(const std::string& path, std::string& content) {
  std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
  if (!file.is_open()) return false;

  std::ostringstream buffer;
  buffer << file.rdbuf();
  content = buffer.str();
  return true;
}

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

  // use a stack to visit each folder
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
      continue;
    }

    struct dirent* entry = nullptr;
    while ((entry = readdir(dir)) != nullptr) {
      std::string name = entry->d_name;
      if (name == "." || name == "..") continue;
      if (name[0] == '.') continue;
      if (name == "build") continue;

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

  quickSortStrings(files);

  for (std::size_t i = 0; i < files.size(); ++i) {
    std::string content;
    if (!readWholeFile(files[i], content)) continue;

    bool stripComments = isSourceCodeFile(files[i]);
    TokenizedText tokens = tokenize(content, stripComments);
    if (tokens.words.empty()) continue;

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
