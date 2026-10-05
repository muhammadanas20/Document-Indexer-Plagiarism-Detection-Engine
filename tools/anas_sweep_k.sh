#!/usr/bin/env bash
# anas_sweep_k.sh -- --k sweep 5 / 15 / 25 / 50 on the medium corpus.
# Owner: Anas (Steps 7b + 8). Output goes into docs/benchmarks-anas.md; the
# best trade-off becomes the recommended default in the report's conclusion.
#
# STUB: plag-detect's `index` subcommand lands at Step 6, so this currently
# prints the command it would run instead of real numbers.
set -euo pipefail

CORPUS="${1:-tests/corpus/anas/small}"
BUILD="${BUILD:-build/bin}"
OUT="${2:-docs/benchmarks-anas.md}"

echo "| k | time (ms) | distinct fingerprints | candidate pairs | precision | recall |"
echo "|---|---|---|---|---|---|"

for k in 5 15 25 50; do
  if [ -x "${BUILD}/plag-detect" ]; then
    # TODO(Anas W6 Step 8): capture --stats, then feed the report to
    # tools/alyan_score.py for precision/recall against the ground truth.
    start=$(date +%s%N)
    "${BUILD}/plag-detect" index "${CORPUS}" --k "${k}" --stats >/dev/null 2>&1 || true
    end=$(date +%s%N)
    ms=$(( (end - start) / 1000000 ))
    echo "| ${k} | ${ms} | TODO | TODO | TODO | TODO |"
  else
    echo "| ${k} | - | - | - | - | - |"
  fi
done | tee -a "${OUT}"

echo
echo "wrote the k-sweep table to ${OUT}"
