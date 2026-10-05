# tasks.md — the shared task list

Section 5 of the implementation plans, pasted in with checkboxes. **Tick the boxes in this
file as you go, so the record lives in the repo and not in our heads.** Each of us ticks only
our own rows.

The last line of each week is the demo — that is what "done" means for that week, and it is
run on Sunday night by Anas (or Alyan in the last weeks).

Key dates: Week 0 = study (4–11 Oct) · build weeks Mon 12 Oct → Sun 22 Nov · **freeze Fri
20 Nov 2026** · **submission Sat 21 Nov 2026** · Week 7 = wrap-up and viva.

**The one hard deadline:** Alyan needs the index API (`lookup()`) and the line map by the end
of Week 4, **Sun 8 Nov**.

---

## Step → branch → PR map (Anas)

| Step | Due | Branch to push | PR title |
|---|---|---|---|
| `src/contract/` + `src/common/` stubs | Sun 18 Oct | `week01-contract` | `W1 contract: shared types + stubs` |
| `src/index/` file reading | Sun 18 Oct | `week01-corpus` | `W1 index: buffer + reader + walker` |
| `src/index/tokenizer`, `pos_to_line` | Sun 25 Oct | `week02-tokenizer` | `W2 index: tokenizer + posToLine` |
| `src/index/rolling_hash` | Sun 25 Oct | `week02-rolling-hash` | `W2 index: rolling hash + property test` |
| `src/index/hash_table` (chaining) | Sun 1 Nov | `week03-hash-table` | `W3 index: chaining hash table + grow tests` |
| `src/index/index_builder` + stats | Sun 8 Nov | `week04-index` | `W4 index: IndexBuilder, stats, lookup API` |
| `hash_table_open`, `bench_hash`, `span_merge` | Sun 15 Nov | `week05-open-addressing` / `week05-spans` | `W5 index: probing table + bench, span merge` |
| stress + memory + support | Sun 22 Nov | `week06-hardening` | `W6 index: stress, k-sweep, memory` |

---

## Week 0 — Sun 4 Oct – Sun 11 Oct

- [x] Create the repo (private), push the layout from section 4 exactly, `main` as default
- [ ] Add Alyan as collaborator with Write; turn on branch protection on `main`
- [x] Write root `CMakeLists.txt` with one `add_subdirectory` per area, and an empty
      `CMakeLists.txt` in every area using `file(GLOB)`
- [x] Create `src/common/dyn_array.hpp`, `fnv.hpp`, `mulmod.hpp` as working, tested little
      pieces — this is the Week 0 homework and it saves a week later
- [x] Add `.github/workflows/ci.yml` (`make check` on push + PR) and `docs/tasks.md`
- [ ] Study list items 1–6
- [ ] 3 commits minimum this week even for setup work — the contribution graph starts now

**◆ Sunday demo / proof:** Alyan clones, runs `cmake --preset debug && cmake --build build &&
ctest` and it is green on his machine.

---

## Week 1 — Mon 12 Oct – Sun 18 Oct
*commit minimum: 3 (Mon / Wed / Fri), PR open by Fri, merged by Sun 23:00*

- [ ] Step 1: `src/contract/` types + stub classes for both of us; tag `contract-v1`
      (branch `week01-contract`)
- [ ] Step 2: `dynamic_buffer`, `file_reader`, `dir_walker` with tests
      `test_anas_fileio.cpp` (branch `week01-corpus`)
- [ ] Open the two issues Alyan listed after reading his section 5; assign them to yourself
      with the `blocked` label
- [ ] PR + review Alyan's harness PR, merge Sunday night

**◆ Sunday demo / proof:** the Sunday demo line for W1 runs on a clean clone; both PRs of the
week are merged.

---

## Week 2 — Mon 19 Oct – Sun 25 Oct
*commit minimum: 3 (Mon / Wed / Fri), PR open by Fri, merged by Sun 23:00*

- [ ] Step 3: tokenizer incl. `pos_to_line` (branch `week02-tokenizer`)
- [ ] Step 4: `rolling_hash` + property test (branch `week02-rolling-hash`)
- [ ] Write 2 lines in `docs/design.md` §1 explaining the two token views — that text goes
      straight into the report

**◆ Sunday demo / proof:** both new test files green in CI; you can state why the roll is
O(1) in one sentence.

---

## Week 3 — Mon 26 Oct – Sun 1 Nov
*commit minimum: 3 (Mon / Wed / Fri), PR open by Fri, merged by Sun 23:00*

- [ ] Step 5 part 1: `hash_table` (chaining, templated) + the shared suite in
      `tests/hash_table_common.h` (branch `week03-hash-table`)
- [ ] Grow/rehash test and the adversarial-key test (keys sharing low bits)
- [ ] If Alyan is behind on KMP: offer to take the naive-search oracle test off his plate
      (issue comment, one commit)

**◆ Sunday demo / proof:** 1 M insert/lookup matches an `std::unordered_map` oracle in the
test; CI green.

---

## Week 4 — Mon 2 Nov – Sun 8 Nov  ⚠ the hard deadline
*commit minimum: 3 (Mon / Wed / Fri), PR open by Fri, merged by Sun 23:00*

- [ ] Step 6: `index_builder` + `--stats` output (branch `week04-index`)
- [ ] Document the API Alyan calls: `lookup(hash, &count)` and
      `DocumentView{norm, posToLine}` — in the header, and in his `tasks.md` row
- [ ] Mid-project check-in: if you are ≥ 1 step behind, cut the open-addressing variant to
      "design only" and say so in the report (do not delay the index for it)

**◆ Sunday demo / proof:** indexing a 1 000-file corpus prints plausible stats in under a
second, and Alyan's candidate finder already compiles against your API.

---

## Week 5 — Mon 9 Nov – Sun 15 Nov
*commit minimum: 3 (Mon / Wed / Fri), PR open by Fri, merged by Sun 23:00*

- [ ] Step 7a: `hash_table_open` (linear probing, tombstones) run through the same shared
      suite (branch `week05-open-addressing`)
- [ ] Step 7b: `tools/anas_bench_hash.cpp` → time, peak RSS, mean/max chain vs probe length,
      both tables, plus a nasty key set
- [ ] Step 7c: `span_merge` (hits → disjoint maximal spans) + merge tests
      (branch `week05-spans`)
- [ ] Review Alyan's heap + AVL PR properly — 45 minutes, not 5, because the AVL is the viva
      risk

**◆ Sunday demo / proof:** benchmark table committed to `docs/benchmarks-anas.md`; every
number quoted in the report has a line in it.

---

## Week 6 — Mon 16 Nov – Thu 19 Nov · final integration and hardening
*commit minimum: 3 (Mon / Wed / Fri), PR open by Fri, merged by Sun 23:00*

- [ ] Hardening: 1 GB synthetic corpus, `--k` sweep 5/15/25/50, malformed UTF-8, memory peak
      (branch `week06-hardening`)
- [ ] Fix everything that integration exposed in your index; nothing in Alyan's files
- [ ] Run the accuracy script on every corpus, paste results into
      `docs/benchmarks-anas.md` (the shared numbers go to the report)
- [ ] Sunday 22 Nov: submission was Sat 21 Nov — from now on only reviewed fixes, both of you

**◆ Sunday demo / proof:** submission-quality build:
`git clone && make check && plag-detect tests/corpus -o r.txt` works for a stranger.

---

## Week 7 — Fri 20 Nov – Sun 22 Nov
*freeze Fri 20 Nov 2026, submit Sat 21 Nov 2026, then viva prep*

- [ ] Write report §7–§8 and your viva part (rolling hash + hash table + why not the STL)
- [ ] Package: source archive / zip of the tagged commit `W7-final`, README checked, slides
      for your slides
- [ ] Submit Sat 21 Nov — if the course deadline differs, shift the whole table by the same
      number of days

**◆ Sunday demo / proof:** everything reproducible from the tagged commit; rehearsal done
once, timed.

---

## Alyan's rows

> Paste section 5 of **Alyan's** implementation plan here, with the same checkbox format, so
> both timelines live in one file. His branch names for the same weeks are `week00-harness`,
> `week01-harness`, `week02-kmp`, `week03-heap`, `week04-avl`, `week05-candidates` /
> `week05-scorer`, `week06-cli-report`.

- [ ] _to be pasted from Alyan's guide_

---

## What is handed over, and what we are waiting for

| Direction | When | The exact thing |
|---|---|---|
| To Alyan | end of Week 1 | Frozen `src/contract/` + stubs, so his classes compile from day one |
| To Alyan | end of Week 4 | `lookup(hash,&count)`, `DocumentView{norm,posToLine}`, and the `--stats` struct for the report header — **the hard deadline** |
| To Alyan | Week 5 | `span_merge`, so his verification hands spans to the report in the agreed shape |
| To the repo | every Sunday | Merged PRs, `tasks.md` ticked, next week's branches created, tag `W<n>-demo` |

**If it is late:** never sit and wait. Every step lists a stand-in to build against
(`fake_index`, hand-written scores, a stubbed CLI). Move to the next step, note the dependency
here, and mention it in the Sunday call.

---

## Definition of Done for one step

Copy this into every PR description. All six lines must be true:

- [ ] It compiles with `-Wall -Wextra -Wconversion` and zero warnings.
- [ ] Its test exists in the repo and passes in the sanitize build.
- [ ] It ran on the sample corpus without crashing, inside the promised time budget.
- [ ] The header says in one comment what the class owns and who owns it.
- [ ] The PR is merged to `main` and the week's demo line was run once by the other person.
- [ ] Every number measured (ms, MB, chain length, recall) is in `docs/benchmarks-anas.md` or
      `docs/benchmarks-alyan.md` in the same PR.
