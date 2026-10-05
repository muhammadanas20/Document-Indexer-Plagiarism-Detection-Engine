# tests/fixtures/anas/ -- Anas's fixtures

Hand-made snippets for `src/index/` with the expected result written down next
to them, so a test can be a golden comparison instead of an opinion.

| fixture | what it pins down | expected |
|---|---|---|
| `simple.txt` | byte-exact read, LF endings | 6 tokens, 3 lines |
| `crlf.txt` | CRLF -> LF trimming on read | same tokens as `simple.txt` |
| `empty.txt` | zero-byte file | `ErrorCode::EmptyFile` |
| `no_newline.txt` | file with no trailing newline | last token still on the right line |
| `comments_only.cpp` | comments produce no tokens | 0 tokens |
| `unterminated_string.cpp` | malformed input | 1 token + `malformed` flag |
| `line42.cpp` | token -> line map | token on line 42 reports 42 |

Owner: Anas. Alyan's fixtures live in `tests/fixtures/alyan/`; neither of us
adds to the other's folder.
