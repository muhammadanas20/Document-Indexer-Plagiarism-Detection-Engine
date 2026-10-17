// Unit tests for src/common utilities (DynArray, FNV-1a, mulmod, pos_to_line)
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "common/dyn_array.hpp"
#include "common/fnv.hpp"
#include "common/mulmod.hpp"
#include "common/pos_to_line.hpp"
#include "contract/contract.hpp"
#include "harness.hpp"

PLAG_TEST(dyn_array_push_and_index) {
  plag::DynArray<int> arr;
  std::vector<int> vec;

  for (int i = 0; i < 10000; ++i) {
    arr.push_back(i * 3);
    vec.push_back(i * 3);
  }

  CHECK_EQ(arr.size(), vec.size());
  for (std::size_t i = 0; i < vec.size(); ++i) {
    CHECK_EQ(arr[i], vec[i]);
  }
  CHECK_EQ(arr.back(), vec.back());
}

PLAG_TEST(dyn_array_copy_and_move) {
  plag::DynArray<int> a;
  for (int i = 0; i < 50; ++i) {
    a.push_back(i);
  }

  plag::DynArray<int> copy(a);
  CHECK_EQ(copy.size(), std::size_t{50});
  CHECK_EQ(copy[25], 25);

  plag::DynArray<int> moved(std::move(a));
  CHECK_EQ(moved.size(), std::size_t{50});
  CHECK_EQ(a.size(), std::size_t{0});
  CHECK_EQ(moved[49], 49);
}

PLAG_TEST(dyn_array_reserve_pop_clear) {
  plag::DynArray<int> a;
  a.reserve(100);
  CHECK(a.capacity() >= 100);
  for (int i = 0; i < 10; ++i) {
    a.push_back(i * 2);
  }
  CHECK_EQ(a.size(), std::size_t{10});
  a.pop_back();
  CHECK_EQ(a.size(), std::size_t{9});
  CHECK_EQ(a.back(), 16);
  a.clear();
  CHECK(a.empty());
}

PLAG_TEST(mulmod_and_powmod_basic) {
  const std::uint64_t p = plag::kMersenne61;
  CHECK_EQ(plag::mulmod(123456ULL, 789012ULL, p),
           (123456ULL * 789012ULL) % p);
  CHECK_EQ(plag::mulmod(p - 1, p - 1, p), std::uint64_t{1});
  CHECK_EQ(plag::powmod(2ULL, 10ULL, p), std::uint64_t{1024});
  CHECK_EQ(plag::submod(5ULL, 10ULL, p), p - 5ULL);
}

PLAG_TEST(fnv1a_known_values) {
  CHECK_EQ(plag::fnv1a(""), plag::kFnvOffsetBasis);
  CHECK_EQ(plag::fnv1a("hello"), plag::fnv1a("hello", 5));
  CHECK(plag::fnv1a("abc") != plag::fnv1a("abd"));
}

PLAG_TEST(pos_to_line_maps_offsets_correctly) {
  const char* text = "first\nsecond\nthird";
  plag::DynArray<std::uint32_t> starts;
  plag::build_line_starts(reinterpret_cast<const std::uint8_t*>(text),
                          std::strlen(text), starts);

  CHECK_EQ(starts.size(), std::size_t{3});
  CHECK_EQ(plag::pos_to_line(starts, 0), 1u);
  CHECK_EQ(plag::pos_to_line(starts, 4), 1u);
  CHECK_EQ(plag::pos_to_line(starts, 6), 2u);
  CHECK_EQ(plag::pos_to_line(starts, 13), 3u);
}

PLAG_TEST(error_code_strings) {
  CHECK_EQ(std::string(plag::to_string(plag::ErrorCode::Ok)), "Ok");
  CHECK_EQ(std::string(plag::to_string(plag::ErrorCode::NotFound)), "NotFound");
}
