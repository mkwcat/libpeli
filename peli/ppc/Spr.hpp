// peli/ppc/Spr.hpp - Special Purpose Registers
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.h"

namespace peli::ppc {

enum class Spr {
  /**
   * Zero Extend Register.
   */
  XER = 1,

  /**
   * Link Register.
   */
  LR = 8,

  /**
   * Count Register.
   */
  CTR = 9,

  /**
   * Data Storage Interrupt Status Register.
   */
  DSISR = 18,

  /**
   * Data Address Register.
   */
  DAR = 19,

  /**
   * Decrementer Register.
   */
  DEC = 22,

  /**
   * SDR1.
   */
  SDR1 = 25,

  /**
   * Save and Restore Register 0.
   * When an exception occurs, the address of the instruction that caused the
   * exception, or the address of the next instruction to be executed, is saved
   * in this register.
   */
  SRR0 = 26,

  /**
   * Save and Restore Register 1.
   * When an exception occurs, the previous value of the MSR is saved in this
   * register.
   */
  SRR1 = 27,

  /**
   * Time Base Lower and Upper registers.
   */
  TBL = 268,
  TBU = 269,

  /**
   * Time Base Lower and Upper Write registers.
   */
  TBLW = 284,
  TBUW = 285,

  /**
   * Processor Version Register.
   */
  PVR = 287,

  /**
   * Instruction Block Address Translation registers.
   */
  IBAT0U = 528,
  IBAT0L = 529,
  IBAT1U = 530,
  IBAT1L = 531,
  IBAT2U = 532,
  IBAT2L = 533,
  IBAT3U = 534,
  IBAT3L = 535,

  /**
   * Data Block Address Translation registers.
   */
  DBAT0U = 536,
  DBAT0L = 537,
  DBAT1U = 538,
  DBAT1L = 539,
  DBAT2U = 540,
  DBAT2L = 541,
  DBAT3U = 542,
  DBAT3L = 543,

  /**
   * Instruction Block Address Translation registers (extended).
   * Only available if SBE (Secondary BAT Enable) is set in HID4.
   */
  IBAT4U = 560,
  IBAT4L = 561,
  IBAT5U = 562,
  IBAT5L = 563,
  IBAT6U = 564,
  IBAT6L = 565,
  IBAT7U = 566,
  IBAT7L = 567,

  /**
   * Data Block Address Translation registers (extended).
   * Only available if SBE (Secondary BAT Enable) is set in HID4.
   */
  DBAT4U = 568,
  DBAT4L = 569,
  DBAT5U = 570,
  DBAT5L = 571,
  DBAT6U = 572,
  DBAT6L = 573,
  DBAT7U = 574,
  DBAT7L = 575,

  /**
   * Graphics Quantization Registers.
   */
  GQR0 = 912,
  GQR1 = 913,
  GQR2 = 914,
  GQR3 = 915,
  GQR4 = 916,
  GQR5 = 917,
  GQR6 = 918,
  GQR7 = 919,

  /**
   * Hardware Implementation-Dependent 0.
   */
  HID2 = 920,

  /**
   * Write Gather Pipe Address Register.
   */
  WPAR = 921,

  /**
   * Direct Memory Access Upper register.
   */
  DMAU = 922,

  /**
   * Direct Memory Access Lower register.
   */
  DMAL = 923,

  /**
   * User Monitor Mode Control Register 0.
   */
  UMMCR0 = 936,

  /**
   * User Performance Monitor Counter registers.
   */
  UPMC1 = 937,
  UPMC2 = 938,
  UPMC3 = 941,
  UPMC4 = 942,

  /**
   * User Sampled Instruction Address.
   */
  USIA = 939,

  /**
   * User Monitor Mode Control Register 1.
   */
  UMMCR1 = 940,

  /**
   * Hardware Implementation-Dependent 5 (Espresso only).
   */
  HID5 = 944,

  /**
   * BCR (Espresso only).
   */
  BCR = 949,

  /**
   * Monitor Mode Control Register 0.
   */
  MMCR0 = 952,

  /**
   * Performance Monitor Counter registers.
   */
  PMC1 = 953,
  PMC2 = 954,
  PMC3 = 957,
  PMC4 = 958,

  /**
   * Monitor Mode Control Register 1.
   */
  MMCR1 = 956,

  /**
   * Sampled Instruction Address.
   */
  SIA = 955,

  /**
   * Hardware Implementation-Dependent 0.
   */
  HID0 = 1008,

  /**
   * Hardware Implementation-Dependent 1.
   */
  HID1 = 1009,

  /**
   * Instruction Address Breakpoint Register.
   */
  IABR = 1010,

  /**
   * Hardware Implementation-Dependent 4.
   */
  HID4 = 1011,

  /**
   * Thermal Diode Calibration Lower.
   */
  TDCL = 1012,

  /**
   * L2 Control Register.
   */
  L2CR = 1017,

  /**
   * Thermal Diode Calibration Upper.
   */
  TDCU = 1018,

  /**
   * Instruction Cache Throttling Control Register.
   */
  ICTC = 1019,

  /**
   * Thermal assist unit registers.
   */
  THRM1 = 1020,
  THRM2 = 1021,
  THRM3 = 1022,

  // The following SPRs are not actual special-purpose registers, but are rather
  // manually checked for by this library.

  /**
   * Machine State Register. Not SPR.
   */
  MSR = 0x8000,

  /**
   * Condition Register. Not SPR.
   */
  CR,

  /**
   * Floating Point Status and Control Register. Not SPR.
   */
  FPSCR,

  /**
   * 16 Segment Registers. Not SPR.
   */
  SR0,
  SR1,
  SR2,
  SR3,
  SR4,
  SR5,
  SR6,
  SR7,
  SR8,
  SR9,
  SR10,
  SR11,
  SR12,
  SR13,
  SR14,
  SR15,
};

template <Spr Reg> static inline u32 SetSpr(u32 value) noexcept {
  if constexpr (Reg == Spr::MSR) {
    __asm__ __volatile__("mtmsr %0;" : : "r"(value));
    return value;
  } else {
    __asm__ __volatile__("mtspr %0, %1;"
                         :
                         : "i"(static_cast<u32>(Reg)), "r"(value));
    return value;
  }
}

template <Spr Reg> static inline u32 GetSpr() noexcept {
  u32 value;
  if (Reg == Spr::MSR) {
    __asm__ __volatile__("mfmsr %0;" : "=r"(value));
    return value;
  } else {
    __asm__ __volatile__("mfspr %0, %1;"
                         : "=r"(value)
                         : "i"(static_cast<u32>(Reg)));
  }
  return value;
}

template <Spr Reg> static inline u32 ClearSprBit(u32 value) {
  return SetSpr<Reg>(GetSpr<Reg>() & ~value);
}

template <Spr Reg> static inline u32 SetSprBit(u32 value) {
  return SetSpr<Reg>(GetSpr<Reg>() | value);
}

} // namespace peli::ppc