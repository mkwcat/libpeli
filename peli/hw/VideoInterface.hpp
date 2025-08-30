// peli/hw/VideoInterface.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.h"
#include "Register.hpp"

namespace peli::hw {

static struct VideoInterface {
public:
  struct Vtr {
    using Size = u16;

    /* 14-15 */ u16 : 2;

    /**
     * Active Video (in full lines?). This value is double buffered.
     */
    /* 4-13 */ volatile u16 ACV : 10 = 0;

    /**
     * Equalization pulse in half lines.
     */
    /* 0-3 */ volatile u16 EQU : 4 = 0;
  };

  /**
   * Vertical Timing Register.
   *
   * Address: 0xCC002000
   * Size: u16
   */
  Register<Vtr> VTR;

  struct Dcr {
    using Size = u16;

    /* 10-15 */ u16 : 6;

    enum class Fmt : u16 {
      NTSC = 0,
      PAL = 1,
      MPAL = 2,
      DEBUG = 3,
    };

    /**
     * Current Video Format.
     */
    /* 8-9 */ volatile Fmt FMT : 2 = Fmt::NTSC;

    enum class Le : u16 {
      OFF = 0,
      ON_1_FLD = 1,
      ON_2_FLD = 2,
      ALWAYS_ON = 3,
    };

    /**
     * Enable Display Latch 1.
     */
    /* 6-7 */ volatile Le LE1 : 2 = Le::OFF;

    /**
     * Enable Display Latch 0.
     */
    /* 4-5 */ volatile Le LE0 : 2 = Le::OFF;

    /**
     * Enable 3D Display mode.
     */
    /* 3 */ volatile u16 DLR : 1 = 0;

    /**
     * Enable Non-Interlaced; top field drawn at field rate and bottom field is
     * not displayed.
     */
    /* 2 */ volatile u16 NIN : 1 = 0;

    /**
     * Reset; clear all data requests and put VI into its idle state.
     */
    /* 1 */ volatile u16 RST : 1 = 0;

    /**
     * Enable video timing generation and data request.
     */
    /* 0 */ volatile u16 ENB : 1 = 0;
  };

  /**
   * Display Configuration Register.
   *
   * Address: 0xCC002002
   * Size: u16
   */
  Register<Dcr> DCR;

  struct Htr0 {
    using Size = u32;

    /* 31 */ u32 : 1;

    /**
     * Horizontal Sync Start to Color Burst Start.
     */
    /* 24-30 */ volatile u32 HCS : 7 = 0;

    /* 23 */ u32 : 1;

    /**
     * Horizontal Sync Start to Color Burst End.
     */
    /* 16-22 */ volatile u32 HCE : 7 = 0;

    /* 9-15 */ u32 : 7;

    /**
     * Halfline Width (W*16 = Width (720)).
     */
    /* 0-8 */ volatile u32 HLW : 9 = 0;
  };

  /**
   * Horizontal Timing Register 0.
   *
   * Address: 0xCC002004
   * Size: u32
   */
  Register<Htr0> HTR0;

  struct Htr1 {
    using Size = u32;

    /* 27-31 */ u32 : 5;

    /**
     * Halfline to Horizontal Blanking Start.
     */
    /* 17-26 */ volatile u32 HBS : 10 = 0;

    /**
     * Horizontal Sync Start to Horizontal Blank End.
     */
    /* 7-16 */ volatile u32 HBE : 10 = 0;

    /**
     * Horizontal Sync Width.
     */
    /* 0-6 */ volatile u32 HSY : 7 = 0;
  };

  /**
   * Horizontal Timing Register 1.
   *
   * Address: 0xCC002008
   * Size: u32
   */
  Register<Htr1> HTR1;

  struct Vtoe {
    using Size = u32;

    /* 26-31 */ u32 : 6;

    /**
     * Post-blanking in half lines.
     */
    /* 16-25 */ volatile u32 PSB : 10 = 0;

    /* 10-15 */ u32 : 6;

    /**
     * Pre-blanking in half lines.
     */
    /* 0-9 */ volatile u32 PRB : 10 = 0;
  };

  /**
   * Odd Field Vertical Timing Register.
   *
   * Address: 0xCC00200C
   * Size: u32
   */
  Register<Vtoe> VTO;

  /**
   * Even Field Vertical Timing Register.
   *
   * Address: 0xCC002010
   * Size: u32
   */
  Register<Vtoe> VTE;

  struct Bbei {
    using Size = u32;

    /**
     * Field 3 start to burst blanking end in half lines.
     */
    /* 21-31 */ volatile u32 BE3 : 11 = 0;

    /**
     * Field 3 start to burst blanking start in half lines.
     */
    /* 16-20 */ volatile u32 BS3 : 5 = 0;

    /**
     * Field 1 start to burst blanking end in half lines.
     */
    /* 5-15 */ volatile u32 BE1 : 11 = 0;

    /**
     * Field 1 start to burst blanking start in half lines.
     */
    /* 0-4 */ volatile u32 BS1 : 5 = 0;
  };

  /**
   * Odd Field Burst Blanking Interval Register.
   *
   * Address: 0xCC002014
   * Size: u32
   */
  Register<Bbei> BBEI;

  struct Bboi {
    using Size = u32;

    /**
     * Field 4 start to burst blanking end in half lines.
     */
    /* 21-31 */ volatile u32 BE4 : 11 = 0;

    /**
     * Field 4 start to burst blanking start in half lines.
     */
    /* 16-20 */ volatile u32 BS4 : 5 = 0;

    /**
     * Field 2 start to burst blanking end in half lines.
     */
    /* 5-15 */ volatile u32 BE2 : 11 = 0;

    /**
     * Field 2 start to burst blanking start in half lines.
     */
    /* 0-4 */ volatile u32 BS2 : 5 = 0;
  };

  /**
   * Even Field Burst Blanking Interval Register.
   *
   * Address: 0xCC002018
   * Size: u32
   */
  Register<Bboi> BBOI;

  struct Fbl {
    using Size = u32;

    /**
     * Always zero (maybe some write-only control register stuff?).
     */
    /* 29-31 */ u32 : 3;

    /**
     * Page offset bit.
     */
    /* 28 */ volatile u32 POFF : 1 = 0;

    /**
     * Horizontal Offset of the left-most pixel within the first word of the
     * fetched picture.
     */
    /* 24-27 */ volatile u32 XOF : 4 = 0;

    /**
     * XFB Address.
     */
    /* 0-23 */ volatile u32 FBB : 24 = 0;
  };

  /**
   * Top Field Base Register (L) (External Framebuffer Half 1).
   *
   * Address: 0xCC00201C
   * Size: u32
   */
  Register<Fbl> TFBL;

  struct Fbr {
    using Size = u32;

    /**
     * External Memory Address of frame buffer.
     */
    /* 0-31 */ volatile u32 FBB : 32 = 0;
  };

  /**
   * Top Field Base Register (R) (Only valid in 3D Mode).
   *
   * Address: 0xCC002020
   * Size: u32
   */
  Register<Fbr> TFBR;

  /**
   * Bottom Field Base Register (L) (External Framebuffer Half 2).
   *
   * Address: 0xCC002024
   * Size: u32
   */

  Register<Fbl> BFBL;

  /**
   * Bottom Field Base Register (R) (Only valid in 3D Mode).
   *
   * Address: 0xCC002028
   * Size: u32
   */
  Register<Fbr> BFBR;

  struct Dpv {
    using Size = u16;

    /* 11-15 */ u16 : 5;

    /**
     * Current vertical Position of Raster beam.
     */
    /* 0-10 */ volatile u16 VCT : 11 = 0;
  };

  /**
   * Current vertical Position.
   *
   * Address: 0xCC00202C
   * Size: u16
   */
  Register<Dpv> DPV;

  struct Dph {
    using Size = u16;

    /* 11-15 */ u16 : 5;

    /**
     * Current horizontal Position of Raster beam.
     */
    /* 0-10 */ volatile u16 HCT : 11 = 0;
  };

  /**
   * Current horizontal Position.
   *
   * Address: 0xCC00202E
   * Size: u16
   */
  Register<Dph> DPH;

  struct Di {
    using Size = u32;

    /**
     * Interrupt Status (1=Active) (Write to clear).
     */
    /* 31 */ volatile u32 INT : 1 = 0;

    /* 29-30 */ u32 : 2;

    /**
     * Interrupt Enable Bit.
     */
    /* 28 */ volatile u32 ENB : 1 = 0;

    /* 26-27 */ u32 : 2;

    /**
     * Vertical Position.
     */
    /* 16-25 */ volatile u32 VCT : 10 = 0;

    /* 10-15 */ u32 : 6;

    /**
     * Horizontal Position.
     */
    /* 0-9 */ volatile u32 HCT : 10 = 0;
  };

  /**
   * Display Interrupt 0.
   *
   * Address: 0xCC002030
   * Size: u32
   */
  Register<Di> DI0;

  /**
   * Display Interrupt 1.
   *
   * Address: 0xCC002034
   * Size: u32
   */
  Register<Di> DI1;

  /**
   * Display Interrupt 2.
   *
   * Address: 0xCC002038
   * Size: u32
   */
  Register<Di> DI2;

  /**
   * Display Interrupt 3.
   *
   * Address: 0xCC00203C
   * Size: u32
   */
  Register<Di> DI3;

  struct Dl {
    using Size = u32;

    /**
     * Horizontal Count.
     */
    /* 0-10 */ volatile u32 HCT : 11 = 0;

    /* 11-15 */ u32 : 5;

    /**
     * Vertical Count.
     */
    /* 16-26 */ volatile u32 VCT : 11 = 0;

    /* 27-30 */ u32 : 4;

    /**
     * Trigger Flag.
     */
    /* 31 */ volatile u32 TRG : 1 = 0;
  };

  /**
   * Display Latch Register 0.
   *
   * Address: 0xCC002040
   * Size: u32
   */
  Register<Dl> DL0;

  /**
   * Display Latch Register 1.
   *
   * Address: 0xCC002044
   * Size: u32
   */
  Register<Dl> DL1;

  struct Hsw {
    using Size = u16;

    /* 15 */ u16 : 1;

    /* 8-14 */ volatile u16 WPL : 7 = 0;

    /* 0-7 */ volatile u16 STD : 8 = 0;
  };

  /**
   * Scaling Width Register.
   *
   * Address: 0xCC002048
   * Size: u16
   */
  Register<Hsw> HSW;

  struct Hsr {
    using Size = u16;

    /* 13-15 */ u16 : 3;

    /**
     * Enable Horizontal Scaling.
     */
    /* 12 */ volatile u16 HS_EN : 1 = 0;

    /* 9-11 */ u16 : 3;

    /**
     * Horizontal stepping size (U1.8 Scaler Value).
     */
    /* 0-8 */ volatile u16 STP : 9 = 0;
  };

  /**
   * Horizontal Scaling Register.
   *
   * Address: 0xCC00204A
   * Size: u16
   */
  Register<Hsr> HSR;

  struct Fct0 {
    using Size = u32;

    /* 30-31 */ u32 : 2;

    /**
     * Tap2.
     */
    /* 20-29 */ volatile u32 T2 : 10 = 0;

    /**
     * Tap1.
     */
    /* 10-19 */ volatile u32 T1 : 10 = 0;

    /**
     * Tap0.
     */
    /* 0-9 */ volatile u32 T0 : 10 = 0;
  };

  /**
   * Filter Coefficient Table 0 (AA stuff).
   *
   * Address: 0xCC00204C
   * Size: u32
   */
  Register<Fct0> FCT0;

  struct Fct1 {
    using Size = u32;

    /* 30-31 */ u32 : 2;

    /**
     * Tap5.
     */
    /* 20-29 */ volatile u32 T5 : 10 = 0;

    /**
     * Tap4.
     */
    /* 10-19 */ volatile u32 T4 : 10 = 0;

    /**
     * Tap3.
     */
    /* 0-9 */ volatile u32 T3 : 10 = 0;
  };

  /**
   * Filter Coefficient Table 1 (AA stuff).
   *
   * Address: 0xCC002050
   * Size: u32
   */
  Register<Fct1> FCT1;

  struct Fct2 {
    using Size = u32;

    /* 30-31 */ u32 : 2;

    /**
     * Tap8.
     */
    /* 20-29 */ volatile u32 T8 : 10 = 0;

    /**
     * Tap7.
     */
    /* 10-19 */ volatile u32 T7 : 10 = 0;

    /**
     * Tap6.
     */
    /* 0-9 */ volatile u32 T6 : 10 = 0;
  };

  /**
   * Filter Coefficient Table 2 (AA stuff).
   *
   * Address: 0xCC002054
   * Size: u32
   */
  Register<Fct2> FCT2;

  struct Fct3 {
    using Size = u32;

    /**
     * Tap12.
     */
    /* 24-31 */ volatile u32 T12 : 8 = 0;

    /**
     * Tap11.
     */
    /* 16-23 */ volatile u32 T11 : 8 = 0;

    /**
     * Tap10.
     */
    /* 8-15 */ volatile u32 T10 : 8 = 0;

    /**
     * Tap9.
     */
    /* 0-7 */ volatile u32 T9 : 8 = 0;
  };

  /**
   * Filter Coefficient Table 3 (AA stuff).
   *
   * Address: 0xCC002058
   * Size: u32
   */
  Register<Fct3> FCT3;

  struct Fct4 {
    using Size = u32;

    /**
     * Tap16.
     */
    /* 24-31 */ volatile u32 T16 : 8 = 0;

    /**
     * Tap15.
     */
    /* 16-23 */ volatile u32 T15 : 8 = 0;

    /**
     * Tap14.
     */
    /* 8-15 */ volatile u32 T14 : 8 = 0;

    /**
     * Tap13.
     */
    /* 0-7 */ volatile u32 T13 : 8 = 0;
  };

  /**
   * Filter Coefficient Table 4 (AA stuff).
   *
   * Address: 0xCC00205C
   * Size: u32
   */
  Register<Fct4> FCT4;

  struct Fct5 {
    using Size = u32;

    /**
     * Tap20.
     */
    /* 24-31 */ volatile u32 T20 : 8 = 0;

    /**
     * Tap19.
     */
    /* 16-23 */ volatile u32 T19 : 8 = 0;

    /**
     * Tap18.
     */
    /* 8-15 */ volatile u32 T18 : 8 = 0;

    /**
     * Tap17.
     */
    /* 0-7 */ volatile u32 T17 : 8 = 0;
  };

  /**
   * Filter Coefficient Table 5 (AA stuff).
   *
   * Address: 0xCC002060
   * Size: u32
   */
  Register<Fct5> FCT5;

  struct Fct6 {
    using Size = u32;

    /**
     * Hardwired to zero.
     */
    /* 24-31 */ u32 : 8;

    /**
     * Tap23.
     */
    /* 16-23 */ volatile u32 T23 : 8 = 0;

    /**
     * Tap22.
     */
    /* 8-15 */ volatile u32 T22 : 8 = 0;

    /**
     * Tap21.
     */
    /* 0-7 */ volatile u32 T21 : 8 = 0;
  };

  /**
   * Filter Coefficient Table 6 (AA stuff).
   *
   * Address: 0xCC002064
   * Size: u32
   */
  Register<Fct6> FCT6;

  /**
   * Unknown register (AA stuff).
   *
   * Address: 0xCC002068
   * Size: u32
   */
  u32 UNK0x68;

  struct ViClk {
    using Size = u16;

    /* 1-15 */ u16 : 15;

    enum Clk {
      /**
       * 27 MHz video clock.
       */
      CLK_27MHZ = 0,

      /**
       * 54 MHz video clock (used in Progressive Mode).
       */
      CLK_54MHZ = 1,
    };

    /**
     * Video clock select.
     */
    /* 0 */ volatile u16 CLK : 1 = 0;
  };

  /**
   * VI Clock Select Register.
   *
   * Address: 0xCC00206C
   * Size: u16
   */
  Register<ViClk> VICLK;

  struct ViSel {
    using Size = u16;

    /* 2-15 */ u16 : 14;

    /* 0-1 */ volatile u16 VISEL : 2 = 0;
  };

  /**
   * VI DTV Status Register.
   *
   * Address: 0xCC00206E
   * Size: u16
   */
  Register<ViSel> VISEL;

  /**
   * Unknown register.
   *
   * Address: 0xCC002070
   * Size: u16
   */
  u16 UNK0x70;

  struct Hbe {
    using Size = u16;

    /**
     * Border Enable.
     */
    /* 15 */ volatile u16 BRDR_EN : 1 = 0;

    /* 10-14 */ u16 : 5;

    /**
     * Border Horizontal Blank End.
     */
    /* 0-9 */ volatile u16 HBE656 : 10 = 0;
  };

  /**
   * Border HBE.
   *
   * Address: 0xCC002072
   * Size: u16
   */
  Register<Hbe> HBE;

  struct Hbs {
    using Size = u16;

    /* 10-15 */ u16 : 6;

    /**
     * Border Horizontal Blank Start.
     */
    /* 0-9 */ volatile u16 HBS656 : 10 = 0;
  };

  /**
   * Border HBS.
   *
   * Address: 0xCC002074
   * Size: u16
   */
  Register<Hbs> HBS;

  /**
   * Pad to 0x80 size block. The values are always 0x00FF.
   */
  u16 PAD[5];
} *const VI = reinterpret_cast<VideoInterface *>(0xCC002000);

static_assert(sizeof(VideoInterface) == 0x80);

} // namespace peli::hw