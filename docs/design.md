# Design notes

Short version of how the engine works and why every structure is there. Written so that
either member can read it before the viva.

## 1. Pipeline

```
files ─► loader ─► tokenizer ─► index ─► similarity ─► ranking ─► report
          stack     line no.    hash table   Jaccard+KMP   sort/heap/AVL  text
          quick sort            BST
                                probing table
                                rolling hash
```

## 2. Normalisation (why two files with different spacing still match)

| Input | Tokens |
|---|---|
| `int Sum(int a) {` | `int sum int a` |
| `int  sum ( int a )` | `int sum int a` |
| `// int Sum(int a)` | *(nothing — the comment is dropped)* |

Everything is lower-cased, punctuation is thrown away, `//` and `/* */` are removed from
source files, and each token keeps its 1-based line number.

## 3. Inverted index

```
wordTable_ : HashTable<std::string, LinkedList<Occurrence>*>

 "total" ─► [ {doc 0, line 5, count 7} ] ─► [ {doc 1, line 8, count 7} ] ─► null
```

* Key = word, value = linked list of occurrences → one lookup answers "which documents and
  which lines", average **O(1)**.
* Collision resolution = **separate chaining** (every bucket is a `LinkedList`).
* Rehashing when `size / buckets > 0.75` → the average chain length stays below 1.
* Deletion is O(1) (unlink from the chain), which is why chaining was chosen over open
  addressing for this table.

A second table with **linear probing** (`probing.hpp`) counts the words of a single document
where no deletion happens; it exists to compare the two collision techniques and to collect
the "average probes" statistic.

## 4. Fingerprints (rolling hash)

For a window of k token hashes `t0 … t(k-1)`:

```
H  = ( … ((t0 · b + t1) · b + t2) … ) mod M           b = 131, M = 1 000 000 007
H' = ((H − t_out · b^(k−1)) · b + t_in) mod M         one slide = O(1)
```

`b^(k−1) mod M` is computed once. The fingerprints of a document are put into a hash table to
remove duplicates and then **merge sorted**, so two documents are compared with one walk:

```
shared = number of equal values while walking both sorted arrays    O(n + m)
Jaccard      = shared / (|A| + |B| − shared)
Containment  = shared / min(|A|, |B|)
```

## 5. Proving a match with KMP

Score alone is not evidence, so for every pair with a score above the limit:

1. take every chunk of **6 consecutive words** of document A,
2. search that chunk in B's normalised text with KMP (`O(n+m)` each),
3. if it is found at the position that continues the current block, extend the block,
   otherwise close it and start a new one,
4. convert word indices into line numbers (`wordLines[]`, and `wordStart[]` + binary search
   for positions found by KMP).

Result: `copied: ali.cpp lines 4-12 == bilal.cpp lines 7-15 (21 words)`.

## 6. Verdict thresholds

| Score | Label |
|---|---|
| ≥ 0.75 | CRITICAL |
| ≥ 0.40 | HIGH |
| ≥ 0.20 | MEDIUM |
| ≥ 0.08 | LOW |
| < 0.08 | not reported |

Constants: `kMinScore` and `suspicionLabel()` in `src/similarity.cpp`, k in `main.cpp`
(`AppData::kgramSize = 5`).

## 7. Ranking

| Question | Structure | Cost |
|---|---|---|
| "the whole list, best first" | merge sort on the pair list | O(P log P) |
| "only the top K" | max heap, pop K times | O(P + K log P) |
| "any rank, always sorted" | AVL tree, reverse in-order | O(P log P) insert |

## 8. Clusters (graph)

Vertex = document, edge = "score ≥ threshold". One BFS per unvisited vertex gives the
connected components, i.e. the groups of students who copied together. Complexity O(V + E).

## 9. Errors and limits

* A folder that cannot be opened, or that has no supported files → the loader returns false and
  the menu prints the reason (`loadCorpus(..., std::string& error)`).
* Files bigger than 2 MB, hidden files and `build/` are skipped.
* A document with fewer than k words has no fingerprints and can never be reported.
* Reordered sentences, paraphrasing and translated text are **not** detected — the comparison
  is word-level.
