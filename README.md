# Document Indexer and Similarity Checker

This C++ program reads text and source files, builds a word index, and compares
files for similar text. A match is a reason to check the files, not proof that
someone copied.

## Build and run

You need a C++17 compiler and CMake on Linux, WSL, or macOS.

```sh
cmake -S . -B build
cmake --build build
./build/plag-detect samples/students
ctest --test-dir build --output-on-failure
```

You can also give another folder instead of `samples/students`. The program
reads `.cpp`, `.h`, `.hpp`, `.c`, `.cc`, `.txt`, and `.md` files in that folder
and its subfolders. Files over 2 MB are skipped.

## Where to start

| File | What is inside |
| --- | --- |
| `src/main.cpp` | menu and user input |
| `src/documents.hpp` and `src/documents.cpp` | read files, split text into words, keep line numbers |
| `src/engine.hpp` and `src/engine.cpp` | word index, fingerprints, and similarity scores |
| `src/algorithms.hpp` and `src/algorithms.cpp` | sorting, binary search, and text search |
| `src/structures.hpp` | linked list, stack, queue, and graph |
| `src/tables.hpp` | hash tables |
| `src/trees.hpp` | trees and heap |
| `src/report.hpp` and `src/report.cpp` | print results |
| `tests/` | checks for each part of the program |
| `samples/students/` | sample input files |

The old Word report was written before this cleanup, so its file names and
implementation details may no longer match the code.

## How it works

1. The loader finds the files. The tokenizer makes words lowercase and keeps
   the line number of each word. It skips comments in source files.
2. The index stores each word in a hash table with the files that contain it.
3. For each file, the program takes groups of five words. It makes a number
   (fingerprint) for each group and keeps each number once.
4. To compare two files, it counts their shared fingerprints. The score is
   `shared / (first + second - shared)`. A higher score means more overlap.
5. For pairs above the limit, KMP looks for matching word groups and the
   program shows their line ranges. You can also search words or phrases from
   the menu and view rankings or groups of similar files.

The fingerprint code makes each group from the start, rather than using a
rolling hash. This is easier to follow, but takes more work for large groups.
The program still includes the other data structures and search algorithms in
the menu; use the file list above to find them.

## Limits

- This compares word sequences, not meaning. Renamed words or reordered text
  can lower the score. Similar code can also occur by chance.
- Fingerprint hashes can collide. Check the displayed matching lines yourself.
- Files with fewer than five words have no fingerprints. Matching line ranges
  use groups of six words, so a score may have no shown range.
- Source comment removal is basic and does not fully parse C++ strings.
- The scanner reads plain text only, not PDF or Word files.
