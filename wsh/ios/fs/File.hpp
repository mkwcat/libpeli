// wsh/ios/fs/File.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../../common/Types.h"
#include "../Resource.hpp"
#include "Interface.hpp"
#include "Types.hpp"

namespace wsh::ios::fs {

class File : Resource<Interface>, public Interface {
public:
  File(const char *path, OpenMode mode)
      : Resource(path, static_cast<u32>(mode)) {}

  ~File() noexcept = default;

  using Resource::GetHandle;
  using Resource::IsValid;
  using Resource::Read;
  using Resource::Resource;
  using Resource::Seek;
  using Resource::Write;

  IOSError GetFileStats(FileStats &stats) noexcept {
    return GetFileStats::Request(GetHandle())
        .Sync()
        .CopyOutput(stats)
        .GetError();
  }

  IOSError GetFileStats(s32 &size, s32 &pos) noexcept {
    auto request = GetFileStats::Request(GetHandle()).Sync();
    FileStats stats = request.GetOutput();
    size = stats.size;
    pos = stats.pos;
    return request.GetError();
  }

  IOSError GetSize(s32 &size) noexcept {
    s32 pos;
    return GetFileStats(size, pos);
  }

  IOSError GetPos(s32 &pos) noexcept {
    s32 size;
    return GetFileStats(size, pos);
  }
};

} // namespace wsh::ios::fs