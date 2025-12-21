// peli/ppc/Spr.hpp - Special Purpose Registers
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.hpp"

namespace peli::ppc {

#define PELI_SPR_XER 1
#define PELI_SPR_LR 8
#define PELI_SPR_CTR 9
#define PELI_SPR_DSISR 18
#define PELI_SPR_DAR 19
#define PELI_SPR_DEC 22
#define PELI_SPR_SDR1 25
#define PELI_SPR_SRR0 26
#define PELI_SPR_SRR1 27
#define PELI_SPR_TBL 268
#define PELI_SPR_TBU 269
#define PELI_SPR_EAR 282
#define PELI_SPR_TBLW 284
#define PELI_SPR_TBUW 285
#define PELI_SPR_PVR 287
#define PELI_SPR_IBAT0U 528
#define PELI_SPR_IBAT0L 529
#define PELI_SPR_IBAT1U 530
#define PELI_SPR_IBAT1L 531
#define PELI_SPR_IBAT2U 532
#define PELI_SPR_IBAT2L 533
#define PELI_SPR_IBAT3U 534
#define PELI_SPR_IBAT3L 535
#define PELI_SPR_DBAT0U 536
#define PELI_SPR_DBAT0L 537
#define PELI_SPR_DBAT1U 538
#define PELI_SPR_DBAT1L 539
#define PELI_SPR_DBAT2U 540
#define PELI_SPR_DBAT2L 541
#define PELI_SPR_DBAT3U 542
#define PELI_SPR_DBAT3L 543
#define PELI_SPR_IBAT4U 560
#define PELI_SPR_IBAT4L 561
#define PELI_SPR_IBAT5U 562
#define PELI_SPR_IBAT5L 563
#define PELI_SPR_IBAT6U 564
#define PELI_SPR_IBAT6L 565
#define PELI_SPR_IBAT7U 566
#define PELI_SPR_IBAT7L 567
#define PELI_SPR_DBAT4U 568
#define PELI_SPR_DBAT4L 569
#define PELI_SPR_DBAT5U 570
#define PELI_SPR_DBAT5L 571
#define PELI_SPR_DBAT6U 572
#define PELI_SPR_DBAT6L 573
#define PELI_SPR_DBAT7U 574
#define PELI_SPR_DBAT7L 575
#define PELI_SPR_GQR0 912
#define PELI_SPR_GQR1 913
#define PELI_SPR_GQR2 914
#define PELI_SPR_GQR3 915
#define PELI_SPR_GQR4 916
#define PELI_SPR_GQR5 917
#define PELI_SPR_GQR6 918
#define PELI_SPR_GQR7 919
#define PELI_SPR_HID2 920
#define PELI_SPR_WPAR 921
#define PELI_SPR_DMAU 922
#define PELI_SPR_DMAL 923
#define PELI_SPR_UMMCR0 936
#define PELI_SPR_UPMC1 937
#define PELI_SPR_UPMC2 938
#define PELI_SPR_USIA 939
#define PELI_SPR_UMMCR1 940
#define PELI_SPR_UPMC3 941
#define PELI_SPR_UPMC4 942
#define PELI_SPR_HID5 944
#define PELI_SPR_BCR 949
#define PELI_SPR_MMCR0 952
#define PELI_SPR_PMC1 953
#define PELI_SPR_PMC2 954
#define PELI_SPR_SIA 955
#define PELI_SPR_MMCR1 956
#define PELI_SPR_PMC3 957
#define PELI_SPR_PMC4 958
#define PELI_SPR_HID0 1008
#define PELI_SPR_HID1 1009
#define PELI_SPR_IABR 1010
#define PELI_SPR_HID4 1011
#define PELI_SPR_TDCL 1012
#define PELI_SPR_DABR 1013
#define PELI_SPR_L2CR 1017
#define PELI_SPR_TDCU 1018
#define PELI_SPR_ICTC 1019
#define PELI_SPR_THRM1 1020
#define PELI_SPR_THRM2 1021
#define PELI_SPR_THRM3 1022

enum class Spr {
  /**
   * Zero Extend Register.
   */
  XER = PELI_SPR_XER,

  /**
   * Link Register.
   */
  LR = PELI_SPR_LR,

  /**
   * Count Register.
   */
  CTR = PELI_SPR_CTR,

  /**
   * Data Storage Interrupt Status Register.
   */
  DSISR = PELI_SPR_DSISR,

  /**
   * Data Address Register.
   */
  DAR = PELI_SPR_DAR,

  /**
   * Decrementer Register.
   */
  DEC = PELI_SPR_DEC,

  /**
   * SDR1.
   */
  SDR1 = PELI_SPR_SDR1,

  /**
   * Save and Restore Register 0.
   * When an exception occurs, the address of the instruction that caused the
   * exception, or the address of the next instruction to be executed, is saved
   * in this register.
   */
  SRR0 = PELI_SPR_SRR0,

  /**
   * Save and Restore Register 1.
   * When an exception occurs, the previous value of the MSR is saved in this
   * register.
   */
  SRR1 = PELI_SPR_SRR1,

  /**
   * Time Base Lower and Upper registers.
   */
  TBL = PELI_SPR_TBL,
  TBU = PELI_SPR_TBU,

  /**
   * External Access Register.
   */
  EAR = PELI_SPR_EAR,

  /**
   * Time Base Lower and Upper Write registers.
   */
  TBLW = PELI_SPR_TBLW,
  TBUW = PELI_SPR_TBUW,

  /**
   * Processor Version Register.
   */
  PVR = PELI_SPR_PVR,

  /**
   * Instruction Block Address Translation registers.
   */
  IBAT0U = PELI_SPR_IBAT0U,
  IBAT0L = PELI_SPR_IBAT0L,
  IBAT1U = PELI_SPR_IBAT1U,
  IBAT1L = PELI_SPR_IBAT1L,
  IBAT2U = PELI_SPR_IBAT2U,
  IBAT2L = PELI_SPR_IBAT2L,
  IBAT3U = PELI_SPR_IBAT3U,
  IBAT3L = PELI_SPR_IBAT3L,

  /**
   * Data Block Address Translation registers.
   */
  DBAT0U = PELI_SPR_DBAT0U,
  DBAT0L = PELI_SPR_DBAT0L,
  DBAT1U = PELI_SPR_DBAT1U,
  DBAT1L = PELI_SPR_DBAT1L,
  DBAT2U = PELI_SPR_DBAT2U,
  DBAT2L = PELI_SPR_DBAT2L,
  DBAT3U = PELI_SPR_DBAT3U,
  DBAT3L = PELI_SPR_DBAT3L,

  /**
   * Instruction Block Address Translation registers (extended).
   * Only available if SBE (Secondary BAT Enable) is set in HID4.
   */
  IBAT4U = PELI_SPR_IBAT4U,
  IBAT4L = PELI_SPR_IBAT4L,
  IBAT5U = PELI_SPR_IBAT5U,
  IBAT5L = PELI_SPR_IBAT5L,
  IBAT6U = PELI_SPR_IBAT6U,
  IBAT6L = PELI_SPR_IBAT6L,
  IBAT7U = PELI_SPR_IBAT7U,
  IBAT7L = PELI_SPR_IBAT7L,

  /**
   * Data Block Address Translation registers (extended).
   * Only available if SBE (Secondary BAT Enable) is set in HID4.
   */
  DBAT4U = PELI_SPR_DBAT4U,
  DBAT4L = PELI_SPR_DBAT4L,
  DBAT5U = PELI_SPR_DBAT5U,
  DBAT5L = PELI_SPR_DBAT5L,
  DBAT6U = PELI_SPR_DBAT6U,
  DBAT6L = PELI_SPR_DBAT6L,
  DBAT7U = PELI_SPR_DBAT7U,
  DBAT7L = PELI_SPR_DBAT7L,

  /**
   * Graphics Quantization Registers.
   */
  GQR0 = PELI_SPR_GQR0,
  GQR1 = PELI_SPR_GQR1,
  GQR2 = PELI_SPR_GQR2,
  GQR3 = PELI_SPR_GQR3,
  GQR4 = PELI_SPR_GQR4,
  GQR5 = PELI_SPR_GQR5,
  GQR6 = PELI_SPR_GQR6,
  GQR7 = PELI_SPR_GQR7,

  /**
   * Hardware Implementation-Dependent 0.
   */
  HID2 = PELI_SPR_HID2,

  /**
   * Write Gather Pipe Address Register.
   */
  WPAR = PELI_SPR_WPAR,

  /**
   * Direct Memory Access Upper register.
   */
  DMAU = PELI_SPR_DMAU,

  /**
   * Direct Memory Access Lower register.
   */
  DMAL = PELI_SPR_DMAL,

  /**
   * User Monitor Mode Control Register 0.
   */
  UMMCR0 = PELI_SPR_UMMCR0,

  /**
   * User Performance Monitor Counter registers.
   */
  UPMC1 = PELI_SPR_UPMC1,
  UPMC2 = PELI_SPR_UPMC2,
  UPMC3 = PELI_SPR_UPMC3,
  UPMC4 = PELI_SPR_UPMC4,

  /**
   * User Sampled Instruction Address.
   */
  USIA = PELI_SPR_USIA,

  /**
   * User Monitor Mode Control Register 1.
   */
  UMMCR1 = PELI_SPR_UMMCR1,

  /**
   * Hardware Implementation-Dependent 5 (Espresso only).
   */
  HID5 = PELI_SPR_HID5,

  /**
   * BCR (Espresso only).
   */
  BCR = PELI_SPR_BCR,

  /**
   * Monitor Mode Control Register 0.
   */
  MMCR0 = PELI_SPR_MMCR0,

  /**
   * Performance Monitor Counter registers.
   */
  PMC1 = PELI_SPR_PMC1,
  PMC2 = PELI_SPR_PMC2,
  PMC3 = PELI_SPR_PMC3,
  PMC4 = PELI_SPR_PMC4,

  /**
   * Monitor Mode Control Register 1.
   */
  MMCR1 = PELI_SPR_MMCR1,

  /**
   * Sampled Instruction Address.
   */
  SIA = PELI_SPR_SIA,

  /**
   * Hardware Implementation-Dependent 0.
   */
  HID0 = PELI_SPR_HID0,

  /**
   * Hardware Implementation-Dependent 1.
   */
  HID1 = PELI_SPR_HID1,

  /**
   * Instruction Address Breakpoint Register.
   */
  IABR = PELI_SPR_IABR,

  /**
   * Hardware Implementation-Dependent 4.
   */
  HID4 = PELI_SPR_HID4,

  /**
   * Thermal Diode Calibration Lower.
   */
  TDCL = PELI_SPR_TDCL,

  /**
   * Data Address Breakpoint Register.
   */
  DABR = PELI_SPR_DABR,

  /**
   * L2 Control Register.
   */
  L2CR = PELI_SPR_L2CR,

  /**
   * Thermal Diode Calibration Upper.
   */
  TDCU = PELI_SPR_TDCU,

  /**
   * Instruction Cache Throttling Control Register.
   */
  ICTC = PELI_SPR_ICTC,

  /**
   * Thermal assist unit registers.
   */
  THRM1 = PELI_SPR_THRM1,
  THRM2 = PELI_SPR_THRM2,
  THRM3 = PELI_SPR_THRM3,

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

template <Spr Reg> inline u32 SetSpr(u32 value) noexcept {
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

template <Spr Reg> inline u32 GetSpr() noexcept {
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

template <Spr Reg> inline u32 ClearSprBit(u32 value) {
  return SetSpr<Reg>(GetSpr<Reg>() & ~value);
}

template <Spr Reg> inline u32 SetSprBit(u32 value) {
  return SetSpr<Reg>(GetSpr<Reg>() | value);
}

} // namespace peli::ppc