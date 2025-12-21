// peli/ppc/Gpr.hpp - General Purpose Registers
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.hpp"

namespace peli::ppc {

template <int Reg> u32 GetGpr() noexcept {
  static_assert(Reg >= 0 && Reg <= 31,
                "Invalid GPR (General Purpose Register) number");

  // I just created the coolest function
  if constexpr (Reg == 0) {
    register u32 reg asm("%r0");
    return reg;
  } else if constexpr (Reg == 1) {
    register u32 reg asm("%r1");
    return reg;
  } else if constexpr (Reg == 2) {
    register u32 reg asm("%r2");
    return reg;
  } else if constexpr (Reg == 3) {
    register u32 reg asm("%r3");
    return reg;
  } else if constexpr (Reg == 4) {
    register u32 reg asm("%r4");
    return reg;
  } else if constexpr (Reg == 5) {
    register u32 reg asm("%r5");
    return reg;
  } else if constexpr (Reg == 6) {
    register u32 reg asm("%r6");
    return reg;
  } else if constexpr (Reg == 7) {
    register u32 reg asm("%r7");
    return reg;
  } else if constexpr (Reg == 8) {
    register u32 reg asm("%r8");
    return reg;
  } else if constexpr (Reg == 9) {
    register u32 reg asm("%r9");
    return reg;
  } else if constexpr (Reg == 10) {
    register u32 reg asm("%r10");
    return reg;
  } else if constexpr (Reg == 11) {
    register u32 reg asm("%r11");
    return reg;
  } else if constexpr (Reg == 12) {
    register u32 reg asm("%r12");
    return reg;
  } else if constexpr (Reg == 13) {
    register u32 reg asm("%r13");
    return reg;
  } else if constexpr (Reg == 14) {
    register u32 reg asm("%r14");
    return reg;
  } else if constexpr (Reg == 15) {
    register u32 reg asm("%r15");
    return reg;
  } else if constexpr (Reg == 16) {
    register u32 reg asm("%r16");
    return reg;
  } else if constexpr (Reg == 17) {
    register u32 reg asm("%r17");
    return reg;
  } else if constexpr (Reg == 18) {
    register u32 reg asm("%r18");
    return reg;
  } else if constexpr (Reg == 19) {
    register u32 reg asm("%r19");
    return reg;
  } else if constexpr (Reg == 20) {
    register u32 reg asm("%r20");
    return reg;
  } else if constexpr (Reg == 21) {
    register u32 reg asm("%r21");
    return reg;
  } else if constexpr (Reg == 22) {
    register u32 reg asm("%r22");
    return reg;
  } else if constexpr (Reg == 23) {
    register u32 reg asm("%r23");
    return reg;
  } else if constexpr (Reg == 24) {
    register u32 reg asm("%r24");
    return reg;
  } else if constexpr (Reg == 25) {
    register u32 reg asm("%r25");
    return reg;
  } else if constexpr (Reg == 26) {
    register u32 reg asm("%r26");
    return reg;
  } else if constexpr (Reg == 27) {
    register u32 reg asm("%r27");
    return reg;
  } else if constexpr (Reg == 28) {
    register u32 reg asm("%r28");
    return reg;
  } else if constexpr (Reg == 29) {
    register u32 reg asm("%r29");
    return reg;
  } else if constexpr (Reg == 30) {
    register u32 reg asm("%r30");
    return reg;
  } else if constexpr (Reg == 31) {
    register u32 reg asm("%r31");
    return reg;
  }
}

} // namespace peli::ppc