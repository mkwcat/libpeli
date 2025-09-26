// peli/ios/fs/Nand.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../Error.hpp"
#include "../Resource.hpp"
#include "../low/Ipc.hpp"
#include "Interface.hpp"
#include <stdlib.h>

namespace peli::ios::fs {

class Nand : Resource<Interface>, public Interface {
public:
  Nand() : Resource("/dev/fs", 0) {}

  constexpr s32 GetHandle() const noexcept { return Resource::GetHandle(); }
  constexpr bool IsValid() const noexcept { return Resource::IsValid(); }

  IOSError GetStats(Stats &stats) const noexcept {
    return GetStats::Request(GetHandle()) //
        .Sync()
        .CopyOutput(stats)
        .GetError();
  }

  IOSError CreateDir(Attr attr) const noexcept {
    return CreateDir::Request(GetHandle(), attr) //
        .Sync()
        .GetError();
  }

  IOSError ReadDirCount(const char *path, u32 &count) const noexcept {
    return ReadDirCount::Request(GetHandle(), path) //
        .Sync()
        .CopyOutput<0>(count)
        .GetError();
  }

  IOSError ReadDir(const char *path, u32 max_entries, NodeName *entries,
                   u32 &total_count) const noexcept {
    return ReadDir::Request(
               GetHandle(), path, max_entries,
               low::IOVector{entries, sizeof(NodeName) * max_entries}) //
        .Sync()
        .CopyOutput<1>(total_count)
        .GetError();
  }

  IOSError SetAttr(Attr attr) const noexcept {
    return SetAttr::Request(GetHandle(), attr) //
        .Sync()
        .GetError();
  }

  IOSError GetAttr(const char *path, Attr &attr) const noexcept {
    return GetAttr::Request(GetHandle(), path) //
        .Sync()
        .CopyOutput<0>(attr)
        .GetError();
  }

  IOSError Delete(const char *path) const noexcept {
    return Delete::Request(GetHandle(), path) //
        .Sync()
        .GetError();
  }

  IOSError Rename(const char *from, const char *to) const noexcept {
    RenamePaths paths{
        {},
        {},
    };
    ::strncpy(paths.from, from, PathSize);
    ::strncpy(paths.to, to, PathSize);
    return Rename::Request(GetHandle(), paths) //
        .Sync()
        .GetError();
  }

  IOSError CreateFile(Attr attr) const noexcept {
    return CreateFile::Request(GetHandle(), attr) //
        .Sync()
        .GetError();
  }

  IOSError GetUsage(const char *path, u32 &used_clusters,
                    u32 &used_inodes) const noexcept {
    return GetUsage::Request(GetHandle(), path) //
        .Sync()
        .CopyOutput<0>(used_clusters)
        .CopyOutput<1>(used_inodes)
        .GetError();
  }

  IOSError Shutdown() const noexcept {
    return Shutdown::Request(GetHandle()) //
        .Sync()
        .GetError();
  }

  template <size_t TPathCount>
  IOSError CreateMultipleFiles(const char *paths[TPathCount],
                               const u32 sizes[TPathCount]) const noexcept {
    alignas(low::Alignment) low::IOVector vec[TPathCount + 1];
    alignas(low::Alignment) char path_buffer[TPathCount][PathSize];
    alignas(low::Alignment) u32 size_buffer[TPathCount];
    for (size_t i = 0; i < TPathCount; ++i) {
      ::strncpy(path_buffer[i], paths[i], PathSize);
      vec[i].data = path_buffer[i];
      vec[i].size = PathSize;
      size_buffer[i] = sizes[i];
    }
    vec[TPathCount].data = size_buffer;
    vec[TPathCount].size = sizeof(u32) * TPathCount;
    return static_cast<IOSError>(low::IOS_Ioctlv(
        GetHandle(), static_cast<u32>(Ioctl::CREATE_MULTIPLE_FILES), TPathCount,
        0, vec));
  }

  IOSError CreateMultipleFiles(const char *const *paths, const u32 *sizes,
                               u32 count) const noexcept {
    u32 vec_size =
        util::AlignUp(low::Alignment, sizeof(low::IOVector) * (count + 1));
    u32 path_buffer_size = util::AlignUp(low::Alignment, PathSize * count);
    u32 size_buffer_size = util::AlignUp(low::Alignment, sizeof(u32) * count);
    void *buffer = ::aligned_alloc(low::Alignment, vec_size + path_buffer_size +
                                                       size_buffer_size);

    low::IOVector *vec = static_cast<low::IOVector *>(buffer);
    char *path_buffer = static_cast<char *>(buffer) + vec_size;
    u32 *size_buffer = reinterpret_cast<u32 *>(path_buffer + path_buffer_size);
    for (u32 i = 0; i < count; ++i) {
      ::strncpy(&path_buffer[i * PathSize], paths[i], PathSize);
      vec[i].data = &path_buffer[i * PathSize];
      vec[i].size = PathSize;
      size_buffer[i] = sizes[i];
    }
    vec[count].data = size_buffer;
    vec[count].size = sizeof(u32) * count;
    IOSError error = static_cast<IOSError>(low::IOS_Ioctlv(
        GetHandle(), static_cast<u32>(Ioctl::CREATE_MULTIPLE_FILES), count, 0,
        vec));
    ::free(buffer);
    return error;
  }
};

} // namespace peli::ios::fs