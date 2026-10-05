#!/usr/bin/env python3
"""alyan_gen_corpus.py -- synthetic corpus generator.

Owner: ALYAN.  Placeholder created by Anas in Week 0 so the layout is complete;
Alyan replaces it freely.

Reproducible from a seed, which is why generated corpora are git-ignored: the
commit stores the command, not 1 GB of text.

    python3 tools/alyan_gen_corpus.py --out tests/corpus/alyan/medium \
        --files 1000 --seed 7 --plagiarism 0.2
    python3 tools/alyan_gen_corpus.py --out /tmp/big --files 10000 --seed 11
"""
import argparse
import os
import random

WORDS = ("alpha beta gamma delta epsilon zeta eta theta iota kappa lambda mu "
         "nu xi omicron pi rho sigma tau upsilon phi chi psi omega").split()


def make_doc(rng, n_tokens):
    return [rng.choice(WORDS) for _ in range(n_tokens)]


def plagiarise(rng, doc, fraction):
    """Copy a contiguous block and rename identifiers, the way students do."""
    if not doc or fraction <= 0:
        return list(doc)
    start = rng.randrange(len(doc))
    length = max(1, int(len(doc) * fraction))
    block = doc[start:start + length]
    out = list(doc)
    out[start:start + length] = [w[::-1] for w in block]
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--out", required=True, help="output directory")
    ap.add_argument("--files", type=int, default=100, help="number of files")
    ap.add_argument("--seed", type=int, default=1, help="RNG seed (reproducibility)")
    ap.add_argument("--tokens", type=int, default=400, help="tokens per file")
    ap.add_argument("--plagiarism", type=float, default=0.2,
                    help="fraction of files that copy an earlier one")
    args = ap.parse_args()

    rng = random.Random(args.seed)
    os.makedirs(args.out, exist_ok=True)

    originals = []
    ground_truth = []
    for i in range(args.files):
        if originals and rng.random() < args.plagiarism:
            src = rng.randrange(len(originals))
            doc = plagiarise(rng, originals[src], 0.6)
            ground_truth.append((src, i))
        else:
            doc = make_doc(rng, args.tokens)
            originals.append(doc)
        path = os.path.join(args.out, "doc_%05d.txt" % i)
        with open(path, "w", encoding="utf-8") as fh:
            fh.write(" ".join(doc) + "\n")

    # Ground truth feeds tools/alyan_score.py for precision/recall.
    with open(os.path.join(args.out, "_ground_truth.tsv"), "w",
              encoding="utf-8") as fh:
        for a, b in ground_truth:
            fh.write("doc_%05d.txt\tdoc_%05d.txt\n" % (a, b))

    print("wrote %d files to %s (%d plagiarised pairs)"
          % (args.files, args.out, len(ground_truth)))


if __name__ == "__main__":
    main()
