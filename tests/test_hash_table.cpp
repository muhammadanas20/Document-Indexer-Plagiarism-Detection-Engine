#include <string>

#include "tables.hpp"
#include "test_util.hpp"

int main() {

  CHECK(hashKey("sum") == hashKey("sum"));
  CHECK(hashKey("sum") != hashKey("total"));
  CHECK(hashKey(7ULL) == 7ULL);

  HashTable<std::string, int> table;
  CHECK(table.size() == 0);
  CHECK(table.get("sum") == nullptr);

  table.put("sum", 3);
  table.put("total", 1);
  table.put("loop", 2);
  CHECK(table.size() == 3);
  CHECK(*table.get("sum") == 3);
  CHECK(*table.get("loop") == 2);
  CHECK(table.contains("total"));
  CHECK(!table.contains("while"));

  table.put("sum", 9);
  CHECK(table.size() == 3);
  CHECK(*table.get("sum") == 9);

  CHECK(table.remove("total"));
  CHECK(!table.contains("total"));
  CHECK(table.size() == 2);
  CHECK(!table.remove("total"));

  HashTable<std::string, int> counts;
  for (int i = 0; i < 5000; ++i) {
    counts.put("word" + std::to_string(i), i);
  }
  CHECK(counts.size() == 5000);
  CHECK(counts.loadFactor() <= 0.75);
  CHECK(counts.bucketCount() >= 5000);

  bool allThere = true;
  for (int i = 0; i < 5000; ++i) {
    int* value = counts.get("word" + std::to_string(i));
    if (value == nullptr || *value != i) {
      allThere = false;
      break;
    }
  }
  CHECK(allThere);

  CHECK(counts.longestChain() < 50);

  HashTable<int, int> small(4);
  for (int i = 0; i < 100; ++i) small.put(i, i * 2);
  CHECK(small.size() == 100);
  CHECK(*small.get(37) == 74);

  LinearProbeTable probe(8);
  probe.add("sum");
  probe.add("sum");
  probe.add("sum");
  probe.add("total");
  CHECK(probe.count("sum") == 3);
  CHECK(probe.count("total") == 1);
  CHECK(probe.count("missing") == 0);
  CHECK(probe.size() == 2);

  LinearProbeTable big(4);
  for (int i = 0; i < 300; ++i) big.add("w" + std::to_string(i % 100));
  CHECK(big.size() == 100);
  CHECK(big.count("w7") == 3);
  CHECK(big.loadFactor() <= 0.75);
  CHECK(big.averageProbes() >= 1.0);

  return testSummary("test_hash_table");
}
