// wsh/ios/fs/Types.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../../common/Types.h"

namespace wsh::ios::fs {

constexpr u32 PathSize = 64; // Includes null terminator
using Path = char[PathSize];

constexpr u32 NodeNameSize = 13; // Includes null terminator
using NodeName = char[NodeNameSize];

struct Stats {
  /* 0x00 */ u32 cluster_size;
  /* 0x04 */ u32 free_clusters;
  /* 0x08 */ u32 used_clusters;
  /* 0x0C */ u32 bad_clusters;
  /* 0x10 */ u32 reserved_clusters;
  /* 0x14 */ u32 free_inodes;
  /* 0x18 */ u32 used_inodes;
};

static_assert(sizeof(Stats) == 0x1C);

struct Usage {
  /* 0x00 */ u32 used_clusters;
  /* 0x04 */ u32 used_inodes;
};

static_assert(sizeof(Usage) == 0x8);

struct FileStats {
  /* 0x00 */ u32 size;
  /* 0x04 */ u32 pos;
};

static_assert(sizeof(FileStats) == 0x8);

struct _WSH_PACKED Attr {
  /* 0x00 */ u32 uid;
  /* 0x04 */ u16 gid;
  /* 0x06 */ Path path;
  /* 0x46 */ u8 perm_owner;
  /* 0x47 */ u8 perm_group;
  /* 0x48 */ u8 perm_other;
  /* 0x49 */ u8 attributes;
};

static_assert(sizeof(Attr) == 0x4A);

struct RenamePaths {
  /* 0x00 */ Path from;
  /* 0x40 */ Path to;
};

static_assert(sizeof(RenamePaths) == 0x80);

} // namespace wsh::ios::fs