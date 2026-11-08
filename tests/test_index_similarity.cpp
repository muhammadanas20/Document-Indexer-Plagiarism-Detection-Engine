#include <string>
#include <vector>

#include "engine.hpp"
#include "documents.hpp"
#include "test_util.hpp"

int main() {

  std::string code =
      "// this comment must disappear\n"
      "int Sum(int a) {\n"
      "  return a + 1;   // so must this one\n"
      "}\n";
  TokenizedText tokens = tokenize(code, true);
  CHECK(tokens.words.size() == 7);
  CHECK(tokens.words[0] == "int");
  CHECK(tokens.words[1] == "sum");
  CHECK(tokens.lines[1] == 2);
  CHECK(tokens.lines[5] == 3);

  TokenizedText text = tokenize("see http://example.com/page", false);
  CHECK(text.words.size() == 5);

  TokenizedText block = tokenize("a /* hidden words here */ b", true);
  CHECK(block.words.size() == 2);
  CHECK(block.words[0] == "a");
  CHECK(block.words[1] == "b");

  TokenizedText joined = tokenize("one/* skip */two", true);
  CHECK(joined.words.size() == 2);
  CHECK(joined.words[0] == "one");
  CHECK(joined.words[1] == "two");

  Document first;
  first.words = {"one", "two", "three"};
  first.wordLines = {1, 2, 3};
  first.buildNormalized();
  CHECK(first.wordStart[1] == 4);
  CHECK(first.lineOfPosition(4) == 2);
  Document second;
  second.words = {"someone", "two", "three"};
  second.wordLines = {1, 2, 3};
  second.buildNormalized();
  CHECK(findCopiedSpans(first, second, 3).empty());
  second.words[0] = "one";
  second.buildNormalized();
  CHECK(findCopiedSpans(first, second, 3).size() == 1);

  std::vector<Document> documents;
  std::string error;
  bool loaded = loadCorpus(SAMPLES_DIR, documents, error);
  if (!loaded) std::cout << "  loader error: " << error << "\n";
  CHECK(loaded);
  CHECK(documents.size() == 6);

  CHECK(documents[0].path.find("ali.cpp") != std::string::npos);
  CHECK(documents[1].path.find("bilal.cpp") != std::string::npos);
  CHECK(documents[0].wordCount() > 30);

  bool linesOk = documents[0].wordLines.size() == documents[0].wordCount();
  for (std::size_t i = 1; i < documents[0].wordLines.size(); ++i) {
    if (documents[0].wordLines[i] < documents[0].wordLines[i - 1]) linesOk = false;
    if (documents[0].wordLines[i] < 1) linesOk = false;
  }
  CHECK(linesOk);

  Index index;
  index.build(documents, 5);
  CHECK(index.documentCount() == 6);
  CHECK(index.totalWords() > 100);
  CHECK(index.uniqueWordCount() > 20);
  CHECK(index.loadFactor() <= 0.75);
  CHECK(index.longestChain() < 30);
  CHECK(index.averageProbes() >= 1.0);

  std::vector<SearchHit> hits = index.searchWord("total");
  CHECK(hits.size() >= 2);
  bool aliFound = false;
  for (std::size_t i = 0; i < hits.size(); ++i) {
    if (hits[i].documentId == 0) aliFound = true;
    CHECK(hits[i].firstLine > 0);
    CHECK(hits[i].count >= 1);
  }
  CHECK(aliFound);

  CHECK(index.searchWord("zzzzz").empty());

  std::vector<std::string> vocabulary = index.sortedVocabulary();
  CHECK(vocabulary.size() == index.uniqueWordCount());
  bool alphabetical = true;
  for (std::size_t i = 1; i < vocabulary.size(); ++i) {
    if (vocabulary[i - 1] > vocabulary[i]) alphabetical = false;
  }
  CHECK(alphabetical);
  CHECK(index.documentFrequency("total") >= 2);
  CHECK(index.documentFrequency("zzzzz") == 0);

  std::vector<unsigned long long> aliGrams = index.fingerprints()[0];
  std::vector<unsigned long long> bilalGrams = index.fingerprints()[1];
  std::vector<unsigned long long> saraGrams = index.fingerprints()[4];
  CHECK(!aliGrams.empty());
  CHECK(!bilalGrams.empty());

  bool gramsSorted = true;
  for (std::size_t i = 1; i < aliGrams.size(); ++i) {
    if (aliGrams[i - 1] > aliGrams[i]) gramsSorted = false;
  }
  CHECK(gramsSorted);

  int aliBilal = countSharedFingerprints(aliGrams, bilalGrams);
  int aliSara = countSharedFingerprints(aliGrams, saraGrams);
  CHECK(aliBilal > aliSara);

  double score = jaccardSimilarity(aliGrams, bilalGrams);
  CHECK(score > 0.30);
  CHECK(score <= 1.0);
  CHECK(jaccardSimilarity(aliGrams, saraGrams) < 0.20);
  CHECK(jaccardSimilarity(aliGrams, aliGrams) == 1.0);

  std::vector<MatchedSpan> spans =
      findCopiedSpans(documents[0], documents[1], 6);
  CHECK(!spans.empty());
  int copiedWords = 0;
  for (std::size_t i = 0; i < spans.size(); ++i) {
    copiedWords += spans[i].words;
    CHECK(spans[i].firstStartLine >= 1);
    CHECK(spans[i].firstEndLine >= spans[i].firstStartLine);
    CHECK(spans[i].secondStartLine >= 1);
  }
  CHECK(copiedWords > 20);

  std::vector<PairResult> pairs = compareAll(index, 1);
  CHECK(!pairs.empty());
  bool aliBilalPair = false;
  for (std::size_t i = 0; i < pairs.size(); ++i) {
    if (pairs[i].firstDoc == 0 && pairs[i].secondDoc == 1) aliBilalPair = true;
  }
  CHECK(aliBilalPair);
  CHECK(std::string(suspicionLabel(0.95)) == "CRITICAL");
  CHECK(std::string(suspicionLabel(0.45)) == "HIGH");
  CHECK(std::string(suspicionLabel(0.05)) == "NONE");

  index.build(documents, 5);
  CHECK(index.totalWords() > 100);
  CHECK(index.documentFrequency("total") >= 2);
  CHECK(index.searchWord("total").size() == hits.size());
  CHECK(index.sortedVocabulary().size() == vocabulary.size());
  return testSummary("test_index_similarity");
}
