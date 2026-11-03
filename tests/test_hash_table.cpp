// test_hash_table.cpp  --  separate chaining and linear probing
// Owner: Anas

#include <string>

#include "hash_table.hpp"
#include "probing.hpp"
#include "test_util.hpp"

using namespace dip;

int main() {
  // ---- hash functions -----------------------------------------------------
  CHECK(hashKey("sum") == hashKey("sum"));      // always the same number
  CHECK(hashKey("sum") != hashKey("total"));    // usually different
  CHECK(hashKey(7ULL) == 7ULL);                 // numbers hash to themselves

  // ---- chaining hash table: insert, find, update, remove ------------------
  HashTable<std::string, int> table;
  CHECK(table.size() == 0);
  CHECK(table.get("sum") == nullptr);           // empty table

  table.put("sum", 3);
  table.put("total", 1);
  table.put("loop", 2);
  CHECK(table.size() == 3);
  CHECK(*table.get("sum") == 3);
  CHECK(*table.get("loop") == 2);
  CHECK(table.contains("total"));
  CHECK(!table.contains("while"));

  // Putting the same key again must UPDATE the value, not add a second entry.
  table.put("sum", 9);
  CHECK(table.size() == 3);
  CHECK(*table.get("sum") == 9);

  CHECK(table.remove("total"));
  CHECK(!table.contains("total"));
  CHECK(table.size() == 2);
  CHECK(!table.remove("total"));                // already gone

  // ---- the table must grow (rehash) and stay correct ----------------------
  HashTable<std::string, int> counts;
  for (int i = 0; i < 5000; ++i) {
    counts.put("word" + std::to_string(i), i);
  }
  CHECK(counts.size() == 5000);
  CHECK(counts.loadFactor() <= 0.75);           // rehashing kept it below 0.75
  CHECK(counts.bucketCount() >= 5000);          // the array grew a few times

  bool allThere = true;
  for (int i = 0; i < 5000; ++i) {
    int* value = counts.get("word" + std::to_string(i));
    if (value == nullptr || *value != i) {
      allThere = false;
      break;
    }
  }
  CHECK(allThere);

  // A good hash function spreads the keys: the longest chain stays short.
  CHECK(counts.longestChain() < 50);

  // ---- many keys in ONE bucket (worst case) still works -------------------
  HashTable<int, int> small(4);                 // only 4 buckets on purpose
  for (int i = 0; i < 100; ++i) small.put(i, i * 2);
  CHECK(small.size() == 100);
  CHECK(*small.get(37) == 74);

  // ---- linear probing table ----------------------------------------------
  LinearProbeTable probe(8);
  probe.add("sum");
  probe.add("sum");
  probe.add("sum");
  probe.add("total");
  CHECK(probe.count("sum") == 3);               // counted three times
  CHECK(probe.count("total") == 1);
  CHECK(probe.count("missing") == 0);
  CHECK(probe.size() == 2);

  // Filling it up forces a grow + re-insert of every old word.
  LinearProbeTable big(4);
  for (int i = 0; i < 300; ++i) big.add("w" + std::to_string(i % 100));
  CHECK(big.size() == 100);                     // 100 different words
  CHECK(big.count("w7") == 3);                  // each one seen 3 times
  CHECK(big.loadFactor() <= 0.75);
  CHECK(big.averageProbes() >= 1.0);            // at least one slot per insert

  return testutil::summary("test_hash_table");
}
