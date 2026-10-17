// Implementation of the single-pass tokenizer
#include "index/tokenizer.hpp"

#include "common/fnv.hpp"
#include "common/pos_to_line.hpp"

namespace plag {
namespace {

bool is_space(std::uint8_t c) {
  return c == ' ' || c == '\t' || c == '\n' ||
         c == '\r' || c == '\v' || c == '\f';
}

bool is_alpha(std::uint8_t c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool is_digit(std::uint8_t c) {
  return c >= '0' && c <= '9';
}

bool is_alnum(std::uint8_t c) {
  return is_alpha(c) || is_digit(c);
}

bool is_two_char_op(std::uint8_t a, std::uint8_t b) {
  if (b == '=') {
    return a == '=' || a == '!' || a == '<' || a == '>' ||
           a == '+' || a == '-' || a == '*' || a == '/';
  }
  if (a == '&' && b == '&') return true;
  if (a == '|' && b == '|') return true;
  if (a == '+' && b == '+') return true;
  if (a == '-' && b == '-') return true;
  if (a == '<' && b == '<') return true;
  if (a == '>' && b == '>') return true;
  if (a == '-' && b == '>') return true;
  if (a == ':' && b == ':') return true;
  return false;
}

std::uint64_t hash_lexeme(const std::uint8_t* data, std::size_t len,
                          bool lower_case) {
  std::uint64_t h = kFnvOffsetBasis;
  for (std::size_t i = 0; i < len; ++i) {
    std::uint8_t ch = data[i];
    if (lower_case && ch >= 'A' && ch <= 'Z') {
      ch = static_cast<std::uint8_t>(ch - 'A' + 'a');
    }
    h ^= static_cast<std::uint64_t>(ch);
    h *= kFnvPrime;
  }
  return h;
}

void add_token(TokenPool& pool, const std::uint8_t* data,
               const DynArray<std::uint32_t>& line_starts,
               std::size_t start, std::size_t length, bool lower_case) {
  std::uint32_t off32 = static_cast<std::uint32_t>(start);
  std::uint32_t len32 = static_cast<std::uint32_t>(length);
  pool.off.push_back(off32);
  pool.len.push_back(len32);
  pool.id.push_back(hash_lexeme(data + start, length, lower_case));
  pool.line.push_back(pos_to_line(line_starts, off32));
}

}  // namespace

TokenizeResult tokenize(const DynamicBuffer& src, TokenPool& pool,
                        bool lower_case_ids) {
  pool.off.clear();
  pool.len.clear();
  pool.id.clear();
  pool.line.clear();

  TokenizeResult res;
  const std::uint8_t* data = src.data();
  const std::size_t n = src.size();
  if (data == nullptr || n == 0) {
    return res;
  }

  DynArray<std::uint32_t> line_starts;
  build_line_starts(data, n, line_starts);

  std::size_t i = 0;
  while (i < n) {
    std::uint8_t c = data[i];

    // Skip whitespace
    if (is_space(c)) {
      ++i;
      continue;
    }

    // Handle single-line comments //
    if (c == '/' && i + 1 < n && data[i + 1] == '/') {
      i += 2;
      while (i < n && data[i] != '\n') {
        ++i;
      }
      continue;
    }

    // Handle block comments /* ... */
    if (c == '/' && i + 1 < n && data[i + 1] == '*') {
      i += 2;
      bool closed = false;
      while (i + 1 < n) {
        if (data[i] == '*' && data[i + 1] == '/') {
          i += 2;
          closed = true;
          break;
        }
        ++i;
      }
      if (!closed) {
        res.malformed = true;
        break;
      }
      continue;
    }

    // Handle string or character literals
    if (c == '"' || c == '\'') {
      std::uint8_t quote = c;
      std::size_t start = i;
      ++i;
      bool closed = false;
      while (i < n) {
        if (data[i] == '\\' && i + 1 < n) {
          i += 2;
          continue;
        }
        if (data[i] == quote) {
          ++i;
          closed = true;
          break;
        }
        if (data[i] == '\n') {
          break;
        }
        ++i;
      }
      if (!closed) {
        res.malformed = true;
      }
      add_token(pool, data, line_starts, start, i - start, lower_case_ids);
      continue;
    }

    // Handle identifiers and keywords
    if (is_alpha(c)) {
      std::size_t start = i;
      ++i;
      while (i < n && is_alnum(data[i])) {
        ++i;
      }
      add_token(pool, data, line_starts, start, i - start, lower_case_ids);
      continue;
    }

    // Handle numeric literals
    if (is_digit(c)) {
      std::size_t start = i;
      ++i;
      while (i < n && (is_alnum(data[i]) || data[i] == '.')) {
        ++i;
      }
      add_token(pool, data, line_starts, start, i - start, lower_case_ids);
      continue;
    }

    // Handle two-character and single-character operators/punctuation
    if (i + 1 < n && is_two_char_op(c, data[i + 1])) {
      add_token(pool, data, line_starts, i, 2, lower_case_ids);
      i += 2;
    } else {
      add_token(pool, data, line_starts, i, 1, lower_case_ids);
      ++i;
    }
  }

  res.tokens = pool.id.size();
  return res;
}

}  // namespace plag
