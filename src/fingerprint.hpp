// fingerprint.hpp  --  k-gram fingerprints with a rolling hash
// (Week 12 topic: Rabin-Karp / rolling hash  +  Week 13: hashing)
// Owner: Anas
//
// The idea: take every window of k consecutive words of a document and turn it
// into one number (a "fingerprint"). Two documents that share many fingerprints
// share many sentences, so they are candidates for plagiarism.
//
// Why fingerprints and not whole sentences? A fingerprint is 8 bytes instead of
// a string, and the number of fingerprints of a document is linear in its
// length, so comparing two documents becomes cheap.
//
// The rolling hash makes it O(1) per window instead of O(k):
//   H = (((w0 * b + w1) * b + w2) ... ) mod p
//   slide: H' = ((H - w_old * b^(k-1)) * b + w_new) mod p

#ifndef DIP_FINGERPRINT_HPP
#define DIP_FINGERPRINT_HPP

#include <string>
#include <vector>

namespace dip {

// k = window size in words (the program uses k = 5 by default).
// Returns the DISTINCT fingerprints of the document, sorted ascending, so two
// documents can be compared with a simple two-pointer walk in O(n + m).
std::vector<unsigned long long> computeFingerprints(
    const std::vector<std::string>& words, int k);

}  // namespace dip

#endif  // DIP_FINGERPRINT_HPP
