# plag-engine

**Document Indexer & Plagiarism Detection Engine** — C++17, no STL containers in engine code.

`plag-detect` reads a corpus of `.txt` / `.cpp` files, breaks each file into tokens, rolls a
Rabin–Karp fingerprint over every window of *k* tokens, stores `fingerprint → every place it
occurs` in a hash table written from scratch, then verifies the candidate pairs with KMP over
token arrays, scores them (Jaccard / containment / coverage), ranks them with a top-*k* heap
and an AVL leaderboard, and writes an audit report with line numbers. Every container in the
engine — the dynamic array, the hash table, the heap, the AVL tree — is hand-written; the STL
appears only in `tests/`, where it serves as the oracle.

**Team:** Anas (group leader — corpus, indexing, core data structures, the shared build) and
Alyan (matching, scoring, ranking, CLI, report). Two people, one owner per file.

---

## The pipeline

```
        ┌────────────────────────── ANAS ──────────────────────────┐
        │                                                          │
 .txt   │  dir_walker ──► file_reader ──► DynamicBuffer            │
 .cpp ──┤                       │                                  │
        │                       ▼                                  │
        │                 tokenizer ──► TokenPool {off,len,id,line}
        │                       │        (exact + normalised views)
        │                       ▼                                  │
        │                  rolling_hash  (Rabin-Karp, O(1) roll)   │
        │                       │                                  │
        │                       ▼                                  │
        │HashTable<u64, OccurrenceList>◄── chained, grown at α=0.75
        │        (open-addressing variant benchmarked alongside)   │
        │                       │                                  │
        │                       ▼                                  │
        │              IndexBuilder ──► lookup(h, &count)          │
        │                       │       DocumentView{norm,posToLine}
        │                       │        IndexStats  (--stats)     │
        └───────────────────────┼──────────────────────────────────┘
                                │  the frozen API, due Sun 8 Nov
        ┌───────────────────────┼────────────── ALYAN ─────────────┐
        │                       ▼                                  │
        │            candidate_generator ──► (docA,docB) pairs     │
        │                       │                                  │
        │                       ▼                                  │
        │                 kmp_find_all ──► exact token matches     │
        │                       │                                  │
        │                       ▼                                  │
        │        span_merge (Anas) ──► disjoint maximal spans      │
        │                       │                                  │
        │                       ▼                                  │
        │          scorer ──► jaccard / containment / coverage     │
        │                       │                                  │
        │                       ▼                                  │
        │        MaxHeap (top-k)  +  RankTree (AVL leaderboard)    │
        │                       │                                  │
        │                       ▼                                  │
        │        cli  ──►  audit_report / json_writer  ──►  r.txt  │
        └──────────────────────────────────────────────────────────┘
```

## Build and run

```bash
cmake --preset debug                     # configures into build/
cmake --build build                      # builds plag-detect + every test
ctest --test-dir build --output-on-failure   # or: cmake --build build --target check

./build/bin/plag-detect --version        # plag-detect 0.1 (stub)
./build/bin/plag-detect index tests/corpus/anas/small --stats   # from Step 6
```

Sanitizer build — must be green before anything is pushed:

```bash
cmake --preset sanitize
cmake --build build-sanitize --target check
```

Presets: `debug` (`build/`), `sanitize` (`build-sanitize/`), `release` (`build-release/`).

## Layout and ownership

One owner per file. If a file is not in your path you do not edit it — you open an issue and
the owner adds the hook, or you add your own file next to it.

| Path | What lives here | Owner |
|---|---|---|
| `CMakeLists.txt`, `.gitignore`, `.clang-format`, `.gitattributes`, `CMakePresets.json`, `README.md` | repo root, frozen after Week 0/1 | Anas |
| `.github/workflows/ci.yml` | `make check` on push and PR | Anas |
| `src/main.cpp` | the `plag-detect` entry point | Anas |
| `src/contract/` | shared types only — frozen after W1 | Anas |
| `src/common/` | `dyn_array`, `fnv`, `mulmod`, `pos_to_line` — frozen after W1 | Anas |
| `src/index/` | buffer, reader, walker, tokenizer, rolling hash, hash tables, index builder, span merge | **Anas** |
| `src/search/` | KMP, candidate generator, scorer, verify mode | **Alyan** |
| `src/rank/` | max-heap, AVL leaderboard | **Alyan** |
| `src/report/` | CLI, audit report, JSON writer | **Alyan** |
| `tests/harness.hpp`, `tests/CMakeLists.txt` | shared test framework, frozen after W1 | Anas creates, Alyan maintains |
| `tests/test_anas_*.cpp`, `tests/fixtures/anas/`, `tests/corpus/anas/` | Anas's tests and corpora | Anas |
| `tests/test_alyan_*.cpp`, `tests/fixtures/alyan/`, `tests/corpus/alyan/` | Alyan's tests and corpora | Alyan |
| `tools/<name>_*` | benchmarks and generators, prefixed by owner | each |
| `docs/` | design, per-person devlogs and benchmarks, the shared task list | shared |

Every area's `CMakeLists.txt` picks sources up with `file(GLOB ...)`, so adding a new `.cpp`
never touches a shared file. That single decision removes most of the merge friction.

## The six rules

1. **No STL shortcuts in engine code.** `std::unordered_map`, `std::map`, `std::set`,
   `std::priority_queue` and `std::sort` are banned under `src/`. `std::string` and file
   streams are fine, and using an STL container in `tests/` as a comparison oracle is
   encouraged.
2. **One owner per file.**
3. **Everything gets a test.** A step without a test you wrote is not finished.
4. **Stub first.** Commit the header with final signatures and empty bodies on day one of a
   step, so the other person never waits.
5. **Clean build only.** `-Wall -Wextra -Wconversion` with zero warnings, and `make check`
   green in the sanitizer build.
6. **Small PRs.** One step = one branch = one PR under ~400 changed lines.

## Working rhythm

* `main` is protected: PR required, CI green required, no force pushes, no deletions.
* `dev` takes urgent work. Each Sunday demo merges to `main` and is tagged `W<n>-demo`.
* One branch per person per week, named in `docs/tasks.md` (e.g. `week03-hash-table`), started
  from the newest `main`.
* At least 3 commits a week (Mon / Wed / Fri) — the graph is the proof of work.
* Commit style: `area: what changed (Who W#)` — e.g.
  `index: chaining hash table with grow at alpha 0.75 (Anas W3)`.
* Merge with **Squash and merge**, delete the branch, rebase with
  `git fetch upstream && git rebase upstream/main`, push with `--force-with-lease`.

## Status

Week 0 skeleton: the full layout, the frozen build, the shared contract, the working
`src/common/` pieces and their tests, and stub headers with final signatures for every class
in both areas. Feature implementations land week by week per `docs/tasks.md`.
