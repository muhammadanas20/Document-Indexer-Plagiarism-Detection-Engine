# Benchmarks — Alyan (`src/search/`, `src/rank/`, `src/report/`)

**Owner: ALYAN.** Anas created this file empty in Week 0 so the layout is complete.

Same discipline as `benchmarks-anas.md`: every number quoted in the report has a line here,
committed in the same PR as the code that produced it. State the machine at the top.

Suggested tables:

* KMP vs naive search — 10 k random texts, ms and speed-up.
* Candidate generation — pairs produced vs pairs verified, per corpus.
* Max-heap vs AVL leaderboard — 100 k insertions, top-100 extraction, ms and peak RSS.
* Accuracy — precision / recall / F1 per corpus, from `tools/alyan_score.py`, at the
  recommended `k`.
