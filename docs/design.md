# Architecture & Design Notes

## 1. Token Views and Line Mapping (`src/index/tokenizer.cpp`)

Each file is read once into a `DynamicBuffer` and tokenized in a single pass:
- Comments (`//` and `/* ... */`) and whitespace are skipped.
- Each token stores its byte offset, length, 1-based line number (computed via binary search over `line_starts`), and 64-bit FNV-1a token ID.
- When `--normalize=case` is enabled (default), ASCII letters are converted to lowercase while hashing so case differences and formatting changes produce the same token IDs.

## 2. O(1) Rolling Hash (`src/index/rolling_hash.cpp`)

For a window of $k$ token IDs $(t_0, t_1, \dots, t_{k-1})$, the polynomial hash is:

$$H = \left(\sum_{i=0}^{k-1} t_i \cdot b^{k-1-i}\right) \bmod p$$

where $p = 2^{61} - 1$. When sliding the window by one token (removing $t_{\text{out}}$ and adding $t_{\text{in}}$):

$$H' = \left((H + p - (t_{\text{out}} \cdot b^{k-1} \bmod p)) \cdot b + t_{\text{in}}\right) \bmod p$$

Because $b^{k-1} \bmod p$ (`highPow`) is precomputed once in `init()`, each slide takes $O(1)$ time regardless of $k$.

## 3. Candidate Generation, KMP Verification & Scoring (`src/search/`)

1. **Candidate Generation:** Iterates over the inverted index buckets and counts how many distinct $k$-gram fingerprints each document pair `(docA, docB)` shares using `HashTable<uint64_t, uint32_t>`.
2. **KMP Verification:** For each candidate pair, `kmp_find_all` verifies exact token sequence matches over the normalized token ID arrays, and `merge_spans` merges contiguous or overlapping $k$-gram matches on the same diagonal into maximal copied spans.
3. **Scoring:**
   - **Jaccard:** $\frac{|A \cap B|}{|A \cup B|}$
   - **Containment:** $\frac{|A \cap B|}{\min(|A|, |B|)}$ (catches small copied sections inside a larger file)
   - **Coverage:** $\frac{\text{matched tokens}}{\min(\text{tokens}_A, \text{tokens}_B)}$
   - **Suspicion Key:** $0.50 \cdot \text{coverage} + 0.30 \cdot \text{containment} + 0.20 \cdot \text{jaccard}$

## 4. Ranking & Reporting (`src/rank/`, `src/report/`)

- **`MaxHeap`:** Array-backed binary max-heap supporting $O(\log n)$ `push`/`pop` and `top_k` extraction.
- **`RankTree`:** Self-balancing AVL tree using LL, RR, LR, and RL rotations to maintain $O(\log n)$ height even on sorted insertions, with reverse in-order traversal for leaderboard queries.
- **`AuditReport` & `json_writer`:** Prints corpus statistics, ranked suspicious document pairs, and matched token spans mapped to 1-based source line numbers via `DocumentView::posToLine`.
