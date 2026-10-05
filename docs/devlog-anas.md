# Devlog — Anas (Member 1, Group Leader)

Private by owner, so it can never clash with Alyan's file. One short entry per working day:
what was done, what was measured, what is blocked. This is the raw material for report §7-§8.

Format:

```
## YYYY-MM-DD  (Week N)
- did:
- measured:
- blocked / owed:
- next:
```

---

## 2026-10-04  (Week 0)

- did: created the repo skeleton from section 4 of the implementation plan — every directory,
  every area's `CMakeLists.txt` with `file(GLOB)`, the root build, `CMakePresets.json`
  (debug / sanitize / release), `.github/workflows/ci.yml`, `.gitignore`, `.clang-format`,
  `.gitattributes`.
- did: `src/contract/` (contract.hpp, span.hpp, error_code.hpp) and `src/common/`
  (dyn_array, fnv, mulmod + `muls_mod.hpp` alias, pos_to_line) as working, tested pieces —
  this is the Week 0 homework.
- did: stub headers with final signatures and dummy bodies for every class in **both** areas,
  so Alyan compiles from day one.
- did: `tests/harness.hpp` (header-only, STL allowed in tests as the oracle) + one test
  executable per `test_*.cpp`, globbed.
- measured: `cmake --preset debug && cmake --build build && ctest` green — see the commit.
- blocked / owed: Alyan needs to be added as a collaborator with Write access; branch
  protection on `main`; `dev` branch created; the Projects board.
- next: tag `contract-v1`, open the branch `week01-contract`, then `week01-corpus`.
