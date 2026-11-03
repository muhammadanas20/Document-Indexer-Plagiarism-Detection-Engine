# Shared Viva Guide — both members

Both of you must know this file. The per-member files (`viva-guide-anas.md`,
`viva-guide-alyan.md`) are for the deep questions.

---

## 1. The project in five lines

1. Read a folder of text/code files and cut them into **words with line numbers**.
2. Store every word in an **inverted index** (hash table) so a word can be found in O(1).
3. Turn every document into a set of **k-word fingerprints** (rolling hash).
4. Two documents are compared by **how many fingerprints they share** (Jaccard score).
5. The suspicious pairs are **proved with KMP** (which lines?), **ranked** (merge sort / heap /
   AVL tree) and **grouped** (graph + BFS).

## 2. Build, run, test (memorise these three commands)

```bash
cmake -S . -B build && cmake --build build      # build
./build/plag-detect samples/students            # run
ctest --test-dir build --output-on-failure      # test  ->  100% tests passed out of 6
```

## 3. The demo script (5 minutes, rehearse it once)

| Step | What you type | What you say |
|---|---|---|
| 1 | `./build/plag-detect samples/students` | "It reads the folder, walks it with a stack, and builds the index in about 1 ms." |
| 2 | `2` | "523 words, 102 distinct words. Load factor 0.398, so the chains are short. Average chain length is 0.4 — the hash function spreads the words. The BST height is 15 where a balanced tree would be 7, and that is exactly why the leaderboard uses the AVL tree." |
| 3 | `3` then `total` | "One hash lookup, average O(1). It answers with the documents, the line numbers and the counts." |
| 4 | `4` then `int numbers int count` | "The same phrase with three algorithms — brute force, KMP, Rabin-Karp. Same matches, and this is how we check them against each other." |
| 5 | `5` then `n` | "This is the result: faisal is a verbatim copy (1.000 CRITICAL), bilal renamed the functions (0.444 HIGH), hamza copied half (0.302 MEDIUM), sara and zainab are not reported. The line ranges come from KMP." |
| 6 | `6` then `2` | "Top 2 from the max heap — we do not sort all pairs." |
| 7 | `7` | "The whole leaderboard from the AVL tree, already sorted by the traversal. Height 3 for 6 nodes." |
| 8 | `8` then `0.35` | "A graph where an edge means 'similar'. BFS gives the cluster: ali, bilal and faisal worked together." |
| 9 | `9` then `sum` | "Vocabulary in alphabetical order from the BST in-order traversal, and binary search finds a word in O(log n)." |
| 10 | `0` | Stop. |

## 4. Data structure → where → why (the table they will ask for)

| Data structure | File | Used for | Why this one | Complexity |
|---|---|---|---|---|
| Singly linked list | `list.hpp` | hash buckets, posting lists, adjacency lists | grows without moving data, O(1) insert | O(1) insert, O(n) search |
| Stack | `stack.hpp` | walking the folders | iterative DFS, no call-stack overflow | O(1) push/pop |
| Queue | `queue.hpp` | BFS in the graph | first in first out = level by level | O(1) push/pop |
| Hash table (chaining) | `hash_table.hpp` | inverted index, fingerprint sets | O(1) lookup, and deletion is O(1) | O(1) average, O(n) worst |
| Hash table (linear probing) | `probing.hpp` | counting words of one document | no deletion needed, no pointers | O(1) average |
| BST | `bst.hpp` | vocabulary (sorted + search) | in-order traversal = alphabetical | O(log n) average, O(n) worst |
| AVL tree | `avl.hpp` | leaderboard of pairs | guaranteed O(log n) height | O(log n) |
| Max heap | `heap.hpp` | top-K pairs | only the K biggest are needed | O(log n) push/pop, O(n + k log n) top-K |
| Graph (adjacency list) | `graph.hpp` | similarity graph, clusters | sparse graph → O(V+E) memory | BFS O(V+E) |
| Merge sort | `sort.cpp` | fingerprints, pair list | O(n log n) always, stable | O(n log n) |
| Quick sort | `sort.cpp` | file paths | fast in place | O(n log n) average, O(n²) worst |
| Insertion sort | `sort.cpp` | the top-K list (K ≈ 10) | no overhead on tiny lists | O(k²) |
| Binary search | `sort.cpp`, `document.hpp` | vocabulary lookup, char position → word | sorted data is available | O(log n) |
| Brute force search | `string_match.cpp` | phrase search (baseline) | simple, used to verify the other two | O(n·m) |
| KMP | `string_match.cpp` | copied-line proof, phrase search | no backtracking, O(n+m) | O(n+m) |
| Rabin-Karp | `string_match.cpp` | phrase search, idea behind fingerprints | rolling hash, O(1) per window | O(n+m) average |

## 5. Division of work (say it the same way)

* **Anas** — everything that turns files into data: `list.hpp`, `stack.hpp`, `hash_table.hpp`,
  `probing.hpp`, `sort.*`, `fingerprint.*`, `tokenizer.*`, `document.hpp`, `loader.*`,
  `index.*`. (reading, tokenising, indexing, hashing, sorting, rolling hash)
* **Alyan** — everything that uses the data to find plagiarism: `queue.hpp`, `bst.hpp`,
  `avl.hpp`, `heap.hpp`, `graph.hpp`, `string_match.*`, `similarity.*`, `report.*`,
  `main.cpp`. (matching, scoring, ranking, clusters, output)

Two interfaces connect the two halves, and both of you should be able to point at them:

1. `Index::fingerprints()` — Anas produces the sorted fingerprint lists; Alyan compares them.
2. `dip::WordTree` (`bst.hpp`) — Alyan's BST class; Anas's `index.cpp` inserts the words.

## 6. Questions that can be asked to either of you

**Q. What exactly is plagiarism detection here — do you compare the whole file?**
> "No. We compare the **set of k-word fingerprints**. A shared set of fingerprints means shared
> sentences. Then KMP finds where those shared sentences are, so we can report line numbers."

**Q. Why k = 5 words and not 1 word or 20 words?**
> "One word is meaningless — everybody writes `int` and `return`. Twenty words never match unless
> almost the whole file is copied. Five is long enough to be a real phrase and short enough to
> catch a copied sentence. Small k → more false positives, big k → more false negatives."

**Q. What happens if two students copy but reorder the sentences?**
> "We miss it. A k-gram is consecutive words, so reordering breaks every fingerprint. Our
> limitation list says this, and the fix would be to compare sentence sets instead of windows."

**Q. What if a student renames every variable?**
> "The score drops but does not vanish. In our sample, `bilal.cpp` is `ali.cpp` with renamed
> functions: 0.444 instead of 1.000, still HIGH, and KMP still finds the copied blocks. If he
> also rewrote the structure, we would lose him — a token-level tool always has this limit."

**Q. Why did you not use `std::unordered_map` / `std::sort`?**
> "Because the course is about building these structures. If we use the STL we are not
> demonstrating anything. We only use `std::string` and `std::vector` as plain text/array
> containers, and `std::ifstream` to read files."

**Q. Where is the memory management? Any leaks?**
> "Every class allocates in its constructor or in `push` and frees in its destructor:
> `~LinkedList`, `~HashTable`, `~LinearProbeTable`, `~WordTree`, `~Leaderboard`, `~Graph`,
> `~Index`. The copy constructor and assignment are `= delete`d (Rule of Three), so a careless
> copy cannot double-free. The `Index` object lives in a `std::unique_ptr` in `main.cpp`."

**Q. How is the program's answer reproducible?**
> "File paths are quick-sorted before reading, so document numbers are stable. All thresholds
> are constants in the code (`kMinScore`, the label thresholds, k = 5). Same folder → same
> output."

**Q. What is the total complexity for a corpus of N words in D documents?**
> "Index: O(N). Fingerprints: O(N). Pairs: O(D²) pairs, but each pair is one walk over sorted
> fingerprint lists, and pairs that share no fingerprint stop immediately. KMP proof: only for
> surviving pairs, O(n+m) each. Ranking: O(P log P). Memory: O(N + P)."

**Q. What would break your program?**
> "A 3 GB file (we skip files above 2 MB), a folder with no text files (we print an error), a
> document shorter than k words (it gets no fingerprints, so it is never suspicious), and a
> copied idea with different code (we only compare words)."

**Q. Which topic of the course was the hardest to use here?**
> "The AVL rotations, because the height must be updated bottom-up while you return from the
> recursion, and one wrong order of `updateHeight` gives a silently unbalanced tree. The test
> that inserts 1 000 sorted scores is what proves the rotations work."

## 7. Division of speaking in the viva

* Opening (1 min): **Anas** — what the project does and the pipeline.
* Demo (5 min): each one drives the options of his own half (Anas: 1, 2, 3, 5-part-1;
  Alyan: 4, 5-result, 6, 7, 8, 9).
* Code questions: whoever's file it is. **Never answer for the other person** — say
  "that is Anas's `hash_table.hpp`, he will explain" if it is not yours, and then listen.
* Closing (1 min): **Alyan** — results, limitations, what we would add.

## 8. If you are stuck

1. Repeat the question in your own words ("you are asking how the score is computed").
2. Say what you *do* know, then what you are unsure about. Never invent a number.
3. Point at the code and read the 3 lines that answer it. The code is simple on purpose — that
   is the point of this project.
4. "I do not remember the exact value, but I can show you the line where it is computed."
