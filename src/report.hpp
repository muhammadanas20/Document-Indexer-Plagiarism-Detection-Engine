// report.hpp  --  printing the results
// Owner: Alyan
//
// All the printing lives here so that main.cpp stays short: main asks for
// something, report.cpp formats it. Every function writes to an std::ostream,
// so the same output can go to the screen or to a file (we open a file with
// an ofstream and pass it in).

#ifndef DIP_REPORT_HPP
#define DIP_REPORT_HPP

#include <ostream>
#include <string>
#include <vector>

#include "heap.hpp"
#include "index.hpp"
#include "similarity.hpp"

namespace dip {

void printLine(std::ostream& out, char character, int count);

void printIndexStats(std::ostream& out, const Index& index);

void printSearchHits(std::ostream& out, const Index& index,
                     const std::string& word,
                     const std::vector<SearchHit>& hits);

void printPairs(std::ostream& out, const Index& index,
                const std::vector<PairResult>& pairs, bool withSpans);

void printTopK(std::ostream& out, const Index& index,
               const std::vector<ScoreEntry>& entries);

void printLeaderboard(std::ostream& out, const Index& index,
                      const std::vector<std::pair<double, std::pair<int, int> > >& ranked);

void printClusters(std::ostream& out, const Index& index,
                   const std::vector<std::vector<int> >& clusters);

void printVocabulary(std::ostream& out, const std::vector<std::string>& words,
                     std::size_t limit);

}  // namespace dip

#endif  // DIP_REPORT_HPP
