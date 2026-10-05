#!/usr/bin/env python3
"""alyan_score.py -- precision / recall of a detection run.

Owner: ALYAN.  Placeholder created by Anas in Week 0; Alyan replaces it freely.

Compares the pairs a report flagged against the generator's ground truth:

    python3 tools/alyan_score.py --truth tests/corpus/alyan/medium/_ground_truth.tsv \
        --flagged findings.tsv
"""
import argparse


def load_pairs(path):
    pairs = set()
    with open(path, "r", encoding="utf-8") as fh:
        for line in fh:
            parts = line.strip().split("\t")
            if len(parts) >= 2 and parts[0] and parts[1]:
                pairs.add(frozenset((parts[0], parts[1])))
    return pairs


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--truth", required=True, help="ground-truth TSV from the generator")
    ap.add_argument("--flagged", required=True, help="TSV of flagged pairs from the report")
    args = ap.parse_args()

    truth = load_pairs(args.truth)
    flagged = load_pairs(args.flagged)

    tp = len(truth & flagged)
    precision = tp / len(flagged) if flagged else 0.0
    recall = tp / len(truth) if truth else 0.0
    f1 = (2 * precision * recall / (precision + recall)) if (precision + recall) else 0.0

    print("| corpus | flagged | true pairs | TP | precision | recall | F1 |")
    print("|---|---|---|---|---|---|---|")
    print("| %s | %d | %d | %d | %.3f | %.3f | %.3f |"
          % (args.truth, len(flagged), len(truth), tp, precision, recall, f1))


if __name__ == "__main__":
    main()
