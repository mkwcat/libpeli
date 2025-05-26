// wsh/util/VIConsole.cpp - Visual debug console shared between PPC and IOS
//   Written by mkwcat
//   Written by stebler
//
// Based on MKW-SP:
// https://github.com/stblr/mkw-sp/blob/main/common/Console.cc
// https://github.com/stblr/mkw-sp/blob/main/common/VI.cc
//
// Copyright (c) 2021-2023 Pablo Stebler
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.h"
#include "../util/Address.hpp"

namespace wsh::util {

class VIConsole {
public:
  // All values must be 32-bit sized to be compatible with uncached memory.
  struct alignas(32) Share {
    u32 *xfb = nullptr;
    u32 xfbWidth;
    u32 xfbHeight;
    u32 lock;
    s32 ppcRow;
    s32 iosRow;
    s32 xfbInit;
    u32 option;
  };

  static_assert(IsAligned(32, sizeof(Share)));

private:
  Share *m_shareBlock;

  volatile Share *m_share;

  u8 m_col;
  bool m_newline;

  static constexpr u32 PpcLock = 0x1;
  static constexpr u32 IosLock = 0x2;

#ifdef WSH_HOST_PPC
  static constexpr u32 MyLock = PpcLock;
  static constexpr u32 OtherLock = IosLock;
#else
  static constexpr u32 MyLock = IosLock;
  static constexpr u32 OtherLock = PpcLock;
#endif

public:
#ifdef WSH_HOST_PPC

  /**
   * Create and allocate memory for the debug console.
   */
  VIConsole(bool sideways = false) noexcept;

  /**
   * Configure VI for the debug console.
   * @param clear Clear the XFB.
   */
  void ConfigureVideo(bool clear = false) noexcept;

#endif

  /**
   * Recreate the debug console using an existing shared configuration.
   */
  VIConsole(Share *share) noexcept;

  /**
   * Get the width of the console framebuffer.
   */
  u16 GetXfbWidth() const noexcept;

  /**
   * Get the height of the console framebuffer.
   */
  u16 GetXfbHeight() const noexcept;

  /**
   * Get column count.
   */
  u8 NumCols() const noexcept;

  /**
   * Get row count.
   */
  u8 NumRows() const noexcept;

  /**
   * Read from the specified pixel on the framebuffer.
   */
  u8 ReadGrayscaleFromXfb(u16 x, u16 y) const noexcept;

  /**
   * Write to the specified pixel on the framebuffer.
   */
  void WriteGrayscaleToXfb(u16 x, u16 y, u8 intensity) noexcept;

  /**
   * Move the framebuffer up by the specified height.
   */
  void MoveUp(u16 height) noexcept;

  /**
   * Flush the XFB to main memory after writing to it.
   */
  void FlushXfb() noexcept;

  /**
   * Print a string to the debug console.
   */
  void Print(const char *s) noexcept;

  /**
   * Print a string to the debug console.
   */
  void Print(const char *s, size_t len) noexcept;

private:
  /**
   * Print a character. Assumes the resource is already locked.
   */
  void printChar(char c) noexcept;

  /**
   * Flush or invalidate the share block.
   *
   * @param write Flush the block instead of invalidate.
   */
  void syncShare(bool write) noexcept;

  /**
   * Lock console from other instance.
   */
  void lock() noexcept;

  /**
   * Unlock console for other instance.
   */
  void unlock() noexcept;

  /**
   * Get the current row for this instance.
   */
  s32 &refMyRow() noexcept;

  /**
   * Get the current row for the other instance.
   */
  s32 &refOtherRow() noexcept;

  /**
   * Increment the row for this instance.
   */
  s32 incrementRow() noexcept;

  /**
   * Decrement the row for both instances.
   */
  s32 decrementRow() noexcept;
};

} // namespace wsh::util