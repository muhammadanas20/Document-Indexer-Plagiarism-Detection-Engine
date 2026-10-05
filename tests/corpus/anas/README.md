# tests/corpus/anas/ -- corpora Anas's tests index

| dir | size | used by |
|---|---|---|
| `small/` | 6 files | Step 2 walker, Step 6 `plag-detect index tests/corpus/anas/small --stats` |
| `medium/` | generated | Step 8 `--k` sweep (generate with `tools/alyan_gen_corpus.py --files 1000 --seed 7`) |

Generated corpora are NOT committed (see `.gitignore`): they are reproducible
from the generator and a seed, which is the whole point of `--seed`.

Owner: Anas. Alyan's corpora live in `tests/corpus/alyan/`.
