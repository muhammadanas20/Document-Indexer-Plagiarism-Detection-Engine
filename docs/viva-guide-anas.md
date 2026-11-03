# Viva Guide — ANAS (Member 1, Group Leader)

Read this in order. Section 1 is what you say in the first 60 seconds. Sections 3–5 are the
answers. Section 6 is the practice questions. Section 7 is what you must be able to do live
in the lab.

---

## 1. My part in one paragraph (say this first)

> "I did the **indexing** half of the project. My job is to get the text out of the files
> and store it in a structure that answers 'which documents contain this word?' in O(1).
> I read the folders with a **stack**, cut the files into words with the **tokenizer**,
> and store `word → list of (document, line, count)` in a **hash table with separate
> chaining**, where every bucket is a **singly linked list** that I wrote myself. I also
> build the **vocabulary BST** with Alyan's tree class, compute the **k-gram fingerprints**
> with a **rolling hash** (the Rabin-Karp idea), and I wrote the **merge sort, quick sort,
> insertion sort and binary search** that the rest of the program uses."

That is 8 sentences but they are all your own keywords: stack, tokenizer, hash table,
chaining, linked list, rolling hash, merge sort, binary search.

---

## 2. My files

| File | What it is | Which menu option uses it |
|---|---|---|
| `src/list.hpp` | singly linked list (template) | hash buckets, posting lists |
| `src/stack.hpp` | stack on a growable array | option 1 (folder walk) |
| `src/hash_table.hpp` | hash table, separate chaining, rehashing | option 3 (search a word) |
| `src/probing.hpp` | hash table, linear probing | option 2 (statistics) |
| `src/sort.hpp` `.cpp` | merge sort, quick sort, insertion sort, binary search | options 5, 9 |
| `src/fingerprint.hpp` `.cpp` | rolling hash → k-gram fingerprints | option 5 |
| `src/tokenizer.hpp` `.cpp` | text → words + line numbers | option 1 |
| `src/document.hpp` | one loaded file (words, lines, normalized text) | everywhere |
| `src/loader.hpp` `.cpp` | folder walk + read + tokenize | option 1 |
| `src/index.hpp` `.cpp` | the inverted index + statistics | options 2, 3, 9 |

---

## 3. The six things I have to be able to explain

### 3.1 Singly linked list — `list.hpp`

**What:** each node holds a value and one pointer to the next node. The list keeps `head`,
`tail` and `size`.

**Why we need it:** a hash bucket must grow without moving anything, and a posting list has
an unknown length. With an array we would have to guess the size or shift elements.

**How:**
```cpp
struct Node { T value; Node* next; };
void pushFront(value) { head_ = new Node(value, head_); }   // O(1)
void pushBack(value)  { tail_->next = new Node(value, nullptr); tail_ = ...; }  // O(1)
bool remove(value)    { walk with two pointers, unlink, delete }               // O(n)
```
Two pointers `previous` and `current` are used in `remove()` because to delete a node you
must change the `next` of the node **before** it.

**Complexity:** insert at front/back O(1), search O(n), delete O(n), memory O(n).

**Rule of Three:** I wrote the destructor (`clear()` frees every node) and **deleted** the
copy constructor and assignment (`= delete`), because copying would copy the pointers and
then both lists would free the same nodes (double free → crash).

> If he asks "why did you delete the copy constructor instead of writing it?" →
> "Because we never copy a list in this project. Deleting it means the compiler stops me if
> I ever try, instead of giving me a shallow copy that frees the same memory twice."

### 3.2 Stack — `stack.hpp`

**What:** LIFO container on a growable array.

**Where:** `loader.cpp` — walking the folders.

**How:** `push` writes at `data_[size_++]`; when the array is full, `grow()` makes a new
array of double size and copies the old values; `pop` returns `data_[--size_]`.

**Why a stack and not recursion:** the folder tree can be deep. Recursion uses the call
stack (about 1-8 MB) and could overflow; my stack lives on the heap. It is also the classic
"iteration with an explicit stack" topic.

**Complexity:** push/pop O(1) amortised, memory O(depth).

> If he asks "why is push O(1) if you sometimes copy the array?" → "Because doubling happens
> only at sizes 1,2,4,8,16..., so the total copying work is 1+2+4+...+n < 2n, which averages
> to O(1) per push. That is amortised O(1)."

### 3.3 Hash table with separate chaining — `hash_table.hpp`  ⭐ my main part

**What:** an array of buckets; **every bucket is a linked list** of `(key, value)` pairs.

**Where:** the inverted index. Key = a word, value = the posting list of that word
(a linked list of `{documentId, firstLine, count}`). Also used for the fingerprint sets.

**Hash function:**
```cpp
unsigned long long hashKey(const std::string& key) {
  unsigned long long hash = 5381;
  for (char c : key) hash = hash * 31 + (unsigned char)c;   // djb2
  return hash;
}
```
Then `index = hash % bucketCount`.

**Collision resolution = separate chaining.** Two words with the same bucket just become
two nodes in the same list. Search walks that one list only.

**Rehashing:** before an insert I check the load factor `size/buckets`. If it would pass
**0.75**, I double the bucket array and put every key into its new bucket. That keeps the
average chain length around 0.75, so lookup stays O(1).

```cpp
if ((size_ + 1.0) / bucketCount_ > 0.75) rehash(bucketCount_ * 2);
```

**Complexity:** insert / find / remove **average O(1)**, worst case **O(n)** (all keys in
one bucket), memory O(n + buckets).

> **The most likely question: "why chaining and not linear probing?"**
> "Because the index must also support deletion, and chaining deletes in O(1) by unlinking a
> node. With open addressing, deleting leaves a hole that breaks later searches, so I would
> have to keep tombstones. Chaining also never fills up, so there is no 'table full' case.
> I did implement linear probing too (`probing.hpp`) for the per-document word counts, where
> there is no deletion — and the statistics screen compares the two."

> **"What is the load factor and why 0.75?"** → "Entries ÷ buckets. Above 0.75 the chains get
> long and lookups stop being O(1); below 0.25 we waste memory. 0.75 is the usual compromise
> and Java's HashMap uses the same value."

### 3.4 Linear probing table — `probing.hpp`

Same idea as open addressing: if the home slot is taken, try the next slot (`(i+1) % cap`),
then the next, and wrap around. No linked lists, everything in one array.

**Where:** counting the words of **one** document (distinct words, average probes).
**No deletion**, so I do not need tombstones — that is exactly why it is safe here.

**Downside (say it yourself):** primary clustering — long runs of filled slots make the probe
sequences long, and performance falls fast above a load factor of about 0.7.

### 3.5 Rolling hash → k-gram fingerprints — `fingerprint.cpp`

**What:** a "k-gram" is k consecutive words. Its fingerprint is one number instead of a
string.

**Polynomial hash:**
`H = ((w0·b + w1)·b + w2 ... ) mod p`, with `b = 131`, `p = 1 000 000 007`.

**Slide in O(1)** instead of O(k):
```
H' = ((H − w_out · b^(k−1)) · b + w_in) mod p
```
`b^(k−1) mod p` is computed once before the loop. Removing the old word and adding the new
one is two multiplications, so a document of n words gives n fingerprints in **O(n)**
instead of O(n·k).

**Duplicates:** I insert the fingerprints into a hash table, so a repeated paragraph counts
once (otherwise repeating a paragraph would double the score).

**Then merge sort** puts them in ascending order (`sort.cpp`), so comparing two documents is
one walk over two sorted lists — O(n + m) — the same merge step as merge sort.

> **"Why hash the k-grams instead of storing the words?"** → "A fingerprint is 8 bytes and one
> comparison instead of comparing k words. It also makes the comparison independent of the
> length of the window."

> **"Can two different k-grams have the same fingerprint?"** → "Yes, hashing can collide.
> That is why the score is only a **filter**, and the real proof is done by KMP on the actual
> words in step 2."

### 3.6 Sorting — `sort.cpp`

* **Merge sort** (O(n log n) always, stable, O(n) extra memory) — on the fingerprints and on
  the pair list. Split in the middle, sort both halves recursively, merge.
* **Quick sort** (average O(n log n), worst O(n²)) — on the file paths. Pivot = middle
  element (choosing the first element would be O(n²) on an already sorted list).
* **Insertion sort** (O(k²)) — on the top-K list, because K is about 10 and for tiny lists
  it beats every other sort.
* **Binary search** (O(log n)) — `binarySearchWord` on the sorted vocabulary.

> **"Why not use one sort for everything?"** → "Because the input sizes are different. Merge
> sort for the big lists because it never degrades, insertion sort for a 10-element list
> because it has almost no overhead, quick sort for the paths because it sorts in place."

### 3.7 Tokenizer and loader (quickly)

* `tokenize()`: skips `//` and `/* */` comments (for code files), keeps letters/digits/
  underscore, lower-cases everything, and remembers the current line number for each word.
  So `int Sum(int a)` and `int sum ( int a )` produce the same tokens — that is how a copy
  with different spacing is still caught.
* `loadCorpus()`: stack of folders → `opendir` / `readdir` / `stat` → keep `.cpp .h .hpp .c
  .cc .txt .md` → skip hidden files, `build/`, and files > 2 MB → **quick sort** the paths →
  read each file → tokenize → build `Document`.

---

## 4. My code path (be able to trace it)

**Option 1 – load**
`main.cpp: loadFolder()` → `loader.cpp: loadCorpus()` → `stack` walk → `quickSortStrings()`
→ `tokenizer.cpp: tokenize()` → `document.hpp: buildNormalized()` → `index.cpp: build()`.

**Option 2 – statistics**
`main.cpp: case 2` → `report.cpp: printIndexStats()` reads
`wordTable_.loadFactor()`, `longestChain()`, `vocabulary_.height()`, `averageProbes()`.

**Option 3 – search a word**
`main.cpp: searchWord()` → `tokenize()` the user's word (same normalisation!) →
`index.cpp: searchWord()` → `wordTable_.get(word)` (one bucket, walk one chain) → print
document + line + count.

**Option 5 – fingerprints** (my part inside Alyan's pipeline)
`similarity.cpp: compareAll()` → `countSharedFingerprints()` walks my sorted fingerprint
arrays → Jaccard → `findCopiedSpans()` (KMP) → report.

---

## 5. Numbers I should remember

| Thing | Value | Why that value |
|---|---|---|
| initial buckets | 16 | small start, grows by doubling |
| rehash trigger | load factor > 0.75 | keeps chains short |
| average chain length | size/buckets ≈ 0.4 in the demo | good hash spread |
| k (window) | 5 words | long enough to be meaningful, short enough not to miss |
| hash base / prime | 131 / 1 000 000 007 | 131 spreads ASCII well, prime keeps the range small |
| vocabulary BST height | 15 for 102 words | a balanced tree would be ~7 → that is why the AVL tree exists |
| tokens in demo corpus | 523 tokens, 102 distinct | ~5× compression of the vocabulary |

---

## 6. Questions he will probably ask (with the answers)

**Q1. What is an inverted index?**
> "A map from a word to the list of places where it appears. Normally you search every
> document for a word (that is a forward search, O(total words)). Inverted means we look the
> word up once in the hash table and get the answer, O(1) on average."

**Q2. Walk me through one lookup.**
> "Hash the word with `hash*31 + c`, take modulo the number of buckets, go to that bucket,
> walk the linked list comparing keys, and when the key matches I get the posting list. Then
> I print every node of the posting list: document, first line, count."

**Q3. What happens when two words have the same hash?**
> "Collision. With chaining they are both stored in the same bucket list, so the lookup has
> to compare the keys — that is why the bucket stores the key as well as the value."

**Q4. Worst case of your hash table?**
> "O(n): if every key lands in the same bucket, the list becomes a linked list and the search
> is linear. Good hash + rehashing keeps this practically impossible."

**Q5. Why did you need a `tail` pointer in the linked list?**
> "Without it, `pushBack` has to walk to the end, O(n). With the tail pointer it is O(1),
> and the posting lists are built with pushBack."

**Q6. How does the tokenizer map a word to a line?**
> "I count '\n' as I scan and store the current line in `lines[i]` for every word. So a word
> carries its own line number — I never search for it later."

**Q7. Why lower-case everything?**
> "Normalisation. `Sum` and `sum` are the same identifier, and in text `The` and `the` are
> the same word. Without it, a copy that changes capitalisation would look different."

**Q8. Why do you strip comments?**
> "So a student cannot hide a copy by writing `//` in front of it — the comment is not code
> and it is not part of the comparison. It also stops the words inside comments from
> inflating the index."

**Q9. Why merge sort and not bubble sort?**
> "Bubble sort is O(n²). A document with 5 000 fingerprints would need 25 million
> comparisons, while merge sort needs about 5 000·13. The course covers bubble sort, but
> sorting the real data needs O(n log n)."

**Q10. Where is the recursion in your part?**
> "Merge sort and quick sort are recursive, and the BST inserts recursively (Alyan's file).
> The folder walk is **not** recursive, I used a stack instead."

**Q11. How do you know your sorts and hash table are correct?**
> "`tests/test_sort_search.cpp` checks the sorted output element by element and the binary
> search on found/missing words. `tests/test_hash_table.cpp` inserts 5 000 keys, then checks
> that every one of them can be read back, that the load factor stayed ≤ 0.75, and that the
> longest chain is short. It also tests a table with only 4 buckets to force collisions."

**Q12. What is the Rule of Three?**
> "If a class manages a resource (my lists, tables and trees use `new`), you must write the
> destructor, the copy constructor and the copy assignment — otherwise the compiler copies
> the pointers and you free the same memory twice. I wrote the destructor and **deleted** the
> two copy functions, because we never copy these structures."

**Q13. Memory leaks — how did you check?**
> "Every `new` has a matching `delete` in a destructor: `LinkedList::clear()`,
> `HashTable::~HashTable` (deletes every bucket), `Index::~Index` (deletes every posting
> list). The test programs run under ctest and exercise the destructors."

**Q14. Why is `computeFingerprints` O(n) and not O(n·k)?**
> "Because of the rolling hash. I remove the leaving word with one multiplication by
> `base^(k-1)` and add the new word with one multiplication, so a slide costs O(1) no matter
> how big k is."

**Q15. What does "normalized" mean in `Document`?**
> "All the words of a file joined by single spaces, e.g. `int sum int a`. KMP works on
> characters, so it needs one string per document. I also store `wordStart[i]`, the character
> position of word i, so a character position found by KMP can be turned back into a word
> index (binary search) and then into a line number."

**Q16. Why is your document number always the same on every run?**
> "Because I quick sort the file paths before reading them. Without that, `readdir` returns
> directory entries in an arbitrary order and the document numbers — and the whole report —
> would change between runs."

**Q17. Where is the graph / heap / AVL in your part?**
> "Those are Alyan's. The only place I touch his code is that `index.cpp` calls his
> `WordTree` for the vocabulary, and `similarity.cpp` calls my `computeFingerprints`."

**Q18. How long does indexing take?**
> "`samples/students` (6 files, 523 words) indexes in about 1 ms. The work is linear in the
> number of tokens: one tokenizer pass, one hash insert per token, and n k-gram slides."

---

## 7. Things you must be able to do live

Practise each of these once; they take 1 minute each.

1. **Show the index working.**
   `./build/plag-detect samples/students` → option `3` → `total`. You get 4 documents with
   line numbers and counts. Then type a made-up word → "found in 0 documents".
2. **Show the statistics** (option 2) and point at: load factor, longest chain, average
   probes, BST height. Say one sentence about each.
3. **Break the hash function on purpose**: change `hash * 31` to `hash * 1` (or `% 4`
   buckets), rebuild, and show the longest chain jumping. Then change it back. *This is the
   best answer to "how do you know the hash function is good?"*
4. **Change k** in `main.cpp` (`app.kgramSize = 3` vs `5`) and show the scores changing.
5. **Add a word to a sample file**, reload, and show it appearing in option 3 and in the
   vocabulary (option 9).
6. **Run the tests**: `ctest --test-dir build --output-on-failure`. Say: "6 test programs,
   about 250 checks, all green."

## 8. One-sentence answers to keep in your pocket

* **Hash table** — "average O(1) lookup by turning the key into an array index."
* **Chaining** — "collisions become nodes in the same bucket's linked list."
* **Rehashing** — "double the array and re-insert when the load factor passes 0.75."
* **Rolling hash** — "slide the window in O(1) by removing the old word and adding the new one."
* **Stack in the loader** — "iterative depth-first folder walk, no call-stack overflow."
* **Merge sort** — "O(n log n) always, stable, needs O(n) extra memory."
* **Binary search** — "halve the range each step, O(log n), only on sorted data."
* **Rule of Three** — "destructor + copy constructor + copy assignment, or delete the copies."
