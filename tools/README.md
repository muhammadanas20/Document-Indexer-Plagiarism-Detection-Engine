# tools/ -- helpers, prefixed by owner

Every file here carries its owner's prefix, so nobody shares a filename with
anybody and a glob never has to be edited.

| file | owner | what it does |
|---|---|---|
| `anas_bench_hash.cpp` | Anas | chaining vs open addressing: ms, peak RSS, chain/probe stats |
| `anas_sweep_k.sh` | Anas | `--k` sweep 5/15/25/50 -> the recommended default |
| `alyan_gen_corpus.py` | Alyan | synthetic corpora from a seed (reproducible, git-ignored) |
| `alyan_score.py` | Alyan | precision / recall of a detection run |

The `.cpp` tools are picked up by `tools/CMakeLists.txt`'s glob automatically.
Shell and Python helpers are run directly.
