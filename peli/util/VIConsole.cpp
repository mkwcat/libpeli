// peli/util/VIConsole.cpp - Visual debug console shared between PPC and IOS
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

#include "VIConsole.hpp"
#include "../util/Address.hpp"
#include "../util/CpuCache.hpp"
#include <cstdlib>

#if defined(PELI_HOST_PPC)
#include "../hw/Video.hpp"
#endif

extern "C" const unsigned char VIConsoleFont[128][16];

namespace peli::util {

constexpr u8 BgIntensity = 16;
constexpr u8 FgIntensity = 235;

constexpr u8 GlyphWidth = 8;
constexpr u8 GlyphHeight = 16;

enum Option {
  // Bit shift
  TabWidth = 1,

  // Bit flag
  Sideways = 1 << 0,
};

#if defined(PELI_HOST_PPC)

/**
 * Create and configure VI for the debug console.
 */
VIConsole::VIConsole(bool sideways) noexcept {
  m_share_block = static_cast<Share *>(std::aligned_alloc(32, sizeof(Share)));
  *m_share_block = {};

  CpuCache::DcFlush(m_share_block, sizeof(Share));

  m_share = util::Uncached(m_share_block);

  ConfigureVideo(true);

  m_share->lock = 0;
  m_share->ppc_row = -1;
  m_share->ios_row = -1;
  m_share->xfb_init = true;
  m_share->option = (3 << Option::TabWidth) | (sideways ? Option::Sideways : 0);
}

/**
 * Configure VI for the debug console.
 * @param clear Clear the XFB.
 */
void VIConsole::ConfigureVideo(bool clear) noexcept {
  hw::Video video;

  m_share->xfb_width = video.GetXfbWidth();
  m_share->xfb_height = video.GetXfbHeight();
  if (m_share->xfb == nullptr) {
    m_share->xfb = reinterpret_cast<u32 *>(video.AllocateXfb());
  }

  if (clear) {
    for (u16 y = 0; y < m_share->xfb_height; y++) {
      for (u16 x = 0; x < m_share->xfb_width; x++) {
        WriteGrayscaleToXfb(x, y, 16);
      }
    }
    FlushXfb();
  }

  video.SetNextFramebuffer(m_share->xfb);
  video.Flush();

  m_col = 0;
}

#endif

/**
 * Recreate the debug console using an existing shared configuration.
 */
VIConsole::VIConsole(Share *share) noexcept {
#if defined(PELI_HOST_PPC)
  m_share = util::Uncached(share);
#else
  m_share = share;
#endif

  CpuCache::DcInvalidate(const_cast<Share *>(share), sizeof(Share));

  Print("\n");
}

/**
 * Flush or invalidate the share block.
 *
 * @param write Flush the block instead of invalidate.
 */
void VIConsole::syncShare([[maybe_unused]] bool write) noexcept {
#if defined(PELI_HOST_IOS)
  if (write) {
    CpuCache::DcFlush(const_cast<Share *>(m_share), sizeof(Share));
  } else {
    CpuCache::DcInvalidate(const_cast<Share *>(m_share), sizeof(Share));
  }
#else
  ppc::Sync();
  ppc::ISync();
#endif
}

/**
 * Lock console from other instance.
 */
void VIConsole::lock() noexcept {
  for (u32 i = 0; i < (MyLock == IosLock ? 8 : 16);) {
    syncShare(false);

    u32 lock = m_share->lock;

    // Check if PPC has locked it
    if (lock & OtherLock) {
      i = 0;
      continue;
    }

    m_share->lock = lock | MyLock;
    syncShare(true);
    i++;
  }
}

/**
 * Unlock console for other instance.
 */
void VIConsole::unlock() noexcept {
  syncShare(false);
  m_share->lock = 0;
  syncShare(true);
}

/**
 * Get the current row for this instance.
 */
s32 &VIConsole::refMyRow() noexcept {
  return const_cast<s32 &>(MyLock == PpcLock ? m_share->ppc_row
                                             : m_share->ios_row);
}

/**
 * Get the current row for the other instance.
 */
s32 &VIConsole::refOtherRow() noexcept {
  return const_cast<s32 &>(MyLock == PpcLock ? m_share->ios_row
                                             : m_share->ppc_row);
}

/**
 * Increment the row for this instance.
 */
s32 VIConsole::incrementRow() noexcept {
  syncShare(false);

  refMyRow()++;
  if (refMyRow() <= refOtherRow()) {
    refMyRow() = refOtherRow() + 1;
  }

  syncShare(true);

  return refMyRow();
}

/**
 * Decrement the row for both instances.
 */
s32 VIConsole::decrementRow() noexcept {
  syncShare(false);

  m_share->ios_row -= 1;
  m_share->ppc_row -= 1;

  syncShare(true);

  return refMyRow();
}

/**
 * Set the alignment width of the TAB character. Does not apply retroactively
 * to previously printed text. Must be between 1 and 16.
 */
void VIConsole::SetTabWidth(u8 width) noexcept {
  syncShare(false);

  constexpr u32 mask = 0xF << Option::TabWidth;
  width = (width - 1) << Option::TabWidth;
  m_share->option = (m_share->option & ~mask) | (width & mask);

  syncShare(true);
}

/**
 * Get the current TAB alignment width.
 */
u8 VIConsole::GetTabWidth() const noexcept {
  return ((m_share->option >> Option::TabWidth) & 0xF) + 1;
}

/**
 * Get the width of the console framebuffer.
 */
u16 VIConsole::GetXfbWidth() const noexcept { return m_share->xfb_width; }

/**
 * Get the height of the console framebuffer.
 */
u16 VIConsole::GetXfbHeight() const noexcept { return m_share->xfb_height; }

/**
 * Get column count.
 */
u8 VIConsole::NumCols() const noexcept {
  if (!(m_share->option & Option::Sideways)) {
    return m_share->xfb_width / GlyphWidth - 1;
  } else {
    return m_share->xfb_height / GlyphWidth - 1;
  }
}

/**
 * Get row count.
 */
u8 VIConsole::NumRows() const noexcept {
  if (!(m_share->option & Option::Sideways)) {
    return m_share->xfb_height / GlyphHeight - 2;
  } else {
    return m_share->xfb_width / GlyphHeight - 2;
  }
}

/**
 * Read from the specified pixel on the framebuffer.
 */
u8 VIConsole::ReadGrayscaleFromXfb(u16 x, u16 y) const noexcept {
  if (x > m_share->xfb_width || y > m_share->xfb_height) {
    return 16;
  }

  u32 val = m_share->xfb[y * (m_share->xfb_width / 2) + x / 2];
  if (x & 1) {
    return val >> 8;
  } else {
    return val >> 24;
  }
}

/**
 * Write to the specified pixel on the framebuffer.
 */
void VIConsole::WriteGrayscaleToXfb(u16 x, u16 y, u8 intensity) noexcept {
  if (x > m_share->xfb_width || y > m_share->xfb_height) {
    return;
  }

  u32 *val = &m_share->xfb[y * (m_share->xfb_width / 2) + x / 2];
  u8 y0 = *val >> 24;
  u8 y1 = *val >> 8;
  if (x & 1) {
    y1 = intensity;
  } else {
    y0 = intensity;
  }
  *val = y0 << 24 | 127 << 16 | y1 << 8 | 127;
}

/**
 * Move the framebuffer up by the specified height.
 */
void VIConsole::MoveUp(u16 height) noexcept {
  if (!(m_share->option & Option::Sideways)) {
    u32 offset = height * (m_share->xfb_width / 2);

    u32 src = AlignDown(offset, 32);
    u32 dest = 0;
    u32 totalSize =
        AlignDown(m_share->xfb_height * (m_share->xfb_width / 2), 32);

    // Copy 8 words at a time
    while (src < totalSize) {
      m_share->xfb[dest++] = m_share->xfb[src++];
      m_share->xfb[dest++] = m_share->xfb[src++];
      m_share->xfb[dest++] = m_share->xfb[src++];
      m_share->xfb[dest++] = m_share->xfb[src++];
      m_share->xfb[dest++] = m_share->xfb[src++];
      m_share->xfb[dest++] = m_share->xfb[src++];
      m_share->xfb[dest++] = m_share->xfb[src++];
      m_share->xfb[dest++] = m_share->xfb[src++];
    }
  } else {
    // Move left instead of up
    u32 offset = height / 2;
    u32 line_count = m_share->xfb_width / 2 - offset;

    for (u32 y = 0; y < m_share->xfb_height; y++) {
      u32 src = y * (m_share->xfb_width / 2) + offset;
      u32 dest = y * (m_share->xfb_width / 2);

      for (u32 i = 0; i < line_count; i++) {
        m_share->xfb[dest++] = m_share->xfb[src++];
      }

      for (u32 i = 0; i < offset; i++) {
        WriteGrayscaleToXfb(m_share->xfb_width - offset + i, y, 16);
      }
    }
  }
}

/**
 * Flush the XFB to main memory after writing to it.
 */
void VIConsole::FlushXfb() noexcept {
  CpuCache::DcFlush(m_share->xfb, 320 * 574 * sizeof(u32));
}

/**
 * Print a character. Assumes the resource is already locked.
 */
void VIConsole::printChar(char c) noexcept {
  if (c == '\n') {
    if (m_newline) {
      incrementRow();
    }

    m_col = 0;
    m_newline = true;
    return;
  }

  if (c == '\r') {
    m_col = 0;
    return;
  }

  if (m_newline) {
    incrementRow();
    m_newline = false;
  }

  if (c == '\t') {
    m_col = AlignUp(GetTabWidth(), m_col + 1);
  }

  if (m_col >= NumCols()) {
    incrementRow();
    m_col = 0;
  }

  s32 row = refMyRow();

  if (row < 0) {
    return;
  }

  while (row >= NumRows()) {
    VIConsole::MoveUp(GlyphHeight);
    row = decrementRow();
  }

  const u8 *glyph = VIConsoleFont[' '];
  if (u32(c) < 128) {
    glyph = VIConsoleFont[u32(c)];
  }

  u16 y0 = row * GlyphHeight + GlyphHeight / 2;
  for (u16 y = 0; y < GlyphHeight; y++) {
    u16 x0 = m_col * GlyphWidth + GlyphWidth / 2;
    for (u16 x = 0; x < GlyphWidth; x++) {
      u8 intensity = glyph[(y * GlyphWidth + x) / 8] & (1 << (8 - (x % 8)))
                         ? FgIntensity
                         : BgIntensity;
      if (m_share->option & Option::Sideways) {
        VIConsole::WriteGrayscaleToXfb(y0 + y, m_share->xfb_height - (x0 + x),
                                       intensity);
      } else {
        VIConsole::WriteGrayscaleToXfb(x0 + x, y0 + y, intensity);
      }
    }
  }

  m_col++;
}

/**
 * Print a string to the visual console.
 */
void VIConsole::Print(const char *s) noexcept {
  lock();
  for (; *s; s++) {
    printChar(*s);
  }
  FlushXfb();
  unlock();
}

/**
 * Print a string to the visual console.
 */
void VIConsole::Print(const char *s, size_t len) noexcept {
  lock();
  for (u32 i = 0; i < len; i++) {
    printChar(s[i]);
  }
  FlushXfb();
  unlock();
}

} // namespace peli::util