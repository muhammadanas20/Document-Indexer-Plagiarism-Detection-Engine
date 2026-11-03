// test_sort_search.cpp  --  sorting, binary search and the three string
// matching algorithms
// Owner: Anas (sorting) + Alyan (string matching)

#include <string>
#include <vector>

#include "sort.hpp"
#include "string_match.hpp"
#include "test_util.hpp"

using namespace dip;

int main() {
  // ---- merge sort on fingerprints ----------------------------------------
  std::vector<unsigned long long> numbers;
  numbers.push_back(9);
  numbers.push_back(1);
  numbers.push_back(5);
  numbers.push_back(1);
  numbers.push_back(7);
  numbers.push_back(3);
  mergeSortFingerprints(numbers);
  CHECK(numbers.size() == 6);
  CHECK(numbers[0] == 1);
  CHECK(numbers[1] == 1);
  CHECK(numbers[2] == 3);
  CHECK(numbers[3] == 5);
  CHECK(numbers[4] == 7);
  CHECK(numbers[5] == 9);

  std::vector<unsigned long long> alreadySorted;
  alreadySorted.push_back(1);
  alreadySorted.push_back(2);
  alreadySorted.push_back(3);
  mergeSortFingerprints(alreadySorted);           // must not break
  CHECK(alreadySorted[0] == 1 && alreadySorted[2] == 3);

  std::vector<unsigned long long> emptyList;
  mergeSortFingerprints(emptyList);               // empty input is fine
  CHECK(emptyList.empty());

  // ---- quick sort on text -------------------------------------------------
  std::vector<std::string> paths;
  paths.push_back("samples/zainab.txt");
  paths.push_back("samples/ali.cpp");
  paths.push_back("samples/sara.cpp");
  paths.push_back("samples/bilal.cpp");
  quickSortStrings(paths);
  CHECK(paths.size() == 4);
  CHECK(paths[0] == "samples/ali.cpp");           // alphabetical
  CHECK(paths[1] == "samples/bilal.cpp");
  CHECK(paths[2] == "samples/sara.cpp");
  CHECK(paths[3] == "samples/zainab.txt");

  // ---- insertion sort on the top-K list ----------------------------------
  std::vector<ScoreEntry> entries(3);
  entries[0].score = 0.20;
  entries[1].score = 0.95;
  entries[2].score = 0.60;
  insertionSortScores(entries);
  CHECK(entries[0].score == 0.95);                // biggest first
  CHECK(entries[1].score == 0.60);
  CHECK(entries[2].score == 0.20);

  // ---- binary search ------------------------------------------------------
  std::vector<std::string> vocabulary;
  vocabulary.push_back("array");
  vocabulary.push_back("for");
  vocabulary.push_back("loop");
  vocabulary.push_back("sum");
  vocabulary.push_back("total");
  CHECK(binarySearchWord(vocabulary, "array") == 0);
  CHECK(binarySearchWord(vocabulary, "sum") == 3);
  CHECK(binarySearchWord(vocabulary, "total") == 4);
  CHECK(binarySearchWord(vocabulary, "while") == -1);   // not in the list
  CHECK(binarySearchWord(vocabulary, "aaa") == -1);
  CHECK(binarySearchWord(vocabulary, "zzz") == -1);

  // ---- brute force --------------------------------------------------------
  std::string text = "the sum of the total sum is the sum";
  std::vector<std::size_t> brute = bruteForceSearch(text, "sum");
  CHECK(brute.size() == 3);
  CHECK(brute[0] == 4);
  CHECK(brute[1] == 21);
  CHECK(brute.back() == 32);   // "the sum" at the end

  // ---- KMP: same answers, and one match more than brute force on overlap --
  std::vector<std::size_t> kmp = kmpSearch(text, "sum");
  CHECK(kmp.size() == brute.size());
  CHECK(kmp == brute);

  std::vector<std::size_t> overlap = kmpSearch("aaaa", "aa");
  CHECK(overlap.size() == 3);              // overlapping matches are reported

  // The prefix table of "ababaca" is the classic KMP example.
  std::vector<std::size_t> prefix = buildPrefixTable("ababaca");
  CHECK(prefix.size() == 7);
  CHECK(prefix[0] == 0);
  CHECK(prefix[1] == 0);
  CHECK(prefix[2] == 1);
  CHECK(prefix[3] == 2);
  CHECK(prefix[4] == 3);
  CHECK(prefix[5] == 0);
  CHECK(prefix[6] == 1);

  // ---- Rabin-Karp ---------------------------------------------------------
  std::vector<std::size_t> rabin = rabinKarpSearch(text, "sum");
  CHECK(rabin == brute);
  std::vector<std::size_t> rabinLong = rabinKarpSearch(text, "total sum");
  CHECK(rabinLong.size() == 1);
  CHECK(rabinLong[0] == 15);

  // ---- the three algorithms must always agree -----------------------------
  std::string corpus =
      "int sum to int n int total for int i int i int n int total int i";
  const char* patterns[4] = {"int", "int total", "sum to int", "nothing"};
  bool agree = true;
  for (int p = 0; p < 4; ++p) {
    std::string pattern = patterns[p];
    if (bruteForceSearch(corpus, pattern) != kmpSearch(corpus, pattern)) agree = false;
    if (kmpSearch(corpus, pattern) != rabinKarpSearch(corpus, pattern)) agree = false;
  }
  CHECK(agree);

  // ---- edge cases ---------------------------------------------------------
  CHECK(bruteForceSearch("abc", "abcd").empty());   // pattern longer than text
  CHECK(kmpSearch("abc", "abcd").empty());
  CHECK(rabinKarpSearch("abc", "abcd").empty());
  CHECK(kmpSearch("abc", "").empty());              // empty pattern
  CHECK(bruteForceSearch("", "a").empty());         // empty text

  return testutil::summary("test_sort_search");
}
