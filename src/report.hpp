#ifndef PROJECT_REPORT_HPP
#define PROJECT_REPORT_HPP

#include <ostream>
#include <string>
#include <vector>
#include "engine.hpp"

// report
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

#endif
