// peli/log/VideoConsole.cpp - Visual debug console shared between PPC and IOS
//   Written by mkwcat
//   Written by stebler
//
// Based on MKW-SP:
// https://github.com/stblr/mkw-sp/blob/main/cmn/Console.cc
// https://github.com/stblr/mkw-sp/blob/main/cmn/VI.cc
//
// Copyright (c) 2021-2023 Pablo Stebler
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../cmn/Types.hpp"
#include "../host/Config.h"
#include "../util/Address.hpp"

namespace peli::log {

class VideoConsole {
public:
  // All values must be 32-bit sized to be compatible with uncached memory.
  struct alignas(32) Share {
    u32 *xfb = nullptr;
    u32 xfb_width;
    u32 xfb_height;
    u32 lock;
    s32 ppc_row;
    s32 ios_row;
    s32 xfb_init;
    u32 option;
  };

  static_assert(util::IsAligned(32, sizeof(Share)));

private:
  Share *m_share_block;

  volatile Share *m_share;

  u8 m_col;
  bool m_newline;

  static constexpr u32 PpcLock = 0x1;
  static constexpr u32 IosLock = 0x2;

#if defined(PELI_HOST_PPC)
  static constexpr u32 MyLock = PpcLock;
  static constexpr u32 OtherLock = IosLock;
#else
  static constexpr u32 MyLock = IosLock;
  static constexpr u32 OtherLock = PpcLock;
#endif

public:
#if defined(PELI_HOST_PPC)

  /**
   * Create and allocate memory for the debug console.
   */
  VideoConsole(bool sideways = false) noexcept;

  /**
   * Configure VI for the debug console.
   * @param clear Clear the XFB.
   */
  void ConfigureVideo(bool clear = false) noexcept;

#endif

  /**
   * Recreate the debug console using an existing shared configuration.
   */
  VideoConsole(Share *share) noexcept;

  /**
   * Set the alignment width of the TAB character. Does not apply retroactively
   * to previously printed text. Must be between 1 and 16.
   */
  void SetTabWidth(u8 width) noexcept;

  /**
   * Get the current TAB alignment width.
   */
  u8 GetTabWidth() const noexcept;

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

} // namespace peli::log