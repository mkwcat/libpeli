// peli/hw/Video.cpp - High level video manager
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "Video.hpp"
#include "../ppc/Msr.hpp"
#include "../util/Address.hpp"
#include "../util/Time.hpp"
#include "VideoInterface.hpp"
#include <cstdlib>
#include <utility>

namespace peli::hw {

Video::Video(const RenderMode &rmode) noexcept
    : VideoInterface{}, m_changed_framebuffer(false) {
  constexpr u16 taps[9] = {496, 476, 430, 372, 297, 219, 142, 70, 12};
  constexpr u8 taps2[14] = {226, 203, 192, 196, 207, 222, 236,
                            252, 8,   15,  19,  19,  15,  12};
  SetTaps(taps, taps2);
  m_changed_taps = true;

  DCR = Dcr{
      .FMT = rmode.mode.GetDcrFmt(),
      .LE1 = Dcr::Le::OFF,
      .LE0 = Dcr::Le::OFF,
      .DLR = rmode.mode.interlace_mode == InterlaceMode::Progressive3D,
      .NIN = rmode.mode.interlace_mode != InterlaceMode::Interlace,
      .RST = 0,
      .ENB = 1,
  };
  m_changed_config = true;

  bool is_progressive =
      rmode.mode.interlace_mode == InterlaceMode::Progressive ||
      rmode.mode.interlace_mode == InterlaceMode::Progressive3D;

  const Timings &timings = rmode.mode.GetTimings();
  HTR0 = timings.HTR0;
  HTR1 = timings.HTR1;
  BBEI = timings.BBEI;
  BBOI = timings.BBOI;
  HBE = timings.HBE;
  HBS = timings.HBS;
  m_changed_timings = true;

  VTR = Vtr{
      .ACV = u16(rmode.xfb_height >> !is_progressive),
      .EQU = timings.VTR.EQU,
  };
  VTO = Vtoe{
      .PSB = timings.VTO.PSB + timings.VTR.ACV -
             u32(rmode.xfb_height >> !is_progressive),
      .PRB = timings.VTO.PRB + u32(rmode.vi_y_origin << is_progressive),
  };
  VTE = Vtoe{
      .PSB = timings.VTE.PSB + timings.VTR.ACV -
             u32(rmode.xfb_height >> !is_progressive),
      .PRB = timings.VTE.PRB + u32(rmode.vi_y_origin << is_progressive),
  };
  if (rmode.vi_y_origin & 1) {
    std::swap(VTO, VTE);
  }

  m_changed_vertical = true;

  HSW = Hsw{
      .WPL = u16(rmode.fb_width >> 4),
      .STD = u16(rmode.fb_width >> 3 >> is_progressive),
  };
  HSR = Hsr{
      .HS_EN = false,
      .STP = u16(u32(rmode.fb_width) * 256u / rmode.vi_width),
  };
  m_changed_scaling = true;

  DI0 = DI0 = timings.DI;
  DI1 = DI1 = {.ENB = 1, .HCT = 1};
  DI2 = DI2 = 0;
  m_changed_interrupts = true;

  // TODO: Initialize A/V encoder
}

void Video::Flush() noexcept {
  if (!VI->DCR.ENB || (m_changed_config && VI->DCR.FMT != DCR.FMT)) {
    // Reset VI
    VI->DCR = Dcr{.RST = 1};
    util::BusDelay(100);
    VI->DCR = Dcr{.RST = 0};
  }

  if (m_changed_taps) {
    m_changed_taps = false;
    VI->FCT0 = FCT0;
    VI->FCT1 = FCT1;
    VI->FCT2 = FCT2;
    VI->FCT3 = FCT3;
    VI->FCT4 = FCT4;
    VI->FCT5 = FCT5;
    VI->FCT6 = FCT6;
  }

  if (m_changed_vertical) {
    m_changed_vertical = false;
    VI->VTR = VTR;
    VI->VTO = VTO;
    VI->VTE = VTE;
  }

  if (m_changed_timings) {
    m_changed_timings = false;
    VI->HTR0 = HTR0;
    VI->HTR1 = HTR1;
    VI->BBEI = BBEI;
    VI->BBOI = BBOI;
    VI->HBE = HBE;
    VI->HBS = HBS;
  }

  if (m_changed_scaling) {
    m_changed_scaling = false;
    VI->HSW = HSW;
    VI->HSR = HSR;
  }

  if (m_changed_interrupts) {
    m_changed_interrupts = false;
    VI->DI0 = DI0;
    VI->DI1 = DI1;
    VI->DI2 = DI2;
  }

  if (m_changed_config) {
    m_changed_config = false;
    VI->DCR = DCR;
  }

  if (m_changed_framebuffer) {
    m_changed_framebuffer = false;
    VI->TFBL = TFBL;
    VI->BFBL = BFBL;
  }
}

namespace {

void *getFramebuffer(VideoInterface::Fbl tfbl) {
  u32 fbb = tfbl.FBB << 9;
  if (tfbl.POFF) {
    fbb <<= 5;
  }
  if (fbb == 0) {
    return nullptr;
  }

  return reinterpret_cast<void *>(util::Effective(fbb));
}

void setFramebuffer(VideoInterface *vi, void *top_framebuffer,
                    void *bottom_framebuffer) {
  u32 fbb_top = reinterpret_cast<u32>(util::Physical(top_framebuffer)) >> 5;
  u32 fbb_bottom;
  if (bottom_framebuffer) {
    fbb_bottom = reinterpret_cast<u32>(util::Physical(bottom_framebuffer)) >> 5;
  } else {
    fbb_bottom = fbb_top;
    if (!vi->DCR.NIN) {
      fbb_bottom += vi->HSW.WPL;
    }
  }

  ppc::Msr::NoInterruptsScope guard;

  vi->TFBL = VideoInterface::Fbl{
      .POFF = true,
      .XOF = 0,
      .FBB = fbb_top,
  };
  vi->BFBL = VideoInterface::Fbl{
      .POFF = true,
      .XOF = 0,
      .FBB = fbb_bottom,
  };
}

} // namespace

void *Video::GetFramebuffer(bool bottom_field) noexcept {
  return getFramebuffer(bottom_field ? VI->BFBL : VI->TFBL);
}

void Video::SetFramebuffer(void *top_framebuffer,
                           void *bottom_framebuffer) noexcept {
  setFramebuffer(VI, top_framebuffer, bottom_framebuffer);
}

void *Video::GetNextFramebuffer(bool bottom_field) const noexcept {
  return getFramebuffer(bottom_field ? VI->BFBL : VI->TFBL);
}

void Video::SetNextFramebuffer(void *top_framebuffer,
                               void *bottom_framebuffer) noexcept {
  setFramebuffer(this, top_framebuffer, bottom_framebuffer);
  m_changed_framebuffer = true;
}

u16 Video::GetXfbWidth() const noexcept { return HSW.WPL << 4; }

u16 Video::GetXfbHeight() const noexcept {
  return VTR.ACV << !(DCR.NIN & VISEL.VISEL & 1);
}

void *Video::AllocateXfb() noexcept {
  return std::aligned_alloc(XfbAlignment, GetXfbSize());
}

} // namespace peli::hw
