# Benchmarks — Anas (`src/index/`)

Every number quoted in the report has a line here, committed in the same PR as the code that
produced it. Never quote a number that is not in this file.

Machine: _TODO: fill in — CPU, cores, RAM, OS, compiler version, build type._

## Step 2 — corpus reading  *(Week 1)*

| corpus | files | bytes | ms | MB/s |
|---|---|---|---|---|
| `tests/corpus/anas/small` | 6 | — | — | — |

## Step 5 vs Step 7a — chaining vs open addressing  *(Week 5)*

Produced by `tools/anas_bench_hash.cpp` (10 M inserts, 10 M lookups, 1 adversarial key set).

| variant | 10M inserts | 10M lookups | peak RSS | mean chain/probe | max | rehashes |
|---|---|---|---|---|---|---|
| chaining | — | — | — | — | — | — |
| linear probing | — | — | — | — | — | — |
| probing (adversarial keys) | — | — | — | — | — | — |

This table is the justification for choosing chaining for the engine. Do not write the
conclusion before the numbers are in.

## Step 6 — index build  *(Week 4)*

| corpus | files | tokens | k | distinct | buckets | alpha | mean chain | longest | ms | bytes/token |
|---|---|---|---|---|---|---|---|---|---|---|
| small | — | — | 15 | — | — | — | — | — | — | — |

Budget promised: 50 000 tokens indexed in < 200 ms debug, < 20 ms release; memory <= 6
bytes/token. Record what we really get, not what we hoped.

## Step 8 — `--k` sweep  *(Week 6)*

From `tools/anas_sweep_k.sh` on the medium corpus, with precision/recall from
`tools/alyan_score.py`.

| k | time (ms) | distinct fingerprints | candidate pairs | precision | recall |
|---|---|---|---|---|---|
| 5 | — | — | — | — | — |
| 15 | — | — | — | — | — |
| 25 | — | — | — | — | — |
| 50 | — | — | — | — | — |

The best trade-off becomes the recommended default in the report's conclusion.

## Step 8 — stress and memory  *(Week 6)*

| corpus | size | files | peak RSS | ms | deterministic? |
|---|---|---|---|---|---|
| 1 GB synthetic (`--files 10000 --seed 11`) | — | — | — | — | — |
| medium (1 000 files — what the marker will use) | — | — | — | — | — |

Robustness sweep — each case must return a documented `ErrorCode`, never crash:

| case | expected `ErrorCode` | result |
|---|---|---|
| missing file | `NotFound` | — |
| 0-byte file | `EmptyFile` | — |
| permission denied | `NotReadable` | — |
| directory passed as a file | documented error | — |
| malformed UTF-8 | `Ok`, bytes tokenised | — |
| one 200 MB single line | `Ok` | — |
| symlink to itself | depth cap stops it | — |
| directory with 100 000 empty files | `Ok`, 0 tokens | — |
