// hash_table.cpp -- STUB (Anas, W3 / Step 5).
//
// HashTable<K,V> is a template, so its definition lives in hash_table.hpp --
// a template body must be visible to whoever instantiates it. This
// translation unit exists for two reasons: the area's CMakeLists glob always
// has a source, and the static checks below fail the build the moment the
// template stops satisfying the API both halves of the project rely on.
#include "index/hash_table.hpp"

#include <cstdint>
#include <type_traits>
#include <utility>

#include "contract/contract.hpp"

namespace plag {
namespace {

using IndexTable = HashTable<std::uint64_t, OccurrenceList>;
using CounterTable = HashTable<std::uint64_t, int>;

// The API contract, checked at compile time.
static_assert(std::is_default_constructible<IndexTable>::value,
              "HashTable must be default constructible");
static_assert(!std::is_copy_constructible<IndexTable>::value,
              "HashTable owns nodes: copy must stay deleted");
static_assert(!std::is_copy_assignable<CounterTable>::value,
              "HashTable owns nodes: copy assignment must stay deleted");
static_assert(std::is_same<decltype(std::declval<IndexTable&>().find(0ULL)),
                           OccurrenceList*>::value,
              "find() must return a pointer to the stored value");

// Proves Alyan's reuse compiles: (docA,docB) -> int, not just fingerprints.
static_assert(std::is_same<decltype(std::declval<CounterTable&>().insert(0ULL, 0)),
                           int&>::value,
              "insert() must return a reference to the stored value");

}  // namespace
}  // namespace plag
