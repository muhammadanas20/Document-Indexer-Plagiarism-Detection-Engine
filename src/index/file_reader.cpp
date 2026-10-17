// Implementation of file_reader
#include "index/file_reader.hpp"

#include <filesystem>
#include <fstream>
#include <system_error>

namespace plag {

ErrorCode read_file(const char* path, DynamicBuffer& out) {
  out.clear();
  if (path == nullptr || path[0] == '\0') {
    return ErrorCode::BadArg;
  }

  std::error_code ec;
  std::filesystem::file_status st = std::filesystem::status(path, ec);
  if (ec || !std::filesystem::exists(st)) {
    return ErrorCode::NotFound;
  }
  if (std::filesystem::is_directory(st) || !std::filesystem::is_regular_file(st)) {
    return ErrorCode::NotReadable;
  }

  std::ifstream in(path, std::ios::binary);
  if (!in.is_open()) {
    return ErrorCode::NotReadable;
  }

  std::uintmax_t fsize = std::filesystem::file_size(path, ec);
  if (!ec && fsize > 0) {
    out.reserve_at_least(static_cast<std::size_t>(fsize));
  }

  std::uint8_t chunk[4096];
  while (in.good()) {
    in.read(reinterpret_cast<char*>(chunk), sizeof(chunk));
    std::streamsize got = in.gcount();
    if (got > 0) {
      out.append(chunk, static_cast<std::size_t>(got));
    }
  }

  if (in.bad()) {
    out.clear();
    return ErrorCode::NotReadable;
  }

  out.trim_trailing_cr();
  if (out.empty()) {
    return ErrorCode::EmptyFile;
  }

  return ErrorCode::Ok;
}

}  // namespace plag
