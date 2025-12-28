// peli/ios/di/Types.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../../cmn/Types.hpp"

namespace peli::ios::di {

struct DiskId {
  /**
   * Game Code, e.g. "RSPE" for Wii Sports.
   */
  /* 0x00 */ char game_code[4];

  /**
   * Maker (Group) Code, typically in ASCII. For example, 0x3031 (01) for
   * first-party Nintendo-published games.
   */
  /* 0x04 */ u16 maker_code;

  /**
   * Disc Number, for multi-disc games.
   */
  /* 0x06 */ u8 disc_number;

  /**
   * Disc Version, for games with multiple revisions.
   */
  /* 0x07 */ u8 disc_version;

  /**
   * Disc Streaming Flag.
   */
  /* 0x08 */ u8 disc_stream_flag;

  /**
   * Disc Streaming Buffer Size.
   */
  /* 0x09 */ u8 disc_stream_size;

  /**
   * Padding.
   */
  /* 0x0A */ u8 pad[0x18 - 0x0A];

  enum class RvlMagic : u32 {
    Not = 0,
    Expected = 0x5D1C9EA3,
  };

  enum class DolMagic : u32 {
    Not = 0,
    Expected = 0xC2339F3D,
  };

  /**
   * Disc Magic Number to designate this as a Wii disc.
   */
  RvlMagic rvl_magic;

  /**
   * Disc Magic Number to designate this as a GameCube disc.
   */
  DolMagic dol_magic;
};

static_assert(sizeof(DiskId) == 0x20);

} // namespace peli::ios::di