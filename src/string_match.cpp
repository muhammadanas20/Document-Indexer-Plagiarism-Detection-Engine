// string_match.cpp  --  see string_match.hpp
// Owner: Alyan

#include "string_match.hpp"

namespace dip {
namespace {

const unsigned long long kRabinBase = 257ULL;
const unsigned long long kRabinMod = 1000000007ULL;  // a prime number

// hash of one character inside the Rabin-Karp rolling hash
unsigned long long charHash(char c) {
  return static_cast<unsigned char>(c);
}

}  // namespace

std::vector<std::size_t> bruteForceSearch(const std::string& text,
                                          const std::string& pattern) {
  std::vector<std::size_t> positions;
  if (pattern.empty() || pattern.size() > text.size()) return positions;

  for (std::size_t start = 0; start + pattern.size() <= text.size(); ++start) {
    std::size_t matched = 0;
    while (matched < pattern.size() &&
           text[start + matched] == pattern[matched]) {
      ++matched;
    }
    if (matched == pattern.size()) positions.push_back(start);
  }
  return positions;
}

std::vector<std::size_t> buildPrefixTable(const std::string& pattern) {
  // prefix[i] = length of the longest proper prefix of pattern[0..i]
  //             that is also a suffix of pattern[0..i]
  std::vector<std::size_t> prefix(pattern.size(), 0);
  std::size_t length = 0;  // length of the current matching prefix
  for (std::size_t i = 1; i < pattern.size(); ++i) {
    while (length > 0 && pattern[i] != pattern[length]) {
      length = prefix[length - 1];  // fall back, do not restart from zero
    }
    if (pattern[i] == pattern[length]) ++length;
    prefix[i] = length;
  }
  return prefix;
}

std::vector<std::size_t> kmpSearch(const std::string& text,
                                   const std::string& pattern) {
  std::vector<std::size_t> positions;
  if (pattern.empty() || pattern.size() > text.size()) return positions;

  std::vector<std::size_t> prefix = buildPrefixTable(pattern);
  std::size_t matched = 0;  // how many characters of the pattern match right now

  for (std::size_t i = 0; i < text.size(); ++i) {
    while (matched > 0 && text[i] != pattern[matched]) {
      matched = prefix[matched - 1];  // skip ahead using the prefix table
    }
    if (text[i] == pattern[matched]) ++matched;
    if (matched == pattern.size()) {
      positions.push_back(i + 1 - pattern.size());
      matched = prefix[matched - 1];  // look for overlapping matches too
    }
  }
  return positions;
}

std::vector<std::size_t> rabinKarpSearch(const std::string& text,
                                         const std::string& pattern) {
  std::vector<std::size_t> positions;
  if (pattern.empty() || pattern.size() > text.size()) return positions;

  const std::size_t m = pattern.size();

  // highPower = base^(m-1) mod prime, needed to remove the leaving character.
  unsigned long long highPower = 1ULL;
  for (std::size_t i = 0; i + 1 < m; ++i) {
    highPower = (highPower * kRabinBase) % kRabinMod;
  }

  unsigned long long patternHash = 0ULL;
  unsigned long long windowHash = 0ULL;
  for (std::size_t i = 0; i < m; ++i) {
    patternHash = (patternHash * kRabinBase + charHash(pattern[i])) % kRabinMod;
    windowHash = (windowHash * kRabinBase + charHash(text[i])) % kRabinMod;
  }
  if (windowHash == patternHash) positions.push_back(0);

  for (std::size_t start = 1; start + m <= text.size(); ++start) {
    // Roll the window: remove the character that left, add the new one.
    unsigned long long leaving = (charHash(text[start - 1]) * highPower) % kRabinMod;
    windowHash = (windowHash + kRabinMod - leaving) % kRabinMod;
    windowHash = (windowHash * kRabinBase + charHash(text[start + m - 1])) % kRabinMod;

    if (windowHash == patternHash) {
      // Same hash does not prove a match (collision), so check the text.
      bool equal = true;
      for (std::size_t k = 0; k < m; ++k) {
        if (text[start + k] != pattern[k]) {
          equal = false;
          break;
        }
      }
      if (equal) positions.push_back(start);
    }
  }
  return positions;
}

}  // namespace dip
