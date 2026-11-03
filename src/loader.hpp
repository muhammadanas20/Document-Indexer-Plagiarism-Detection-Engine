// loader.hpp  --  reading the corpus folder
// (Week 3: linked lists/containers, Week 9: stack, Week 6: quick sort)
// Owner: Anas
//
// What it does:
//   1. walks the folder and all its sub-folders with a STACK (iterative DFS),
//   2. keeps the files with a supported extension (.cpp .h .hpp .c .cc .txt .md),
//   3. quick-sorts the file list so the document numbers are the same every run,
//   4. reads every file, tokenizes it and builds a Document.

#ifndef DIP_LOADER_HPP
#define DIP_LOADER_HPP

#include <string>
#include <vector>

#include "document.hpp"

namespace dip {

// Returns false and fills `error` when the folder cannot be opened or is empty.
bool loadCorpus(const std::string& folder, std::vector<Document>& documents,
                std::string& error);

// True for .cpp .h .hpp .c .cc (comments are stripped) - used by the loader.
bool isSourceCodeFile(const std::string& path);

// The extensions we read. Kept here so the guide and the code agree.
bool isSupportedFile(const std::string& path);

}  // namespace dip

#endif  // DIP_LOADER_HPP
