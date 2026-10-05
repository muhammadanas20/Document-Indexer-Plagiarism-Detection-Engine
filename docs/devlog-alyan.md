# Devlog — Alyan (Member 2)

Private by owner, so it can never clash with Anas's file. **Owner: ALYAN** — Anas created this
empty file in Week 0 so the layout is complete, and does not write in it.

Suggested format, same as Anas's:

```
## YYYY-MM-DD  (Week N)
- did:
- measured:
- blocked / owed:
- next:
```

## Week 0 setup (from section 7 of the plan)

```bash
# fork anas-rz/plag-engine on GitHub first, then:
git clone https://github.com/alyan/plag-engine.git && cd plag-engine
git remote rename origin alyan
git remote add upstream https://github.com/anas-rz/plag-engine.git
git remote -v            # must show exactly two remotes: upstream + origin
git fetch upstream && git reset --hard upstream/main
cmake --preset debug && cmake --build build && ctest --test-dir build --output-on-failure
```

If that last line is not green, it is bug #1 and it is Anas's to fix — say so before writing
any code.
