# Design

One architecture document for both halves. Anas writes §1-§2, Alyan writes §3-§4, in separate
PRs, so the two never touch the same lines. Decisions from the Sunday call are recorded here
the same day.

---

## 1. The two token views  *(Anas — lands Week 2)*

Every file is read once into a `DynamicBuffer` and tokenised into a single pool of
`{offset, length}` slices. From that one allocation the engine gets two views:

* **exact** — the bytes as written. Used for printing excerpts in the report.
* **normalised** — the same slice, optionally lower-cased under `--normalize=case`. This is
  the view that is compared, and it is what catches the student who only renamed variables
  and reformatted the file.

Two views, one allocation: no second copy of the corpus ever exists.

> TODO(Anas W2, Step 3): two lines here explaining the two token views — this text goes
> straight into the report.

## 2. Why the roll is O(1)  *(Anas — lands Week 2)*

The window hash is

```
H  = (t0*b^(k-1) + t1*b^(k-2) + ... + t(k-1)) mod p
```

Sliding the window one token to the right does not recompute it:

```
H' = ((H - out*b^(k-1)) * b + in) mod p
```

One subtraction, one multiplication, one addition — independent of `k`. `highPow = b^(k-1) mod p`
is precomputed once in `init()`. Because the index is built with a single loop over the token
stream (`roll()`, then `insert()`), the whole corpus costs **O(total tokens)** whatever `k` is.
That is the one-sentence viva answer.

Underflow guard: the modulus is added *before* the subtraction —
`(H + p - mulmod(out, highPow)) % p` — and a debug build asserts `value < mod`.

Modulus `p = 2^61 - 1`, base a random odd number chosen at start-up, so nobody can craft a
collision on purpose. `--verify-mode=double` runs a second independent modulus pair.

> TODO(Anas W2, Step 4): write this up the same day the roller is done, while it is fresh.

## 3. Matching, candidates and scoring  *(Alyan — lands Week 5)*

> TODO(Alyan): KMP over token ids, the candidate filter, and the Jaccard / containment /
> coverage formulas, including which one is the primary suspicion key and why.

## 4. Ranking and the report  *(Alyan — lands Week 6)*

> TODO(Alyan): why a top-k heap *and* an AVL leaderboard, what the AVL buys that the heap does
> not, and how the report gets line numbers from `DocumentView::posToLine` without re-reading
> a single file.

---

## Decisions log

| date | decision | by |
|---|---|---|
| 2026-10-04 | Layout, root CMake and the contract header are frozen after Week 0/1; changes go through an issue, not an edit. | Anas |
| 2026-10-04 | Every area's CMakeLists globs its own sources, so adding a file never edits a shared file. | Anas |
| 2026-10-04 | `OccurrenceList` is `DynArray<Occurrence>` and hits are **appended**, not prepended — cache locality on the hot path. | Anas |
| 2026-10-04 | Scope-cut order if we fall behind: (1) open-addressing benchmark → design paragraph, (2) smaller demo corpus, (3) drop `--format=json`. The index API for Alyan is never delayed. | both |
