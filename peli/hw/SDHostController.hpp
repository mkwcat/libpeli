// peli/hw/SDHostController.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.hpp"
#include "Namespace.hpp"
#include "Register.hpp"

namespace peli::hw {

struct SDHostController {
  /**
   * SDMA System Address.
   *
   * Address: 0x0D070000
   * Size: u32
   */
  volatile u32 SDMAADR;

  /**
   * Block Size Register.
   *
   * Address: 0x0D070004
   * Size: u16
   */
  volatile u16 BSR;

  /**
   * 16-bit Block Count Register.
   *
   * Address: 0x0D070006
   * Size: u16
   */
  volatile u16 BCR;

  /**
   * Argument Register.
   *
   * Address: 0x0D070008
   * Size: u32
   */
  volatile u32 AR;

  /**
   * Transfer Mode Register.
   * Address: 0x0D07000C
   * Size: u16
   */
  volatile u16 TMR;

  /**
   * Command Register.
   *
   * Address: 0x0D07000E
   * Size: u16
   */
  volatile u16 CR;

  /**
   * Response Register.
   *
   * Address: 0x0D070010
   * Size: u16[8]
   */
  volatile u16 RR[8];

  /**
   * Buffer Data Port Register.
   *
   * Address: 0x0D070020
   * Size: u32
   */
  volatile u32 BDPR;

  /**
   * Present State Register.
   *
   * Address: 0x0D070024
   * Size: u32
   */
  volatile u32 PSR;

  struct Hc1r {
    using Size = u8;

    /**
     * Card Detect Signal Selection.
     */
    /* 7 */ volatile u8 SDSS : 1 = 0;

    /**
     * Card Detect Test Level.
     */
    /* 6 */ volatile u8 CDTL : 1 = 0;

    /**
     * Extended Data Transfer Width (Embedded and SD Mode only).
     */
    /* 5 */ volatile u8 EDTW : 1 = 0;

    enum : u8 {
      DMA_SDMA = 0,
      DMA_RESV = 1,
      DMA_32ADMA2 = 2,
    };

    /**
     * DMA Select.
     */
    /* 4 */ volatile u8 DMA : 2 = 0;

    /**
     * High Speed Enable (SD Mode only).
     */
    /* 2 */ volatile u8 HSE : 1 = 0;

    enum : u8 {
      DTW_1BIT = 0,
      DTW_4BIT = 1,
    };

    /**
     * Data Transfer Width (SD Mode only).
     */
    /* 1 */ volatile u8 DTW : 1 = 0;

    /**
     * LED Control,
     */
    /* 0 */ volatile u8 LED : 1 = 0;
  };

  /**
   * Host Control 1 Register.
   *
   * Address: 0x0D070028
   * Size: u8
   */
  volatile Register<Hc1r> HC1R;

  /**
   * Power Control Register.
   *
   * Address: 0x0D070029
   * Size: u8
   */
  volatile u8 PCR;

  /**
   * Block Gap Control Register.
   *
   * Address: 0x0D07002A
   * Size: u8
   */
  volatile u8 BGCR;

  /**
   * Wakeup Control Register.
   *
   * Address: 0x0D07002B
   * Size: u8
   */
  volatile u8 WCR;

  /**
   * Clock Control Register.
   *
   * Address: 0x0D07002C
   * Size: u16
   */
  volatile u16 CCR;

  /**
   * Timeout Control Register.
   *
   * Address: 0x0D07002E
   * Size: u8
   */
  volatile u8 TCR;

  /**
   * Software Reset Register.
   *
   * Address: 0x0D07002F
   * Size: u8
   */
  volatile u8 SRR;

  /**
   * Normal Interrupt Status Register.
   *
   * Address: 0x0D070030
   * Size: u16
   */
  volatile u16 NISR;

  /**
   * Error Interrupt Status Register.
   *
   * Address: 0x0D070032
   * Size: u16
   */
  volatile u16 EISR;

  /**
   * Normal Interrupt Status Enable Register.
   *
   * Address: 0x0D070034
   * Size: u16
   */
  volatile u16 NISENR;

  /**
   * Error Interrupt Status Enable Register.
   *
   * Address: 0x0D070036
   * Size: u16
   */
  volatile u16 EISENR;

  /**
   * Normal Interrupt Signal Enable Register.
   *
   * Address: 0x0D070038
   * Size: u16
   */
  volatile u16 NISIGENR;

  /**
   * Error Interrupt Signal Enable Register.
   *
   * Address: 0x0D07003A
   * Size: u16
   */
  volatile u16 EISIGENR;

  /**
   * Auto CMD Error Status Register.
   *
   * Address: 0x0D07003C
   * Size: u16
   */
  volatile u16 ACMDESR;

  /**
   * Host Control 2 Register.
   *
   * Address: 0x0D07003E
   * Size: u16
   */
  volatile u16 HC2R;

  /**
   * Capabilities Register.
   *
   * Address: 0x0D070040
   * Size: u16[4]
   */
  volatile u16 CAR[4];
};

namespace ppc {
inline SDHostController *const SDHC0 =
    reinterpret_cast<SDHostController *>(0xCD070000);
}
namespace iop {
inline SDHostController *const SDHC0 =
    reinterpret_cast<SDHostController *>(0xCD870000);
}
using PELI_HW_HOST_NAME::SDHC0;

} // namespace peli::hw
