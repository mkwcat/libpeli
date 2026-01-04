// peli/vi/Video.cpp - High level video manager
//   Written by mkwcat
//
// Copyright (c) 2025-2026 mkwcat
// SPDX-License-Identifier: MIT

#include "Video.hpp"
#include "../host/Host.hpp"
#include "../hw/VideoInterface.hpp"
#include "../ppc/Msr.hpp"
#include "../util/Address.hpp"
#include "../util/Time.hpp"

namespace peli::vi {

Video::Video(const RenderMode &rmode) noexcept : m_changed_framebuffer(false) {
  constexpr u16 taps[9] = {496, 476, 430, 372, 297, 219, 142, 70, 12};
  constexpr u8 taps2[14] = {226, 203, 192, 196, 207, 222, 236,
                            252, 8,   15,  19,  19,  15,  12};
  SetTaps(taps, taps2);
  m_changed_taps = true;

  m_dcr = VI::Dcr{
      .FMT = rmode.mode.GetDcrFmt(),
      .LE1 = VI::Dcr::Le::OFF,
      .LE0 = VI::Dcr::Le::OFF,
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
  m_htr0 = timings.HTR0;
  m_htr1 = timings.HTR1;
  m_bbei = timings.BBEI;
  m_bboi = timings.BBOI;
  m_hbe = timings.HBE;
  m_hbs = timings.HBS;
  m_changed_timings = true;

  m_vtr = VI::Vtr{
      .ACV = u16(rmode.xfb_height >> !is_progressive),
      .EQU = timings.VTR.EQU,
  };
  m_vto = VI::Vtoe{
      .PSB = timings.VTO.PSB + timings.VTR.ACV -
             u32(rmode.xfb_height >> !is_progressive),
      .PRB = timings.VTO.PRB + u32(rmode.vi_y_origin << is_progressive),
  };
  m_vte = VI::Vtoe{
      .PSB = timings.VTE.PSB + timings.VTR.ACV -
             u32(rmode.xfb_height >> !is_progressive),
      .PRB = timings.VTE.PRB + u32(rmode.vi_y_origin << is_progressive),
  };
  if (rmode.vi_y_origin & 1) {
    // Swap even and odd fields
    auto vto = m_vto;
    m_vto = m_vte;
    m_vte = vto;
  }

  m_changed_vertical = true;

  m_hsw = VI::Hsw{
      .WPL = u16(rmode.fb_width >> 4),
      .STD = u16(rmode.fb_width >> 3 >> is_progressive),
  };
  m_hsr = VI::Hsr{
      .HS_EN = false,
      .STP = u16(u32(rmode.fb_width) * 256u / rmode.vi_width),
  };
  m_changed_scaling = true;

  m_di[0] = timings.DI;
  m_di[1] = {.ENB = 1, .HCT = 1};
  m_di[2] = {};
  m_changed_interrupts = true;

  // TODO: Initialize A/V encoder
}

void Video::Flush() noexcept {
  if (!hw::VI->DCR.ENB || (m_changed_config && hw::VI->DCR.FMT != m_dcr.FMT)) {
    // Reset VI
    hw::VI->DCR = VI::Dcr{.RST = 1};
    util::BusDelay(100);
    hw::VI->DCR = VI::Dcr{.RST = 0};
  }

  if (m_changed_taps) {
    m_changed_taps = false;
    hw::VI->FCT0 = m_fct0;
    hw::VI->FCT1 = m_fct1;
    hw::VI->FCT2 = m_fct2;
    hw::VI->FCT3 = m_fct3;
    hw::VI->FCT4 = m_fct4;
    hw::VI->FCT5 = m_fct5;
    hw::VI->FCT6 = m_fct6;
  }

  if (m_changed_vertical) {
    m_changed_vertical = false;
    hw::VI->VTR = m_vtr;
    hw::VI->VTO = m_vto;
    hw::VI->VTE = m_vte;
  }

  if (m_changed_timings) {
    m_changed_timings = false;
    hw::VI->HTR0 = m_htr0;
    hw::VI->HTR1 = m_htr1;
    hw::VI->BBEI = m_bbei;
    hw::VI->BBOI = m_bboi;
    hw::VI->HBE = m_hbe;
    hw::VI->HBS = m_hbs;
  }

  if (m_changed_scaling) {
    m_changed_scaling = false;
    hw::VI->HSW = m_hsw;
    hw::VI->HSR = m_hsr;
  }

  if (m_changed_interrupts) {
    m_changed_interrupts = false;
    hw::VI->DI0 = m_di[0];
    hw::VI->DI1 = m_di[1];
    hw::VI->DI2 = m_di[2];
  }

  if (m_changed_config) {
    m_changed_config = false;
    hw::VI->DCR = m_dcr;
  }

  if (m_changed_framebuffer) {
    m_changed_framebuffer = false;
    hw::VI->TFBL = m_tfbl;
    hw::VI->BFBL = m_bfbl;
  }
}

namespace {

void *getFramebuffer(hw::VideoInterface::Fbl tfbl) {
  u32 fbb = tfbl.FBB << 9;
  if (tfbl.POFF) {
    fbb <<= 5;
  }
  if (fbb == 0) {
    return nullptr;
  }

  return reinterpret_cast<void *>(util::Effective(fbb));
}

void setFramebuffer(const hw::VideoInterface::Dcr dcr,
                    const hw::VideoInterface::Hsw hsw,
                    hw::VideoInterface::Fbl &tfbl,
                    hw::VideoInterface::Fbl &bfbl, void *top_framebuffer,
                    void *bottom_framebuffer) {
  u32 fbb_top = reinterpret_cast<u32>(util::Physical(top_framebuffer)) >> 5;
  u32 fbb_bottom;
  if (bottom_framebuffer) {
    fbb_bottom = reinterpret_cast<u32>(util::Physical(bottom_framebuffer)) >> 5;
  } else {
    fbb_bottom = fbb_top;
    if (!dcr.NIN) {
      fbb_bottom += hsw.WPL;
    }
  }

  ppc::Msr::NoInterruptsScope guard;

  tfbl = hw::VideoInterface::Fbl{
      .POFF = true,
      .XOF = 0,
      .FBB = fbb_top,
  };
  bfbl = hw::VideoInterface::Fbl{
      .POFF = true,
      .XOF = 0,
      .FBB = fbb_bottom,
  };
}

} // namespace

void *Video::GetFramebuffer(bool bottom_field) noexcept {
  return getFramebuffer(bottom_field ? hw::VI->BFBL : hw::VI->TFBL);
}

void Video::SetFramebuffer(void *top_framebuffer,
                           void *bottom_framebuffer) noexcept {
  VI::Fbl tfbl, bfbl;
  setFramebuffer(hw::VI->DCR, hw::VI->HSW, tfbl, bfbl, top_framebuffer,
                 bottom_framebuffer);
  hw::VI->TFBL = tfbl;
  hw::VI->BFBL = bfbl;
}

void *Video::GetNextFramebuffer(bool bottom_field) const noexcept {
  return getFramebuffer(bottom_field ? hw::VI->BFBL : hw::VI->TFBL);
}

void Video::SetNextFramebuffer(void *top_framebuffer,
                               void *bottom_framebuffer) noexcept {
  setFramebuffer(m_dcr, m_hsw, m_tfbl, m_bfbl, top_framebuffer,
                 bottom_framebuffer);
  m_changed_framebuffer = true;
}

u16 Video::GetXfbWidth() const noexcept { return m_hsw.WPL << 4; }

u16 Video::GetXfbHeight() const noexcept {
  return m_vtr.ACV << !(m_dcr.NIN & m_visel.VISEL & 1);
}

void *Video::AllocateXfb() noexcept {
  return host::Alloc(XfbAlignment, GetXfbSize());
}

} // namespace peli::vi
