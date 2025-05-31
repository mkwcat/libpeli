#pragma once

#include "VideoInterface.hpp"

namespace wsh::hw {

/**
 * High-level video manager.
 */
class Video : private VideoInterface {
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
    Htr0 HTR0;
    Htr1 HTR1;
    Vtr VTR;
    Vtoe VTO;
    Vtoe VTE;
    Bbei BBEI;
    Bboi BBOI;
    Hbe HBE;
    Hbs HBS;
    Di DI;
  };

  struct RenderMode;

  struct TVMode {
    Standard standard;
    InterlaceMode interlace_mode;

    const Timings &GetTimings() const noexcept;
    const RenderMode &GetRenderMode() const noexcept;
    Dcr::Fmt GetDcrFmt() const noexcept { return Video::GetDcrFmt(standard); }

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

  static const Timings TimingsInterlacedNtscEurgb60;
  static const Timings TimingsNonInterlacedNtscEurgb60;
  static const Timings TimingsInterlacedPal;
  static const Timings TimingsNonInterlacedPal;
  static const Timings TimingsInterlacedPalM;
  static const Timings TimingsNonInterlacedPalM;
  static const Timings TimingsProgressiveNtscPalMEurgb60;
  static const Timings TimingsNtsc3D;
  static const Timings TimingsInterlacedGca;
  static const Timings TimingsProgressiveGca;
  static const Timings TimingsProgressivePal;

  static const RenderMode RenderModeNtscInterlaced;
  static const RenderMode RenderModeNtscNonInterlaced;
  static const RenderMode RenderModeNtscProgressive;
  static const RenderMode RenderModePalInterlaced;
  static const RenderMode RenderModePalNonInterlaced;
  static const RenderMode RenderModePalProgressive;
  static const RenderMode RenderModePalMInterlaced;
  static const RenderMode RenderModePalMNonInterlaced;
  static const RenderMode RenderModePalMProgressive;
  static const RenderMode RenderModeEurgb60Interlaced;
  static const RenderMode RenderModeEurgb60NonInterlaced;
  static const RenderMode RenderModeEurgb60Progressive;

  static Dcr::Fmt GetDcrFmt(Standard standard) noexcept {
    switch (standard) {
    default:
      return Dcr::Fmt::NTSC;
    case Standard::Pal:
      return Dcr::Fmt::PAL;
    case Standard::PalM:
      return Dcr::Fmt::MPAL;
    case Standard::Debug:
      return Dcr::Fmt::DEBUG;
    }
  }

  void GetTaps(u16 taps[9], u8 taps2[14]) const noexcept {
    taps[0] = FCT0.T0;
    taps[1] = FCT0.T1;
    taps[2] = FCT0.T2;
    taps[3] = FCT1.T3;
    taps[4] = FCT1.T4;
    taps[5] = FCT1.T5;
    taps[6] = FCT2.T6;
    taps[7] = FCT2.T7;
    taps[8] = FCT2.T8;
    taps2[0] = FCT3.T9;
    taps2[1] = FCT3.T10;
    taps2[2] = FCT3.T11;
    taps2[3] = FCT3.T12;
    taps2[4] = FCT4.T13;
    taps2[5] = FCT4.T14;
    taps2[6] = FCT4.T15;
    taps2[7] = FCT4.T16;
    taps2[8] = FCT5.T17;
    taps2[9] = FCT5.T18;
    taps2[10] = FCT5.T19;
    taps2[11] = FCT5.T20;
    taps2[12] = FCT6.T21;
    taps2[13] = FCT6.T22;
  }

  void SetTaps(const u16 taps[9], const u8 taps2[14]) noexcept {
    FCT0 = {
        .T2 = taps[2],
        .T1 = taps[1],
        .T0 = taps[0],
    };
    FCT1 = {
        .T5 = taps[5],
        .T4 = taps[4],
        .T3 = taps[3],
    };
    FCT2 = {
        .T8 = taps[8],
        .T7 = taps[7],
        .T6 = taps[6],
    };
    FCT3 = {
        .T12 = taps2[3],
        .T11 = taps2[2],
        .T10 = taps2[1],
        .T9 = taps2[0],
    };
    FCT4 = {
        .T16 = taps2[7],
        .T15 = taps2[6],
        .T14 = taps2[5],
        .T13 = taps2[4],
    };
    FCT5 = {
        .T20 = taps2[11],
        .T19 = taps2[10],
        .T18 = taps2[9],
        .T17 = taps2[8],
    };
    FCT6 = {
        .T23 = taps2[13],
        .T22 = taps2[12],
        .T21 = taps2[11],
    };
  }

  Video(const RenderMode &rmode) noexcept;
  Video() noexcept : Video(TVMode::Detect().GetRenderMode()) {}

  void Flush() noexcept;
  void FlushAll() noexcept {
    m_changed_config = true;
    m_changed_vertical = true;
    m_changed_timings = true;
    m_changed_taps = true;
    m_changed_scaling = true;
    m_changed_interrupts = true;
    m_changed_framebuffer = true;
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
  bool m_changed_config;
  bool m_changed_vertical;
  bool m_changed_timings;
  bool m_changed_taps;
  bool m_changed_scaling;
  bool m_changed_interrupts;
  bool m_changed_framebuffer;
};

} // namespace wsh::hw