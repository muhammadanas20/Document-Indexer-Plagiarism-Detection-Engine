// Implementation of document pair similarity metrics
#include "search/scorer.hpp"

#include "search/verify_mode.hpp"

namespace plag {
namespace {

double clamp01(double v) {
  if (v < 0.0) return 0.0;
  if (v > 1.0) return 1.0;
  return v;
}

}  // namespace

double jaccard(std::size_t shared, std::size_t a, std::size_t b) {
  if (a == 0 && b == 0) return 0.0;
  std::size_t total = a + b;
  if (total <= shared) {
    return 1.0;
  }
  std::size_t uni = total - shared;
  if (uni == 0) return 0.0;
  return clamp01(static_cast<double>(shared) / static_cast<double>(uni));
}

double containment(std::size_t shared, std::size_t a) {
  if (a == 0) return 0.0;
  return clamp01(static_cast<double>(shared) / static_cast<double>(a));
}

double coverage(std::size_t matchedTokens, std::size_t shorterDoc) {
  if (shorterDoc == 0) return 0.0;
  return clamp01(static_cast<double>(matchedTokens) /
                 static_cast<double>(shorterDoc));
}

double suspicion_key(const PairScore& p) {
  return 0.50 * p.coverage + 0.30 * p.containment + 0.20 * p.jaccard;
}

PairScore score_pair(DocumentId a, DocumentId b, std::size_t shared,
                     std::size_t tokensA, std::size_t tokensB,
                     std::size_t matchedTokens, std::uint32_t spans) {
  PairScore ps;
  ps.docA = a;
  ps.docB = b;

  std::size_t shorter = (tokensA < tokensB) ? tokensA : tokensB;
  ps.jaccard = jaccard(shared, tokensA, tokensB);
  ps.containment = containment(shared, shorter);
  ps.coverage = coverage(matchedTokens, shorter);
  ps.matchedTokens = static_cast<std::uint32_t>(matchedTokens);
  ps.spans = spans;
  ps.label = static_cast<std::uint8_t>(label_for(ps.coverage));
  return ps;
}

}  // namespace plag
