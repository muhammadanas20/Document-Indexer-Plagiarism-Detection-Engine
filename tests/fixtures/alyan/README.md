# tests/fixtures/alyan/ -- Alyan's fixtures

The 20 tokenizer/matcher fixtures Alyan writes in Week 1-2 (Step 3 expects them
as the source of truth for the tokenizer tests).

Naming: `<nn>_<what>.txt` next to `<nn>_<what>.expected` holding the expected
token list, one token per line.

Suggested set, from the implementation plan:

1. `a+b;` vs `a + b ;` -- equal in the normalised view, different byte offsets
2. `int x=1;` vs `int y=1;` -- equal only with `--normalize=case`
3. a whole-comment file -- 0 tokens
4. a string running off the end -- exactly 1 token + the `malformed` flag
5. the rest: KMP patterns with known LPS tables and known match positions

Owner: ALYAN. Anas created this folder and this README in Week 0 and does not
add fixtures here.
