# Document Indexer & Plagiarism Detection Engine

A command-line document indexing and plagiarism detection engine written in C++17 for our Data Structures course project. All core containers (`DynArray`, `HashTable`, `OpenHashTable`, `MaxHeap`, and `RankTree` AVL tree) are implemented from scratch without using STL containers (`std::vector`, `std::unordered_map`, `std::map`, `std::priority_queue`, or `std::sort`) in engine code.

**Team Members:**
- **Anas:** Corpus reading (`DynamicBuffer`, `file_reader`, `dir_walker`), tokenizer, Rabin-Karp rolling hash, hash tables (separate chaining and open addressing), index builder, and span merging.
- **Alyan:** Candidate generation, KMP string matching over token arrays, similarity scoring (Jaccard, containment, coverage), Max-Heap, AVL leaderboard (`RankTree`), CLI, and audit/JSON reporting.

---

## How It Works

1. **File Reading & Tokenization (`src/index/`):**
   - Walks a corpus directory recursively for `.txt`, `.cpp`, `.hpp`, `.h`, `.cc`, and `.c` files in sorted order.
   - Reads file bytes into `DynamicBuffer`, strips comments and whitespace, and converts tokens into 64-bit FNV-1a token IDs (with optional case normalization) while tracking 1-based line numbers (`posToLine`).
2. **Rabin-Karp Rolling Hash & Indexing (`src/index/`):**
   - Slides a window of size `k` over each document's token ID stream in $O(1)$ time per step modulo $2^{61}-1$.
   - Stores `fingerprint -> OccurrenceList` in a custom separate-chaining `HashTable` that doubles capacity when the load factor exceeds $0.75$.
3. **Candidate Generation & KMP Verification (`src/search/`):**
   - Counts shared $k$-gram fingerprints between document pairs and filters pairs meeting `minShared`.
   - Verifies exact token matches between candidate pairs using the Knuth-Morris-Pratt (KMP) algorithm (`kmp_find_all`) and merges adjacent/overlapping matches into maximal spans (`merge_spans`).
4. **Scoring, Ranking & Reporting (`src/search/`, `src/rank/`, `src/report/`):**
   - Computes **Jaccard**, **Containment**, and **Coverage** similarity scores and assigns a suspicion label (`none`, `low`, `medium`, `high`, `critical`).
   - Ranks pairs using a custom `MaxHeap` and self-balancing AVL tree (`RankTree`).
   - Outputs either a human-readable text audit report (`AuditReport`) with line numbers or structured JSON (`json_writer`).

---

## Build & Run

```bash
# Configure and build (Debug preset)
cmake --preset debug
cmake --build build

# Run all unit tests
ctest --test-dir build --output-on-failure

# Build index and print stats
./build/bin/plag-detect index tests/corpus/anas/small --stats

# Run plagiarism detection on a directory (with window size k=5)
./build/bin/plag-detect detect tests/corpus/anas/small --k 5

# Write JSON report to a file
./build/bin/plag-detect detect tests/corpus/anas/small --k 5 --format json -o report.json
```

### Sanitizer Build (ASan + UBSan)

```bash
cmake --preset sanitize
cmake --build build-sanitize --target check
```

---

## Project Structure

- `src/common/` — `DynArray`, `fnv1a`, modular arithmetic (`mulmod`, `powmod`, `submod`), and `pos_to_line`.
- `src/contract/` — Shared structs (`Occurrence`, `Token`, `Span`, `PairScore`, `IndexStats`, `ErrorCode`).
- `src/index/` — `DynamicBuffer`, `file_reader`, `dir_walker`, `tokenizer`, `RollingHash`, `HashTable`, `OpenHashTable`, `IndexBuilder`, `span_merge`.
- `src/search/` — `kmp`, `candidate_generator`, `scorer`, `verify_mode`.
- `src/rank/` — `MaxHeap` and `RankTree` (AVL tree).
- `src/report/` — `cli`, `audit_report`, `json_writer`.
- `tests/` — Unit test suites for every component (`test_anas_*` and `test_alyan_*`).
- `tools/` — Hash table benchmark (`anas_bench_hash`) and corpus helper scripts.
