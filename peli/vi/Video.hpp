// peli/vi/Video.hpp - High level video manager
//   Written by mkwcat
//
// Copyright (c) 2025-2026 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../hw/VideoInterface.hpp"

namespace peli::vi {

/**
 * High-level video manager.
 */
class Video {
  using VI = hw::VideoInterface;

public:
  static constexpr u32 XfbAlignment = 0x4000;

  static constexpr u32 MaxWidthNtsc = 720;
  static constexpr u32 MaxHeightNtsc = 480;

  static constexpr u32 MaxWidthPal = 720;
  static constexpr u32 MaxHeightPal = 576;

  static constexpr u32 MaxWidthMpal = 720;
  static constexpr u32 MaxHeightMpal = 480;

  static constexpr u32 MaxWidthEurgb60 = 720;
  static constexpr u32 MaxHeightEurgb60 = 480;

  enum class Standard : u8 {
    Ntsc,
    Pal,
    PalM,
    Debug,
    DebugPal,
    Eurgb60,
    Gca,
    Extra,
    HD720,
  };

  enum class InterlaceMode : u8 {
    Interlace,
    NonInterlace,
    Progressive,
    Progressive3D,
  };

  struct Timings {
    hw::VideoInterface::Htr0 HTR0;
    VI::Htr1 HTR1;
    VI::Vtr VTR;
    VI::Vtoe VTO;
    VI::Vtoe VTE;
    VI::Bbei BBEI;
    VI::Bboi BBOI;
    VI::Hbe HBE;
    VI::Hbs HBS;
    VI::Di DI;
  };

  struct RenderMode;

  struct TVMode {
    Standard standard;
    InterlaceMode interlace_mode;

    const Timings &GetTimings() const noexcept;
    const RenderMode &GetRenderMode() const noexcept;
    VI::Dcr::Fmt GetDcrFmt() const noexcept {
      return Video::GetDcrFmt(standard);
    }

    static TVMode Detect() noexcept;
  };

  struct RenderMode {
    TVMode mode;
    u16 fb_width;
    u16 efb_height;
    u16 xfb_height;
    u16 vi_x_origin, vi_y_origin;
    u16 vi_width;
    u16 vi_height;
    u8 field_rendering;
    u8 antialiasing;
    u8 sample_pattern[12][2];
    u8 vertical_filter[7];
  };

  static const struct {
    struct {
      Timings NtscEurgb60, Pal, PalM, Gca;
    } Interlaced;
    struct {
      Timings NtscEurgb60, Pal, PalM;
    } NonInterlaced;
    struct {
      Timings NtscPalMEurgb60, Gca, Pal;
    } Progressive;
    Timings Ntsc3D;
  } TimingSets;

  static const struct {
    struct {
      RenderMode Interlaced, NonInterlaced, Progressive;
    } Ntsc, Pal, PalM, Eurgb60;
  } RenderModes;

  static VI::Dcr::Fmt GetDcrFmt(Standard standard) noexcept {
    switch (standard) {
    default:
      return VI::Dcr::Fmt::NTSC;
    case Standard::Pal:
      return VI::Dcr::Fmt::PAL;
    case Standard::PalM:
      return VI::Dcr::Fmt::MPAL;
    case Standard::Debug:
      return VI::Dcr::Fmt::DEBUG;
    }
  }

  void GetTaps(u16 taps[9], u8 taps2[14]) const noexcept {
    taps[0] = m_fct0.T0;
    taps[1] = m_fct0.T1;
    taps[2] = m_fct0.T2;
    taps[3] = m_fct1.T3;
    taps[4] = m_fct1.T4;
    taps[5] = m_fct1.T5;
    taps[6] = m_fct2.T6;
    taps[7] = m_fct2.T7;
    taps[8] = m_fct2.T8;
    taps2[0] = m_fct3.T9;
    taps2[1] = m_fct3.T10;
    taps2[2] = m_fct3.T11;
    taps2[3] = m_fct3.T12;
    taps2[4] = m_fct4.T13;
    taps2[5] = m_fct4.T14;
    taps2[6] = m_fct4.T15;
    taps2[7] = m_fct4.T16;
    taps2[8] = m_fct5.T17;
    taps2[9] = m_fct5.T18;
    taps2[10] = m_fct5.T19;
    taps2[11] = m_fct5.T20;
    taps2[12] = m_fct6.T21;
    taps2[13] = m_fct6.T22;
  }

  void SetTaps(const u16 taps[9], const u8 taps2[14]) noexcept {
    m_fct0 = {
        .T2 = taps[2] & 0x3FFu,
        .T1 = taps[1] & 0x3FFu,
        .T0 = taps[0] & 0x3FFu,
    };
    m_fct1 = {
        .T5 = taps[5] & 0x3FFu,
        .T4 = taps[4] & 0x3FFu,
        .T3 = taps[3] & 0x3FFu,
    };
    m_fct2 = {
        .T8 = taps[8] & 0x3FFu,
        .T7 = taps[7] & 0x3FFu,
        .T6 = taps[6] & 0x3FFu,
    };
    m_fct3 = {
        .T12 = taps2[3],
        .T11 = taps2[2],
        .T10 = taps2[1],
        .T9 = taps2[0],
    };
    m_fct4 = {
        .T16 = taps2[7],
        .T15 = taps2[6],
        .T14 = taps2[5],
        .T13 = taps2[4],
    };
    m_fct5 = {
        .T20 = taps2[11],
        .T19 = taps2[10],
        .T18 = taps2[9],
        .T17 = taps2[8],
    };
    m_fct6 = {
        .T23 = taps2[13],
        .T22 = taps2[12],
        .T21 = taps2[11],
    };
  }

  Video(const RenderMode &rmode) noexcept;
  Video() noexcept : Video(TVMode::Detect().GetRenderMode()) {}

  void Flush() noexcept;
  void FlushAll() noexcept {
    m_changed_config = m_changed_vertical = m_changed_timings = m_changed_taps =
        m_changed_scaling = m_changed_interrupts = m_changed_framebuffer = true;
    Flush();
  }

  static void *GetFramebuffer(bool bottom_field = false) noexcept;
  static void SetFramebuffer(void *top_framebuffer,
                             void *bottom_framebuffer = nullptr) noexcept;

  void *GetNextFramebuffer(bool bottom_field = false) const noexcept;
  void SetNextFramebuffer(void *top_framebuffer,
                          void *bottom_framebuffer = nullptr) noexcept;

  u16 GetXfbWidth() const noexcept;
  u16 GetXfbHeight() const noexcept;
  u32 GetXfbSize() const noexcept {
    return GetXfbWidth() * GetXfbHeight() * sizeof(u32);
  }

  void *AllocateXfb() noexcept;

  void SetupEncoder() noexcept;

private:
  bool m_changed_config, m_changed_vertical, m_changed_timings, m_changed_taps,
      m_changed_scaling, m_changed_interrupts, m_changed_framebuffer;

  VI::Dcr m_dcr;

  VI::Di m_di[3];

  VI::Fbl m_tfbl;
  VI::Fbl m_bfbl;

  VI::Fct0 m_fct0;
  VI::Fct1 m_fct1;
  VI::Fct2 m_fct2;
  VI::Fct3 m_fct3;
  VI::Fct4 m_fct4;
  VI::Fct5 m_fct5;
  VI::Fct6 m_fct6;

  VI::Vtr m_vtr;
  VI::Vtoe m_vto;
  VI::Vtoe m_vte;
  VI::ViSel m_visel;

  VI::Htr0 m_htr0;
  VI::Htr1 m_htr1;
  VI::Bbei m_bbei;
  VI::Bboi m_bboi;
  VI::Hbe m_hbe;
  VI::Hbs m_hbs;

  VI::Hsw m_hsw;
  VI::Hsr m_hsr;
};

} // namespace peli::vi
