# Document Indexer & Plagiarism Detection Engine

A C++17 console project for the **Data Structures Lab (CL-2001)**. It reads a folder of
documents, builds an index of every word, and finds the pairs of documents that copied
from each other — and it shows the exact line numbers.

Every data structure the course asks for is written by us in simple C++ (no
`std::map`, no `std::unordered_map`, no `std::sort`, no `std::list`). `std::string` and
`std::vector` are used only as plain containers for text and results.

---

## 1. What the program does

```
 folder of files
        |
        v
 [1] LOADER         stack (folder walk) + quick sort (file order)
        |           reads .cpp .h .txt, removes comments, lower-cases the words
        v
 [2] TOKENIZER      "int Sum(int a)"  ->  int sum int a      (line numbers kept)
        |
        v
 [3] INDEX          hash table (separate chaining):  word -> linked list of
        |                                            (document, line, count)
        |           BST: the vocabulary in alphabetical order
        |           linear-probing table: words of one document (statistics)
        |           rolling hash: the k-gram fingerprints of every document
        v
 [4] SIMILARITY     merge two sorted fingerprint lists -> Jaccard score
        |           KMP: where exactly is the copied text?  -> line ranges
        v
 [5] RANKING        merge sort (all pairs) / max heap (top K) / AVL tree (leaderboard)
        |           graph + BFS -> clusters of students who copied together
        v
 [6] REPORT         table of suspicious pairs + copied line ranges
```

## 2. Build and run

```bash
# needs g++ and cmake (Linux / WSL / macOS)
cmake -S . -B build
cmake --build build

# run the program on the sample folder
./build/plag-detect samples/students
```

Then pick a number from the menu:

```
 1) Load a folder and build the index
 2) Show index statistics
 3) Search a word            (hash table)
 4) Search a phrase          (brute force / KMP / Rabin-Karp)
 5) Detect plagiarism        (k-gram fingerprints + KMP)
 6) Show top-K suspicious pairs (max heap)
 7) Show the full leaderboard   (AVL tree)
 8) Show plagiarism clusters    (graph + BFS)
 9) Show the vocabulary      (BST + binary search)
 0) Exit
```

Run the tests:

```bash
ctest --test-dir build --output-on-failure
```

Expected result: `100% tests passed out of 6`.

## 3. The sample folder

`samples/students/` is a small fake class. It is built so the program has something
clear to show:

| file | what it is | what the program must say |
|---|---|---|
| `ali.cpp` | the original | — |
| `faisal.cpp` | verbatim copy of `ali.cpp` | **CRITICAL** (score 1.000) |
| `bilal.cpp` | copy with the functions renamed | **HIGH** (score 0.444) |
| `hamza.cpp` | half copy, own `main()` | **MEDIUM** (score 0.302) |
| `sara.cpp` | own linked-list program | not reported |
| `zainab.txt` | own notes on bubble sort | not reported |

Real output of option 5 on that folder:

```
SUSPICIOUS PAIRS: 6 pair(s) above the score limit
#   PAIR                              SCORE   CONTAINMENT  WORDS    VERDICT
1   ali.cpp / faisal.cpp              1.000   1.000        98       CRITICAL
        copied: ali.cpp lines 1-32  ==  faisal.cpp lines 1-32   (98 words, found with KMP)
2   ali.cpp / bilal.cpp               0.444   0.615        76       HIGH
        copied: ali.cpp lines 4-12  ==  bilal.cpp lines 7-15   (21 words, found with KMP)
        copied: ali.cpp lines 17-29 ==  bilal.cpp lines 20-32  (36 words, found with KMP)
4   ali.cpp / hamza.cpp               0.302   0.506        49       MEDIUM
        copied: ali.cpp lines 1-17  ==  hamza.cpp lines 1-17   (43 words, found with KMP)
```

## 4. Files

```
CMakeLists.txt          build file (one library + one program + one test per file)
src/
  list.hpp              singly linked list            (chaining, postings, adjacency)
  stack.hpp             stack                          (folder walk)
  queue.hpp             queue                          (BFS)
  hash_table.hpp        hash table, separate chaining   (inverted index)
  probing.hpp           hash table, linear probing     (word counts, comparison)
  bst.hpp               binary search tree             (vocabulary)
  avl.hpp               AVL tree + rotations           (leaderboard)
  heap.hpp              max heap / priority queue      (top-K pairs)
  graph.hpp             graph + BFS                    (clusters)
  sort.hpp / .cpp       merge sort, quick sort, insertion sort, binary search
  string_match.hpp/.cpp brute force, KMP, Rabin-Karp
  fingerprint.hpp/.cpp  rolling hash -> k-gram fingerprints
  tokenizer.hpp/.cpp    text -> words + line numbers
  document.hpp          one loaded file
  loader.hpp/.cpp       folder walk + read + tokenize
  index.hpp/.cpp        the inverted index, vocabulary, fingerprints
  similarity.hpp/.cpp   Jaccard score + KMP proof (spans) + labels
  report.hpp/.cpp       all the printing
  main.cpp              the menu
tests/                  one test program per area (6 files, ~250 checks)
samples/students/       the demo corpus (6 files)
docs/
  design.md             the architecture and the formulas
  viva-guide-anas.md    member 1: what he built and the answers to the viva
  viva-guide-alyan.md   member 2: what he built and the answers to the viva
  viva-guide-shared.md  both members: demo script, complexity table, common questions
```

## 5. Who did what

| Member | Files | Course topics |
|---|---|---|
| **Anas** (leader) | `list.hpp`, `stack.hpp`, `hash_table.hpp`, `probing.hpp`, `sort.*`, `fingerprint.*`, `tokenizer.*`, `document.hpp`, `loader.*`, `index.*` | linked list, stack, hashing + collision resolution + rehashing, merge/quick/insertion sort, binary search, rolling hash, Rule of Three, pointers |
| **Alyan** | `queue.hpp`, `bst.hpp`, `avl.hpp`, `heap.hpp`, `graph.hpp`, `string_match.*`, `similarity.*`, `report.*`, `main.cpp` | queue, BST, AVL rotations, heap / priority queue, graph + BFS, brute force / KMP / Rabin-Karp, recursion |

`index.cpp` calls the BST that Alyan wrote (`dip::WordTree`), and `similarity.cpp` calls
the rolling hash that Anas wrote (`computeFingerprints`). That is the only place where
the two parts touch.

## 6. Course topics covered

| Week | Topic | Where it is used |
|---|---|---|
| 1 | ADT, pointers, Rule of Three | every class (`~LinkedList`, `~HashTable`, deleted copy) |
| 3 | Singly linked list | `list.hpp` → hash buckets, posting lists, adjacency lists |
| 4 | Recursion / backtracking | BST, AVL, merge sort, quick sort |
| 5 | Linear / binary / interpolation search | `binarySearchWord`, `wordIndexOfPosition` |
| 5-7 | Bubble → insertion, merge, quick sort | `sort.cpp` (merge sort on fingerprints and pairs, quick sort on paths) |
| 9 | Stack, queue | `loader.cpp` (stack), `graph.hpp` BFS (queue) |
| 10 | Binary tree, BST | `bst.hpp` (vocabulary) |
| 11 | AVL tree, rotations | `avl.hpp` (leaderboard) |
| 12 | String search: brute force, Rabin-Karp, KMP | `string_match.cpp` |
| 13 | Hashing, hash functions, collisions, rehashing | `hash_table.hpp`, `probing.hpp`, `fingerprint.cpp` |
| 13 | Priority queue / heap | `heap.hpp` (top-K) |
| 14 | Graph, representation, traversal | `graph.hpp` (similarity graph + BFS) |

## 7. Known limits (say this yourself in the viva)

1. It compares **words**, not meaning: a copy that renames every identifier scores
   lower (see `bilal.cpp`, 0.444 instead of 1.000).
2. It only sees **exact** word sequences of length k, so a copy that reorders every
   sentence is not caught.
3. Two documents are compared only if they share at least one k-gram — that is what
   makes it fast, and it is why a rewritten paragraph is missed.
4. It reads plain text files only (no PDF/DOCX) and skips files bigger than 2 MB.
