// wsh/util/ViConsole.cpp - Visual debug console shared between PPC and IOS
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

#include "ViConsole.hpp"
#include "../hw/VideoInterface.hpp"
#include "../util/Address.hpp"
#include "../util/CpuCache.hpp"
#include <malloc.h>

extern "C" const u8 ViConsoleFont[128][16];

namespace wsh::util {

constexpr u8 BG_INTENSITY = 16;
constexpr u8 FG_INTENSITY = 235;

constexpr u8 GLYPH_WIDTH = 8;
constexpr u8 GLYPH_HEIGHT = 16;

enum Flag {
  SIDEWAYS = 1 << 0,
};

#ifdef WSH_HOST_PPC

/**
 * Create and configure VI for the debug console.
 */
ViConsole::ViConsole(bool sideways) noexcept {
  m_shareBlock = static_cast<Share *>(memalign(32, sizeof(Share)));

  CpuCache::DcFlush(m_shareBlock, sizeof(Share));

  m_share = util::Uncached(m_shareBlock);

  ConfigureVideo(true);

  m_share->lock = 0;
  m_share->ppcRow = -1;
  m_share->iosRow = -1;
  m_share->xfbInit = true;
  m_share->option = sideways ? Flag::SIDEWAYS : 0;
}

/**
 * Configure VI for the debug console.
 * @param clear Clear the XFB.
 */
void ViConsole::ConfigureVideo(bool clear) noexcept {
  bool isNtsc = hw::VI->DCR.FMT == hw::VI->DCR.Fmt::NTSC;
  bool isProgressive = isNtsc && (hw::VI->VISEL.VISEL & 1 || hw::VI->DCR.NIN);
  m_share->xfbWidth = 608;
  m_share->xfbHeight = isNtsc ? 448 : 538;

  if (m_share->xfb == nullptr) {
    m_share->xfb = static_cast<u32 *>(
        memalign(16384, m_share->xfbWidth * m_share->xfbHeight * sizeof(u32)));
  }

  if (clear) {
    for (u16 y = 0; y < m_share->xfbHeight; y++) {
      for (u16 x = 0; x < m_share->xfbWidth; x++) {
        WriteGrayscaleToXfb(x, y, 16);
      }
    }
    FlushXfb();
  }

  hw::VI->VTR = hw::VideoInterface::Vtr{
      .ACV = u16(m_share->xfbHeight >> (isProgressive ? 0 : 1)),
      .EQU = u16((isNtsc ? 12 : 10) >> (isProgressive ? 0 : 1)),
  };

  hw::VideoInterface::Vtoe vto;
  hw::VideoInterface::Vtoe vte;
  if (isProgressive) {
    vto = {
        .PSB = 38,
        .PRB = 80,
    };
    vte = vto;
  } else if (isNtsc) {
    vto = {
        .PSB = 19,
        .PRB = 40,
    };
    vte = {
        .PSB = 18,
        .PRB = 41,
    };
  } else {
    vto = {
        .PSB = 19,
        .PRB = 53,
    };
    vte = {
        .PSB = 18,
        .PRB = 54,
    };
  }
  hw::VI->VTO = vto;
  hw::VI->VTE = vte;

  hw::VI->HSW = hw::VideoInterface::Hsw{
      .WPL = u16(m_share->xfbWidth >> 4),
      .STD = u16(m_share->xfbWidth >> 3 >> isProgressive),
  };

  hw::VI->HSR = hw::VideoInterface::Hsr{
      .HS_EN = true,
      .STP = u16(isNtsc ? 234 : 233),
  };

  hw::VI->TFBL = hw::VideoInterface::Fbl{
      .POFF = true,
      .XOF = 0,
      .FBB = u32(Physical(m_share->xfb)) >> (5 + 9),
  };

  hw::VI->BFBL = hw::VideoInterface::Fbl{
      .POFF = true,
      .XOF = 0,
      .FBB = u32(Physical(m_share->xfb +
                          !isProgressive * m_share->xfbWidth / 2)) >>
             (5 + 9),
  };

  m_col = 0;
}

#endif

/**
 * Recreate the debug console using an existing shared configuration.
 */
ViConsole::ViConsole(Share *share) noexcept {
  m_share = share;
#ifdef WSH_HOST_PPC
  m_share = util::Uncached(share);
#endif

  CpuCache::DcInvalidate(const_cast<Share *>(m_share), sizeof(Share));

  Print("\n");
}

/**
 * Flush or invalidate the share block.
 *
 * @param write Flush the block instead of invalidate.
 */
void ViConsole::syncShare([[maybe_unused]] bool write) noexcept {
#if defined(WSH_HOST_IOS)
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
void ViConsole::lock() noexcept {
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
void ViConsole::unlock() noexcept {
  syncShare(false);
  m_share->lock = 0;
  syncShare(true);
}

/**
 * Get the current row for this instance.
 */
s32 &ViConsole::refMyRow() noexcept {
  return const_cast<s32 &>(MyLock == PpcLock ? m_share->ppcRow
                                             : m_share->iosRow);
}

/**
 * Get the current row for the other instance.
 */
s32 &ViConsole::refOtherRow() noexcept {
  return const_cast<s32 &>(MyLock == PpcLock ? m_share->iosRow
                                             : m_share->ppcRow);
}

/**
 * Increment the row for this instance.
 */
s32 ViConsole::incrementRow() noexcept {
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
s32 ViConsole::decrementRow() noexcept {
  syncShare(false);

  m_share->iosRow -= 1;
  m_share->ppcRow -= 1;

  syncShare(true);

  return refMyRow();
}

/**
 * Get the width of the console framebuffer.
 */
u16 ViConsole::GetXfbWidth() const noexcept { return m_share->xfbWidth; }

/**
 * Get the height of the console framebuffer.
 */
u16 ViConsole::GetXfbHeight() const noexcept { return m_share->xfbHeight; }

/**
 * Get column count.
 */
u8 ViConsole::NumCols() const noexcept {
  if (!(m_share->option & Flag::SIDEWAYS)) {
    return m_share->xfbWidth / GLYPH_WIDTH - 1;
  } else {
    return m_share->xfbHeight / GLYPH_WIDTH - 1;
  }
}

/**
 * Get row count.
 */
u8 ViConsole::NumRows() const noexcept {
  if (!(m_share->option & Flag::SIDEWAYS)) {
    return m_share->xfbHeight / GLYPH_HEIGHT - 2;
  } else {
    return m_share->xfbWidth / GLYPH_HEIGHT - 2;
  }
}

/**
 * Read from the specified pixel on the framebuffer.
 */
u8 ViConsole::ReadGrayscaleFromXfb(u16 x, u16 y) const noexcept {
  if (x > m_share->xfbWidth || y > m_share->xfbHeight) {
    return 16;
  }

  u32 val = m_share->xfb[y * (m_share->xfbWidth / 2) + x / 2];
  if (x & 1) {
    return val >> 8;
  } else {
    return val >> 24;
  }
}

/**
 * Write to the specified pixel on the framebuffer.
 */
void ViConsole::WriteGrayscaleToXfb(u16 x, u16 y, u8 intensity) noexcept {
  if (x > m_share->xfbWidth || y > m_share->xfbHeight) {
    return;
  }

  u32 *val = &m_share->xfb[y * (m_share->xfbWidth / 2) + x / 2];
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
void ViConsole::MoveUp(u16 height) noexcept {
  if (!(m_share->option & Flag::SIDEWAYS)) {
    u32 offset = height * (m_share->xfbWidth / 2);

    u32 src = AlignDown(offset, 32);
    u32 dest = 0;
    u32 totalSize = AlignDown(m_share->xfbHeight * (m_share->xfbWidth / 2), 32);

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
    u32 lineCount = m_share->xfbWidth / 2 - offset;

    for (u32 y = 0; y < m_share->xfbHeight; y++) {
      u32 src = y * (m_share->xfbWidth / 2) + offset;
      u32 dest = y * (m_share->xfbWidth / 2);

      for (u32 i = 0; i < lineCount; i++) {
        m_share->xfb[dest++] = m_share->xfb[src++];
      }

      for (u32 i = 0; i < offset; i++) {
        WriteGrayscaleToXfb(m_share->xfbWidth - offset + i, y, 16);
      }
    }
  }
}

/**
 * Flush the XFB to main memory after writing to it.
 */
void ViConsole::FlushXfb() noexcept {
  CpuCache::DcFlush(m_share->xfb, 320 * 574 * sizeof(u32));
}

/**
 * Print a character. Assumes the resource is already locked.
 */
void ViConsole::printChar(char c) noexcept {
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

  if (m_col >= NumCols()) {
    incrementRow();
    m_col = 0;
  }

  s32 row = refMyRow();

  if (row < 0) {
    return;
  }

  while (row >= NumRows()) {
    ViConsole::MoveUp(GLYPH_HEIGHT);
    row = decrementRow();
  }

  const u8 *glyph = ViConsoleFont[' '];
  if (u32(c) < 128) {
    glyph = ViConsoleFont[u32(c)];
  }

  u16 y0 = row * GLYPH_HEIGHT + GLYPH_HEIGHT / 2;
  for (u16 y = 0; y < GLYPH_HEIGHT; y++) {
    u16 x0 = m_col * GLYPH_WIDTH + GLYPH_WIDTH / 2;
    for (u16 x = 0; x < GLYPH_WIDTH; x++) {
      u8 intensity = glyph[(y * GLYPH_WIDTH + x) / 8] & (1 << (8 - (x % 8)))
                         ? FG_INTENSITY
                         : BG_INTENSITY;
      if (m_share->option & Flag::SIDEWAYS) {
        ViConsole::WriteGrayscaleToXfb(y0 + y, m_share->xfbHeight - (x0 + x),
                                       intensity);
      } else {
        ViConsole::WriteGrayscaleToXfb(x0 + x, y0 + y, intensity);
      }
    }
  }

  m_col++;
}

/**
 * Print a string to the visual console.
 */
void ViConsole::Print(const char *s) noexcept {
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
void ViConsole::Print(const char *s, std::size_t len) noexcept {
  lock();
  for (u32 i = 0; i < len; i++) {
    printChar(s[i]);
  }
  FlushXfb();
  unlock();
}

} // namespace wsh::util