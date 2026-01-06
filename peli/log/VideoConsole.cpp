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

#include "VideoConsole.hpp"
#include "../host/Host.hpp"
#include "../util/Address.hpp"
#include "../util/CpuCache.hpp"

#if defined(PELI_HOST_PPC)
#include "../vi/Video.hpp"
#endif

extern "C" constinit const unsigned char VIConsoleFont[128][16];

namespace peli::log {

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
VideoConsole::VideoConsole(bool sideways) noexcept {
  m_share_block = static_cast<Share *>(host::Alloc(32, sizeof(Share)));
  *m_share_block = {};

  util::CpuCache::DcFlush(m_share_block, sizeof(Share));

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
void VideoConsole::ConfigureVideo(bool clear) noexcept {
  vi::Video video;

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
VideoConsole::VideoConsole(Share *share) noexcept {
#if defined(PELI_HOST_PPC)
  m_share = util::Uncached(share);
#else
  m_share = share;
#endif

  util::CpuCache::DcInvalidate(const_cast<Share *>(share), sizeof(Share));

  Print("\n");
}

/**
 * Flush or invalidate the share block.
 *
 * @param write Flush the block instead of invalidate.
 */
void VideoConsole::syncShare([[maybe_unused]] bool write) noexcept {
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
void VideoConsole::lock() noexcept {
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
void VideoConsole::unlock() noexcept {
  syncShare(false);
  m_share->lock = 0;
  syncShare(true);
}

/**
 * Get the current row for this instance.
 */
s32 &VideoConsole::refMyRow() noexcept {
  return const_cast<s32 &>(MyLock == PpcLock ? m_share->ppc_row
                                             : m_share->ios_row);
}

/**
 * Get the current row for the other instance.
 */
s32 &VideoConsole::refOtherRow() noexcept {
  return const_cast<s32 &>(MyLock == PpcLock ? m_share->ios_row
                                             : m_share->ppc_row);
}

/**
 * Increment the row for this instance.
 */
s32 VideoConsole::incrementRow() noexcept {
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
s32 VideoConsole::decrementRow() noexcept {
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
void VideoConsole::SetTabWidth(u8 width) noexcept {
  syncShare(false);

  constexpr u32 mask = 0xF << Option::TabWidth;
  width = static_cast<u8>((width - 1) << Option::TabWidth);
  m_share->option = (m_share->option & ~mask) | (width & mask);

  syncShare(true);
}

/**
 * Get the current TAB alignment width.
 */
u8 VideoConsole::GetTabWidth() const noexcept {
  return ((m_share->option >> Option::TabWidth) & 0xF) + 1;
}

/**
 * Get the width of the console framebuffer.
 */
u32 VideoConsole::GetXfbWidth() const noexcept { return m_share->xfb_width; }

/**
 * Get the height of the console framebuffer.
 */
u32 VideoConsole::GetXfbHeight() const noexcept { return m_share->xfb_height; }

/**
 * Get column count.
 */
u32 VideoConsole::NumCols() const noexcept {
  if (!(m_share->option & Option::Sideways)) {
    return m_share->xfb_width / GlyphWidth - 1;
  } else {
    return m_share->xfb_height / GlyphWidth - 1;
  }
}

/**
 * Get row count.
 */
u32 VideoConsole::NumRows() const noexcept {
  if (!(m_share->option & Option::Sideways)) {
    return m_share->xfb_height / GlyphHeight - 2;
  } else {
    return m_share->xfb_width / GlyphHeight - 2;
  }
}

/**
 * Read from the specified pixel on the framebuffer.
 */
u8 VideoConsole::ReadGrayscaleFromXfb(u32 x, u32 y) const noexcept {
  if (x > m_share->xfb_width || y > m_share->xfb_height) {
    return 16;
  }

  u32 val = m_share->xfb[y * (m_share->xfb_width / 2) + x / 2];
  if (x & 1) {
    return u8(val >> 8);
  } else {
    return u8(val >> 24);
  }
}

/**
 * Write to the specified pixel on the framebuffer.
 */
void VideoConsole::WriteGrayscaleToXfb(u32 x, u32 y, u8 intensity) noexcept {
  if (x > m_share->xfb_width || y > m_share->xfb_height) {
    return;
  }

  u32 *val = &m_share->xfb[y * (m_share->xfb_width / 2) + x / 2];
  u32 y0 = (*val >> 24) & 0xFF;
  u32 y1 = (*val >> 8) & 0xFF;
  if (x & 1) {
    y1 = intensity;
  } else {
    y0 = intensity;
  }
  *val = y0 << 24 | 127u << 16 | y1 << 8 | 127u;
}

/**
 * Move the framebuffer up by the specified height.
 */
void VideoConsole::MoveUp(u32 height) noexcept {
  if (!(m_share->option & Option::Sideways)) {
    u32 offset = height * (m_share->xfb_width / 2);

    u32 src = util::AlignDown(offset, 32u);
    u32 dest = 0;
    u32 totalSize =
        util::AlignDown(m_share->xfb_height * (m_share->xfb_width / 2), 32u);

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
void VideoConsole::FlushXfb() noexcept {
  util::CpuCache::DcFlush(m_share->xfb, 320 * 574 * sizeof(u32));
}

/**
 * Print a character. Assumes the resource is already locked.
 */
void VideoConsole::printChar(char c) noexcept {
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
    m_col = util::AlignUp<u8>(GetTabWidth(), m_col + 1);
  }

  if (m_col >= NumCols()) {
    incrementRow();
    m_col = 0;
  }

  if (refMyRow() < 0) {
    return;
  }
  u32 row = static_cast<u32>(refMyRow());

  while (row >= NumRows()) {
    VideoConsole::MoveUp(GlyphHeight);
    decrementRow();
    row--;
  }

  const u8 *glyph = VIConsoleFont[' '];
  if (u32(c) < 128) {
    glyph = VIConsoleFont[u32(c)];
  }

  u32 y0 = row * GlyphHeight + GlyphHeight / 2;
  for (u32 y = 0; y < GlyphHeight; y++) {
    u32 x0 = m_col * GlyphWidth + GlyphWidth / 2;
    for (u32 x = 0; x < GlyphWidth; x++) {
      u8 intensity = glyph[(y * GlyphWidth + x) / 8] & (1 << (8 - (x % 8)))
                         ? FgIntensity
                         : BgIntensity;
      if (m_share->option & Option::Sideways) {
        VideoConsole::WriteGrayscaleToXfb(
            y0 + y, m_share->xfb_height - (x0 + x), intensity);
      } else {
        VideoConsole::WriteGrayscaleToXfb(x0 + x, y0 + y, intensity);
      }
    }
  }

  m_col++;
}

/**
 * Print a string to the visual console.
 */
void VideoConsole::Print(const char *s) noexcept {
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
void VideoConsole::Print(const char *s, size_t len) noexcept {
  lock();
  for (u32 i = 0; i < len; i++) {
    printChar(s[i]);
  }
  FlushXfb();
  unlock();
}

} // namespace peli::log