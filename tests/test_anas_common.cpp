// test_anas_common.cpp -- REAL tests for the Week 0 homework pieces.
// Owner: Anas.  Covers src/common/: dyn_array, fnv, mulmod, pos_to_line, plus
// the frozen contract types. These are not stubs -- this is the file that has
// to be green on Alyan's machine before he writes a line of code.
//
// The STL is used here on purpose, as the oracle: engine code may not use
// std::vector or std::unordered_map, tests must compare against them.
#include <cstdint>
#include <string>
#include <vector>

#include "common/dyn_array.hpp"
#include "common/fnv.hpp"
#include "common/mulmod.hpp"
#include "common/pos_to_line.hpp"
#include "contract/contract.hpp"
#include "harness.hpp"

namespace {

// Deterministic xorshift64*. The project claims "runs twice, identical output",
// so no test may depend on a randomly seeded generator.
class Rng {
 public:
  explicit Rng(std::uint64_t seed) : s_(seed == 0 ? 0x9E3779B97F4A7C15ULL : seed) {}
  std::uint64_t next() {
    s_ ^= s_ >> 12;
    s_ ^= s_ << 25;
    s_ ^= s_ >> 27;
    return s_ * 2685821657736338717ULL;
  }

 private:
  std::uint64_t s_;
};

}  // namespace

// ---------------------------------------------------------------------------
// DynArray
// ---------------------------------------------------------------------------

PLAG_TEST(dyn_array_matches_vector_oracle_over_one_million_pushes) {
  constexpr std::size_t kN = 1000000;
  plag::DynArray<std::uint64_t> mine;
  std::vector<std::uint64_t> oracle;
  oracle.reserve(kN);

  Rng rng(12345);
  for (std::size_t i = 0; i < kN; ++i) {
    const std::uint64_t v = rng.next();
    mine.push_back(v);
    oracle.push_back(v);
  }

  CHECK_EQ(mine.size(), oracle.size());
  CHECK(!mine.empty());
  for (std::size_t i = 0; i < kN; ++i) CHECK_EQ(mine[i], oracle[i]);
  CHECK_EQ(mine.back(), oracle.back());
}

PLAG_TEST(dyn_array_growth_is_geometric) {
  plag::DynArray<int> a;
  CHECK_EQ(a.size(), std::size_t{0});
  CHECK(a.empty());

  a.push_back(1);
  const std::size_t cap1 = a.capacity();
  CHECK(cap1 >= 1);

  // Doubling means after 1M pushes the capacity is under 2M, never 1M+1M.
  for (int i = 0; i < 1000000; ++i) a.push_back(i);
  CHECK(a.capacity() < 2 * a.size());
  CHECK(a.capacity() >= a.size());
}

PLAG_TEST(dyn_array_move_transfers_ownership) {
  plag::DynArray<int> a;
  for (int i = 0; i < 100; ++i) a.push_back(i);
  const std::size_t n = a.size();

  plag::DynArray<int> b(std::move(a));
  CHECK_EQ(b.size(), n);
  CHECK_EQ(a.size(), std::size_t{0});   // moved-from is empty, not dangling
  CHECK_EQ(b[42], 42);

  plag::DynArray<int> c;
  c = std::move(b);
  CHECK_EQ(c.size(), n);
  CHECK_EQ(b.size(), std::size_t{0});
  CHECK_EQ(c[99], 99);
}

PLAG_TEST(dyn_array_reserve_and_pop_and_clear) {
  plag::DynArray<int> a;
  a.reserve(1000);
  CHECK(a.capacity() >= 1000);
  for (int i = 0; i < 10; ++i) a.push_back(i * 2);
  CHECK_EQ(a.size(), std::size_t{10});
  CHECK_EQ(a.back(), 18);
  a.pop_back();
  CHECK_EQ(a.size(), std::size_t{9});
  CHECK_EQ(a.back(), 16);
  a.clear();
  CHECK(a.empty());
  CHECK_EQ(a.size(), std::size_t{0});
}

PLAG_TEST(dyn_array_explicit_size_constructor) {
  plag::DynArray<std::uint32_t> a(16);
  CHECK(a.capacity() >= 16);
  CHECK_EQ(a.size(), std::size_t{0});   // capacity is not size
  a.push_back(7u);
  CHECK_EQ(a[0], 7u);
}

// ---------------------------------------------------------------------------
// mulmod / powmod / submod
// ---------------------------------------------------------------------------

PLAG_TEST(mulmod_matches_128bit_reference_on_random_pairs) {
  using u128 = unsigned __int128;
  const std::uint64_t p = plag::kMersenne61;
  Rng rng(987654321);

  for (int i = 0; i < 200000; ++i) {
    const std::uint64_t a = rng.next() % p;
    const std::uint64_t b = rng.next() % p;
    const std::uint64_t ref =
        static_cast<std::uint64_t>((static_cast<u128>(a) * b) % p);
    CHECK_EQ(plag::mulmod(a, b, p), ref);
  }
}

PLAG_TEST(mulmod_never_overflows_near_the_modulus) {
  const std::uint64_t p = plag::kMersenne61;
  // (p-1)*(p-1) is the worst case: 2^122, far beyond 64 bits.
  const std::uint64_t got = plag::mulmod(p - 1, p - 1, p);
  using u128 = unsigned __int128;
  const std::uint64_t ref = static_cast<std::uint64_t>(
      (static_cast<u128>(p - 1) * (p - 1)) % p);
  CHECK_EQ(got, ref);
  CHECK_EQ(got, 1u);   // (p-1)^2 == 1 mod p
  CHECK(got < p);
}

PLAG_TEST(powmod_matches_naive_loop) {
  const std::uint64_t p = plag::kMersenne61;
  Rng rng(4242);
  for (int i = 0; i < 200; ++i) {
    const std::uint64_t b = rng.next() % p;
    for (std::uint64_t e = 0; e < 40; ++e) {
      std::uint64_t naive = 1 % p;
      for (std::uint64_t j = 0; j < e; ++j) naive = plag::mulmod(naive, b, p);
      CHECK_EQ(plag::powmod(b, e, p), naive);
    }
  }
}

PLAG_TEST(powmod_caches_base_pow_k_minus_one_for_the_roller) {
  // The rolling hash needs base^(k-1) mod p; pin the contract here.
  const std::uint64_t p = plag::kMersenne61;
  const std::uint64_t b = 1000003;
  CHECK_EQ(plag::powmod(b, 0, p), 1u);
  CHECK_EQ(plag::powmod(b, 1, p), b);
  CHECK_EQ(plag::powmod(b, 14, p), plag::mulmod(plag::powmod(b, 7, p),
                                                plag::powmod(b, 7, p), p));
}

PLAG_TEST(submod_is_never_negative) {
  const std::uint64_t p = plag::kMersenne61;
  // 3 - 5 mod p must be p-2, not a wrapped-around huge number.
  CHECK_EQ(plag::submod(3, 5, p), p - 2);
  CHECK_EQ(plag::submod(5, 3, p), 2u);
  CHECK_EQ(plag::submod(0, 0, p), 0u);

  Rng rng(31337);
  for (int i = 0; i < 100000; ++i) {
    const std::uint64_t a = rng.next() % p;
    const std::uint64_t b = rng.next() % p;
    const std::uint64_t r = plag::submod(a, b, p);
    CHECK(r < p);
    // (a - b) + b == a  (mod p)
    CHECK_EQ((r + b) % p, a % p);
  }
}

// ---------------------------------------------------------------------------
// FNV-1a
// ---------------------------------------------------------------------------

PLAG_TEST(fnv1a_matches_known_test_vectors) {
  CHECK_EQ(plag::fnv1a(""), 0xcbf29ce484222325ULL);   // offset basis
  CHECK_EQ(plag::fnv1a("a"), 0xaf63dc4c8601ec8cULL);
  CHECK_EQ(plag::fnv1a("foobar"), 0x85944171f73967e8ULL);
  CHECK_EQ(plag::fnv1a("hello world"), 0x779a65e7023cd2e7ULL);
  CHECK_EQ(plag::fnv1a("int"), 0x2b9fff192bd4c83eULL);
}

PLAG_TEST(fnv1a_explicit_length_agrees_with_nul_terminated_form) {
  const char* s = "token";
  CHECK_EQ(plag::fnv1a(s, 5), plag::fnv1a(s));
  // A shorter slice must hash differently -- the tokenizer relies on len.
  CHECK(plag::fnv1a(s, 4) != plag::fnv1a(s, 5));
}

PLAG_TEST(fnv_mix_spreads_low_bits) {
  // Keys that share their low bits must not land in the same bucket.
  const std::size_t buckets = 64;
  std::vector<int> hits(buckets, 0);
  for (std::uint64_t i = 0; i < 4096; ++i) {
    const std::uint64_t key = i << 32;          // all low 32 bits identical
    const std::uint64_t h = plag::fnv_mix(key);
    ++hits[static_cast<std::size_t>((h ^ (h >> 32)) % buckets)];
  }
  std::size_t used = 0;
  for (std::size_t i = 0; i < buckets; ++i) {
    if (hits[i] > 0) ++used;
  }
  // A hash with boring low bits would touch ~1 bucket out of 64.
  CHECK(used > buckets / 2);
}

// ---------------------------------------------------------------------------
// pos_to_line
// ---------------------------------------------------------------------------

PLAG_TEST(pos_to_line_reports_the_right_line_number) {
  const std::string src = "line one\nline two\nline three\nline four\n";
  plag::DynArray<std::uint32_t> starts;
  plag::build_line_starts(reinterpret_cast<const std::uint8_t*>(src.data()),
                          src.size(), starts);
  CHECK_EQ(starts.size(), std::size_t{5});   // 4 newlines -> 5 line starts

  // Offset of "line three" is 18; it is line 3.
  const std::size_t off = src.find("line three");
  CHECK_EQ(plag::pos_to_line(starts, static_cast<std::uint32_t>(off)), 3u);

  // The very first byte is line 1, and one past the end clamps to line 5.
  CHECK_EQ(plag::pos_to_line(starts, 0u), 1u);
  CHECK_EQ(plag::pos_to_line(starts, static_cast<std::uint32_t>(src.size())), 5u);
}

PLAG_TEST(pos_to_line_handles_a_file_with_no_newlines) {
  const std::string src = "one single line, no newline at the end";
  plag::DynArray<std::uint32_t> starts;
  plag::build_line_starts(reinterpret_cast<const std::uint8_t*>(src.data()),
                          src.size(), starts);
  CHECK_EQ(starts.size(), std::size_t{1});
  CHECK_EQ(plag::pos_to_line(starts, 0u), 1u);
  CHECK_EQ(plag::pos_to_line(starts, 10u), 1u);
}

PLAG_TEST(pos_to_line_on_empty_input_is_line_one) {
  plag::DynArray<std::uint32_t> starts;
  plag::build_line_starts(nullptr, 0, starts);
  CHECK_EQ(starts.size(), std::size_t{1});
  CHECK_EQ(plag::pos_to_line(starts, 0u), 1u);
}

// ---------------------------------------------------------------------------
// Contract: the frozen shared types must stay trivially usable by both halves
// ---------------------------------------------------------------------------

PLAG_TEST(contract_types_are_default_constructible_and_agree_on_sizes) {
  plag::Occurrence o;
  CHECK_EQ(o.docId, 0u);
  CHECK_EQ(o.pos, 0u);

  plag::Span s;
  s.startA = 10;
  s.endA = 25;
  CHECK_EQ(s.tokens(), 15u);

  plag::IndexStats st;
  CHECK_EQ(st.files, std::size_t{0});
  CHECK_NEAR(st.alpha, 0.0, 1e-12);
  CHECK(!st.malformedSeen);

  CHECK(plag::ErrorCode::Ok != plag::ErrorCode::NotFound);
  CHECK(std::string(plag::to_string(plag::ErrorCode::EmptyFile)) == "EmptyFile");
  CHECK(std::string(plag::to_string(plag::ErrorCode::BadArg)) == "BadArg");
}
