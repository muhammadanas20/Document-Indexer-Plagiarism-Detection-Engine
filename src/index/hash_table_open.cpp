// hash_table_open.cpp -- STUB (Anas, W5 / Step 7a). The template definition
// lives in the header; this TU keeps the glob non-empty and pins the API.
#include "index/hash_table_open.hpp"

#include <cstdint>
#include <type_traits>
#include <utility>

#include "contract/contract.hpp"

namespace plag {
namespace {

using ProbeTable = OpenHashTable<std::uint64_t, OccurrenceList>;

static_assert(std::is_default_constructible<ProbeTable>::value,
              "OpenHashTable must be default constructible");
static_assert(!std::is_copy_constructible<ProbeTable>::value,
              "OpenHashTable owns its arrays: copy must stay deleted");

}  // namespace
}  // namespace plag
