# Viva Guide — ALYAN (Member 2)

Read this in order. Section 1 is your 60-second opening. Sections 3–6 are the answers,
section 7 is the practice questions, section 8 is what you must be able to do live.

---

## 1. My part in one paragraph (say this first)

> "I did the **detection** half: I take the fingerprinted documents that Anas builds and
> decide which ones are copied. I compare the sorted k-gram fingerprints with a **two-pointer
> merge** and compute the **Jaccard score**, then I **prove** it with **KMP** and turn the
> positions into line numbers. The results are ranked with a **merge sort**, a **max heap**
> (top-K) and an **AVL tree** (full leaderboard). I also build a **similarity graph** and use
> **BFS with a queue** to group the students who copied together, I wrote the **brute force,
> KMP and Rabin-Karp** string searches that the phrase search uses, plus the **BST** for the
> vocabulary, the report and the menu."

---

## 2. My files

| File | What it is | Menu option |
|---|---|---|
| `src/queue.hpp` | queue (circular array) | 8 (BFS) |
| `src/bst.hpp` | binary search tree (vocabulary) | 9 |
| `src/avl.hpp` | AVL tree with rotations (leaderboard) | 7 |
| `src/heap.hpp` | max heap / priority queue | 6 |
| `src/graph.hpp` | graph + BFS + clusters | 8 |
| `src/string_match.hpp` `.cpp` | brute force, KMP, Rabin-Karp | 4 |
| `src/similarity.hpp` `.cpp` | Jaccard score, shared fingerprints, copied spans | 5 |
| `src/report.hpp` `.cpp` | all the printing | 2,3,5,6,7,8,9 |
| `src/main.cpp` | the menu | everything |

---

## 3. The seven things I have to be able to explain

### 3.1 Queue — `queue.hpp`

**What:** FIFO container in a **circular array**.

```cpp
push: data_[(front_ + size_) % capacity_] = value; ++size_;
pop:  value = data_[front_]; front_ = (front_ + 1) % capacity_; --size_;
```
The `%` makes the array wrap around, so we never shift elements after a pop (shifting would
cost O(n)).

**Where:** the BFS queue in `graph.hpp`.

**Complexity:** push/pop O(1) amortised.

> If he asks "why not a stack in BFS?" → "BFS must visit the neighbours in the order they were
> discovered — first in, first out. With a stack it becomes DFS and the distances/levels are
> lost."

### 3.2 Binary search tree — `bst.hpp`

**What:** node = (word, documentCount, left, right). Left subtree has smaller words, right
subtree bigger. Insert and search are **recursive**: go left if the word is smaller, right if
bigger, stop at `nullptr` and put the new node there.

**Where:** the **vocabulary**. `inOrder()` (left, node, right) prints the words in
**alphabetical order** — that is the BST property and it is why we use a BST here instead of
the hash table (the hash table cannot give sorted output).

**Complexity:** average O(log n) per insert/search, worst case **O(n)** when the input is
already sorted (the tree becomes a line). Height in the demo: 15 for 102 words, while a
balanced tree would be ~7 — I say this out loud and then explain that this is exactly why
the leaderboard uses the AVL tree.

> If he asks "why not hash the vocabulary too?" → "Because I need it **sorted** for the
> vocabulary print and for binary search. A hash table has no order."

### 3.3 AVL tree — `avl.hpp`  ⭐ my favourite part

**What:** a BST that keeps itself balanced. Every node stores its **height**, and after every
insertion I walk back up and check `balance = height(left) − height(right)`. If it is
`> 1` or `< −1` I rotate:

| Case | When | Fix |
|---|---|---|
| LL | balance > 1 and the key went left-left | one **right rotation** |
| RR | balance < −1 and the key went right-right | one **left rotation** |
| LR | balance > 1 and the key went left-right | left rotate the left child, then right rotate |
| RL | balance < −1 and the key went right-left | right rotate the right child, then left rotate |

```cpp
static Node* rotateRight(Node* y) {          // y is too heavy on the left
  Node* x = y->left;
  y->left = x->right;                        // the middle subtree changes parent
  x->right = y;
  updateHeight(y); updateHeight(x);          // fix the heights bottom-up
  return x;                                  // x is the new root of this subtree
}
```

**Where:** the **leaderboard** of document pairs. I insert every pair with its score, and the
**reverse in-order traversal** (`right, node, left`) returns the pairs already sorted from the
highest score to the lowest — no sorting needed afterwards.

**Complexity:** insert/search **O(log n) guaranteed** (height ≤ 1.44·log₂n), memory O(n).
Proof of the test: inserting scores 0…999 in increasing order — the worst case for a plain
BST — gives a height of at most 16 in `tests/test_trees_heap.cpp`.

> **Likely question: "why does a rotation not break the BST property?"**
> "Because the rotation only changes which node is the parent. In `rotateRight`, everything in
> `x->right` is still smaller than `y` and everything in the left of `x` is still smaller than
> `x`, so the inorder order of the words/scores does not change."

> **"Why AVL and not just a BST?"** → "Because the insert order is the order in which the pairs
> are found, which can be almost sorted by score. A plain BST would then be a linked list with
> O(n) insert. The AVL tree guarantees O(log n)."

### 3.4 Max heap / priority queue — `heap.hpp`

**What:** a **complete binary tree stored in an array** where every parent is ≥ its children:
`parent(i) = (i−1)/2`, `left(i) = 2i+1`, `right(i) = 2i+2`.

* `push`: put the item at the end, then **bubble up** while it is bigger than its parent.
* `pop`: take the root, move the last item to the root, then **sift down** while a child is
  bigger.

**Where:** "show me the **top-K** most suspicious pairs" (menu 6). I push all pairs and pop K
times.

**Complexity:** push O(log n), pop O(log n), building by n pushes = O(n log n) (it would be
O(n) with heapify, which we do not need for these sizes). Top-K costs O(n + k log n) instead
of O(n log n) for a full sort.

> **"Why a heap and not a sort?"** → "Because I only need the first K of n items. Sorting
> prepares all n, the heap only prepares the biggest one, K times."

> **"Heap or AVL for ranking — what is the difference?"** → "The heap gives only the maximum
> cheaply and cannot answer 'the 57th best'. The AVL tree keeps everything sorted, so it can
> answers any rank. Small K → heap, whole leaderboard → AVL."

### 3.5 Graph + BFS — `graph.hpp`  ⭐ (Week 14 topic)

**What:** **adjacency list** — an array with one linked list per vertex, holding its
neighbours. For a sparse graph it needs O(V + E) memory, while an adjacency matrix always
needs O(V²).

**Where:** the **similarity graph**: a vertex per document, an **edge whenever the score is
above a threshold**. If four students copied from one person, the pairs are 6 edges but the
graph has one connected component — BFS shows the group.

**BFS:**
```cpp
queue.push(start); seen[start] = true;
while (!queue.empty()) {
  int v = queue.pop();
  for (every neighbour u of v) if (!seen[u]) { seen[u] = true; queue.push(u); }
}
```
One BFS per unvisited vertex gives all the **connected components** = the clusters.

**Complexity:** O(V + E).

> **"Why BFS and not DFS for the clusters?"** → "Either one finds the components. BFS is
> iterative with a queue, so no recursion depth problem, and it visits the closest documents
> first, which reads better in the report. DFS with a stack would also work."

### 3.6 String search: brute force, KMP, Rabin-Karp — `string_match.cpp` ⭐ (Week 12)

All three answer the same question: **at which positions does `pattern` appear in `text`?**

| Algorithm | Idea | Time |
|---|---|---|
| **Brute force** | try the pattern at every start position, compare character by character | O(n·m) |
| **KMP** | precompute a prefix (LPS) table so that after a mismatch we never move backwards in the text | O(n + m) |
| **Rabin-Karp** | hash the pattern, hash the first window, then **slide the hash in O(1)**; verify only when the hashes match | O(n + m) average, O(n·m) worst |

**KMP prefix table**: `prefix[i]` = length of the longest proper prefix of
`pattern[0..i]` that is also a suffix. When a mismatch happens after `matched` characters, we
set `matched = prefix[matched − 1]` instead of restarting at 0 — so no character of the text
is ever read twice.

Example in the tests: `buildPrefixTable("ababaca")` = `{0,0,1,2,3,0,1}`.

**Rabin-Karp slide:**
```
H' = ((H − text[start−1]·b^(m−1))·b + text[start+m−1]) mod p
```
A hash match is **not** a proof, so I compare the characters too (that is the "collision
check").

**Where:**
* Menu **4** runs all three on the user's phrase in every document and prints the time of each,
  so the difference is visible. They must report the same number of matches — that is how we
  verify each other.
* **Step 2 of the plagiarism pipeline** (below).

### 3.7 The plagiarism pipeline — `similarity.cpp` ⭐⭐ (the heart of my part)

```
STEP 1 (cheap filter)                      STEP 2 (proof)
sorted fingerprints of A   ->  count      every 6-word chunk of A
sorted fingerprints of B   ->  common     searched inside B with KMP
        |                        |                 |
   Jaccard = common / union  ----+                 +--> line ranges ("lines 4-12 == lines 7-15")
```

**Why two steps?** With 100 documents there are 4 950 pairs. Running KMP on all of them is
slow. Step 1 is only numbers (two sorted lists, one walk, O(n + m)) and throws away almost
every pair.

**Jaccard:** `|A ∩ B| / |A ∪ B|` — "how much do the two documents share out of everything they
have". Range 0…1.
**Containment:** `|A ∩ B| / min(|A|,|B|)` — "how much of the *smaller* document is inside the
other". It stays high when a small file was copied into a big file, where Jaccard would be
low. We print both.

**Span finding:** I take every chunk of **6 words** of document A (chunks overlap, so word 0
starts a chunk, word 1 starts a chunk, …), search the chunk in B with **KMP**, and if it is
found I open a span. The next chunk usually matches at the next position (same diagonal), so
the span just grows; when the matches stop, I close the span and report the line ranges. That
is why 40 copied words in a row appear as **one** span, not 35.

**Verdict labels** (fixed thresholds in `suspicionLabel()`):
`≥ 0.75 CRITICAL`, `≥ 0.40 HIGH`, `≥ 0.20 MEDIUM`, `≥ 0.08 LOW`, else `NONE`. Pairs under
0.08 are not reported at all.

> **"Why Jaccard and not just counting equal words?"** → "Because the length matters. Two long
> documents share common words like `int`, `return`, `for` anyway. Jaccard divides by the
> union, so sharing 10 words out of 1 000 gives a small score, and it is comparable between
> documents of different sizes."

---

## 4. My code path (trace it once)

**Option 5 – detect:** `main.cpp: detectPlagiarism()` → `similarity.cpp: compareAll()` →
`countSharedFingerprints()` (two-pointer) → `jaccardSimilarity()` → `findCopiedSpans()` →
`kmpSearch()` → `mergeSortPairs()` (Anas's merge sort) → `report.cpp: printPairs()`.

**Option 6 – top-K:** `showTopPairs()` → build a `MaxHeap` from the pairs → `topK(k)` →
`insertionSortScores()` → `printTopK()`.

**Option 7 – leaderboard:** `showLeaderboard()` → `Leaderboard::insert()` (AVL) →
`rankedDescending()` (reverse in-order) → `printLeaderboard()`.

**Option 8 – clusters:** `showClusters()` → `Graph` + `addEdge()` for every pair above the
threshold → `Graph::clusters()` → `bfs()` (queue) → `printClusters()`.

**Option 9 – vocabulary:** `showVocabulary()` → `Index::sortedVocabulary()` (Anas calls my
`WordTree::inOrder`) → `binarySearchWord()` → `Index::documentFrequency()`.

---

## 5. Numbers I should remember

| Thing | Value | Why |
|---|---|---|
| span chunk | 6 words | long enough to be meaningful, short enough to catch a half-sentence copy |
| report threshold | score ≥ 0.08 | below that it is only boilerplate |
| labels | 0.75 / 0.40 / 0.20 / 0.08 | fixed, so the output is reproducible |
| demo results | 1.000 / 0.444 / 0.302 | verbatim copy / renamed copy / half copy |
| AVL height | ≤ 16 for 1 000 inserts | plain BST would reach 1 000 |
| heap top-K | O(n + k log n) | vs O(n log n) for sorting everything |

---

## 6. Questions he will probably ask (with the answers)

**Q1. In one sentence, how does your program detect plagiarism?**
> "It fingerprints every k-word window of both documents, counts how many fingerprints they
> share (Jaccard), and if that score is high it runs KMP to find the exact copied lines."

**Q2. What is a k-gram fingerprint?**
> "A number computed from k consecutive words with a polynomial rolling hash. Same k words in
> the same order → same number. It turns a document into a set of numbers we can compare
> quickly."

**Q3. Why is the fingerprint step O(n) and not O(n·k)?**
> "Because of the rolling hash: the window slides by removing one word and adding one word,
> two multiplications, so each slide is O(1)."

**Q4. What if two different k-grams get the same fingerprint?**
> "That is a hash collision. It only makes the score slightly higher — never a false proof,
> because step 2 re-checks the real words with KMP."

**Q5. How does KMP avoid re-reading the text?**
> "It precomputes the prefix table. After a mismatch at `matched` characters, the table tells
> us how much of the pattern is still matching, so we jump to `prefix[matched−1]` instead of
> going back to the start. Every text character is compared once → O(n + m)."

**Q6. Difference between KMP and Rabin-Karp?**
> "Both are linear. KMP compares characters and never hashes; Rabin-Karp compares hashes and
> only verifies on a hash match, so it can be faster on average but has a worst case with
> collisions. Rabin-Karp has the advantage that it extends nicely to 2D or to multiple
> patterns."

**Q7. Why do you use KMP and Rabin-Karp in the same project?**
> "Rabin-Karp's rolling hash is the idea behind the fingerprints (step 1, the cheap filter),
> and KMP is used in step 2 to prove the match and get the line numbers. Same idea, two
> different jobs."

**Q8. Explain the Jaccard formula.**
> "Shared fingerprints divided by all distinct fingerprints of both documents. 1.0 means the
> sets are identical (a verbatim copy), 0 means they share nothing."

**Q9. Why is the score not 1.0 for `bilal.cpp`?**
> "Because he renamed the functions, so every k-gram that contains a function name changed.
> The body still matches, so the score stays high (0.444 → HIGH), but it is not identical.
> That is an honest limitation of word-level comparison — I say it in the report."

**Q10. Why the AVL tree here and not the heap?**
> "The heap only gives the maximum repeatedly. The AVL tree keeps the whole leaderboard sorted
> and can answer any rank. We use both: heap for 'top 5', AVL for the full list."

**Q11. Why does the AVL tree have to store the height in every node?**
> "Because the balance factor must be checked after every insertion, and computing a height by
> walking the subtree would cost O(n) each time. Storing it makes the check O(1) and the update
> O(1) per level."

**Q12. Show me an LR rotation.**
> "The left child is right-heavy: I rotate left around the left child, which turns the LR case
> into the LL case, then rotate right around the node. Two rotations, the tree becomes
> balanced, and the in-order sequence does not change."

**Q13. Why is the graph adjacency list better than a matrix here?**
> "With 100 documents a matrix has 10 000 cells and almost all of them are 0, because only a
> few pairs are similar. The adjacency list stores only the real edges, O(V + E) memory, and
> BFS walks only the edges that exist."

**Q14. What is the time complexity of the whole detection?**
> "Fingerprints: O(N) for N total words. All pairs: O(P) where P is the number of pairs and
> each pair costs O(f) with f fingerprints — with the shared-fingerprint filter most pairs stop
> immediately. KMP proof: only for the few surviving pairs, O(n + m) each. Ranking: O(P log P)
> with merge sort."

**Q15. What did you test?**
> "`tests/test_trees_heap.cpp`: BST in-order is sorted, AVL height ≤ 16 after 1 000 sorted
> inserts, heap top-K order. `tests/test_graph.cpp`: degrees, BFS order, clusters.
> `tests/test_sort_search.cpp`: brute force, KMP and Rabin-Karp must return exactly the same
> positions, plus the classic `ababaca` prefix table. Total: 6 programs, ~250 checks."

**Q16. Where is recursion in your part?**
> "BST insert/search/height, AVL insert + rotations, the reverse in-order traversal, and the
> merge sort/quick sort. BFS I wrote iteratively with a queue on purpose."

**Q17. What would you add with more time?**
> "Word-level shingling with synonyms, IDF weighting so common words count less, and a
> tolerance for reordering. And a GUI or PDF support."

---

## 7. Things you must be able to do live

1. **Option 5** on `samples/students`, then read the table out loud: "ali/faisal 1.000
   CRITICAL because it is a verbatim copy; ali/bilal 0.444 HIGH because he renamed the
   functions; sara is not in the list."
2. **Option 4** with the phrase `int numbers int count` — show that brute force, KMP and
   Rabin-Karp report the same matches, and that KMP/Rabin-Karp are not slower.
3. **Option 7** — point at `AVL height: 3 for 6 node(s)` and say what would happen with a
   plain BST.
4. **Option 8** with threshold `0.35` — the cluster is ali + bilal + faisal (one group), and
   explain that hamza is below the threshold.
5. **Option 6** with K=2 — the top two pairs.
6. **Break something on purpose**: change the threshold in `suspicionLabel()` (e.g. HIGH at
   0.30) and show the labels change; then change it back.

## 8. One-sentence answers to keep in your pocket

* **Queue** — "circular array, first in first out, used by BFS."
* **BST** — "left smaller, right bigger; in-order gives sorted output; worst case O(n)."
* **AVL** — "BST + height + rotations, so the height stays O(log n)."
* **Heap** — "complete tree in an array, parent ≥ children, pop the max in O(log n)."
* **Graph** — "adjacency list, O(V+E) memory, BFS finds the connected components."
* **Brute force** — "try every position, O(n·m)."
* **KMP** — "prefix table, never go back in the text, O(n+m)."
* **Rabin-Karp** — "rolling hash, slide in O(1), verify on a hash match."
* **Jaccard** — "shared ÷ union, between 0 and 1."
