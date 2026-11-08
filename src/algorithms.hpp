#ifndef PROJECT_ALGORITHMS_HPP
#define PROJECT_ALGORITHMS_HPP

#include <cstddef>
#include <string>
#include <vector>
#include "engine.hpp"

// sort
void mergeSortFingerprints(std::vector<unsigned long long>& keys);

void quickSortStrings(std::vector<std::string>& items);

void mergeSortPairs(std::vector<PairResult>& pairs);

void insertionSortScores(std::vector<ScoreEntry>& entries);

int binarySearchWord(const std::vector<std::string>& sortedWords,
                     const std::string& word);

// string match
std::vector<std::size_t> bruteForceSearch(const std::string& text,
                                          const std::string& pattern);

std::vector<std::size_t> kmpSearch(const std::string& text,
                                   const std::string& pattern);

std::vector<std::size_t> rabinKarpSearch(const std::string& text,
                                         const std::string& pattern);

std::vector<std::size_t> buildPrefixTable(const std::string& pattern);

#endif
