# Pushing this to GitHub — the exact commands

Everything below is section 2 and section 7 of the implementation plan, in the order to run
them. The repository folder is already laid out; this is the part that has to happen on your
machine and on github.com.

## 1. Anas — owner side (do this first, ~45 minutes, then it is frozen)

```bash
cd plag-engine                       # this folder

git init -b main
git add .
git commit -m "chore: repo layout + frozen cmake (Anas W0)"

# create the PRIVATE repo anas-rz/plag-engine on github.com first, then:
git remote add origin https://github.com/anas-rz/plag-engine.git
git push -u origin main

# weekly work lands on dev; main only gets the Sunday merges
git switch -c dev
git push -u origin dev

# tag the frozen contract once Step 1 is in
git tag -a contract-v1 -m 'contract: shared types + stubs'
git push --tags
```

## 2. Settings to click on GitHub (2 minutes, do not skip)

1. **Settings → Collaborators** → add Alyan with **Write** access. A *contributor* is just
   someone with commits; without Write access Alyan could not push branches.
2. **Settings → Branches → Add branch protection rule** for `main`:
   * Require a pull request before merging
   * Require status checks to pass → pick **`ci / test`**
   * Do not allow force pushes
   * Do not allow deletions
   * Optionally: *Restrict who can push to matching branches* → nobody, so even you go
     through a PR.
3. **Insights → Projects** → board with columns *Backlog, This week, In progress, Review,
   Done*. Every task row in `docs/tasks.md` becomes one card, assigned and dated.
4. **Actions** tab → enable workflows. `ci.yml` runs `make check` on every push and PR.
5. Switch the repo to public only if the course asks.

## 3. Alyan — contributor side (his own terminal, same day)

```bash
# fork anas-rz/plag-engine on github.com first, then:
git clone https://github.com/alyan/plag-engine.git && cd plag-engine
git remote rename origin alyan
git remote add upstream https://github.com/anas-rz/plag-engine.git
git remote -v                      # must show exactly two: upstream + origin
git fetch upstream && git reset --hard upstream/main

git switch -c week00-harness upstream/main

cmake --preset debug && cmake --build build && ctest --test-dir build --output-on-failure
```

That last line must be green before he writes anything. If it is not, that is bug #1 and it is
Anas's to fix.

Always start a branch from `upstream/main`, never from the fork's stale `main`.

## 4. The weekly cycle (both)

```bash
git fetch upstream                                  # Alyan only
git switch -c week03-hash-table upstream/main       # Anas: origin/main
# ... work, commit at Mon / Wed / Fri ...

cmake --preset sanitize
cmake --build build-sanitize --target check         # must be green before pushing

git push -u origin week03-hash-table                # Alyan pushes to his fork
```

**PR:** base `anas-rz/plag-engine:main` ← head `alyan:week03-hash-table`
(or `anas-rz:week03-hash-table` for Anas).
Title: `W3 index: chaining hash table + grow test` — always `WeekN area: what`.
Body: 3 lines — what you did · how to check it (the ctest output line) · what you still owe.
Add `Closes #14` if it answers an issue. Labels: `ana` or `aly` plus `W3`.

**Merging:** review within 24 h → **Squash and merge** → delete the branch on the merge
dialog. Then Alyan resyncs:

```bash
git fetch upstream && git switch main && git reset --hard upstream/main
git push --force origin main
```

**Tags:** Anas tags each demo — `git tag -a W3-demo -m 'index demo' && git push --tags`.

## 5. Commit message style (so the log reads like a report)

```
index: chaining hash table with grow at alpha 0.75 (Anas W3)
search: kmp lps + findAll, naive-oracle test (Alyan W2)
rank: max-heap top_k, descending-array case (Alyan W3)
tests: 20 tokenizer fixtures with expected token lists (Alyan W1)
docs: bench_hash chaining vs probing numbers (Anas W5)
```

Prefix = the area (= the owner), then what changed, then who and which week.

## 6. If `main` moves under you

```bash
git fetch upstream && git rebase upstream/main
git push --force-with-lease origin <your-branch>
```

Never a merge commit inside your own branch — it makes the PR unreadable. Never force-push
`main`.
