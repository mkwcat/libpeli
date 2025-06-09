// wsh/ppc/Msr.hpp - Machine State Register
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.h"
#include "detail/SprInterface.hpp"

namespace wsh::ppc {

struct MsrBits {
  static constexpr Spr SPR = Spr::MSR;

  /* 0-12 */ u32 RESV0 : 13 = 0;
  /* 13 */ u32 POW : 1 = 0;
  /* 14 */ u32 RESV14 : 1 = 0;
  /* 15 */ u32 ILE : 1 = 0;
  /* 16 */ u32 EE : 1 = 0;
  /* 17 */ u32 PR : 1 = 0;
  /* 18 */ u32 FP : 1 = 0;
  /* 19 */ u32 ME : 1 = 0;
  /* 20 */ u32 FE0 : 1 = 0;
  /* 21 */ u32 SE : 1 = 0;
  /* 22 */ u32 BE : 1 = 0;
  /* 23 */ u32 FE1 : 1 = 0;
  /* 24 */ u32 RESV24 : 1 = 0;
  /* 25 */ u32 IP : 1 = 0;
  /* 26 */ u32 IR : 1 = 0;
  /* 27 */ u32 DR : 1 = 0;
  /* 28 */ u32 RESV28 : 1 = 0;
  /* 29 */ u32 PM : 1 = 0;
  /* 30 */ u32 RI : 1 = 0;
  /* 31 */ u32 LE : 1 = 0;
};

struct Msr : public detail::SprInterface<MsrBits> {
  Msr() noexcept : detail::SprInterface<MsrBits>() {}
  Msr(u32 value) noexcept : detail::SprInterface<MsrBits>(value) {}
  Msr(const Msr &rhs) noexcept : detail::SprInterface<MsrBits>(rhs) {}
  Msr(const MsrBits &rhs) noexcept : detail::SprInterface<MsrBits>(rhs) {}

  static u32 EnterRealMode() noexcept;
  static void ExitRealMode(u32 prevMsr) noexcept;

  static bool EnableInterrupts() noexcept {
    // Done in ASM to ensure no floating point operations are performed between
    // reading and writing to the MSR
    u32 tmp;
    bool flag;
    __asm__ __volatile__("mfmsr %0;"
                         "rlwinm. %1, %0, 32-15, 1;"
                         "bne +12;"
                         "ori %0, %0, 0x8000;"
                         "mtmsr %0"
                         : "=r"(tmp), "=r"(flag)
                         :
                         : "cr0");
    return flag;
  }

  static bool DisableInterrupts() noexcept {
    if (!MoveFrom().EE) {
      return false;
    }
    // Call DisableInterrupts syscall to ensure atomic write to MSR
    __asm__ __volatile__("crclr 28; sc" ::: "cr7", "r0");
    return true;
  }

  static void SetInterrupts(bool enable) noexcept {
    if (enable) {
      EnableInterrupts();
    } else {
      DisableInterrupts();
    }
  }

  class RealModeScope {
  public:
    RealModeScope() noexcept : msr(EnterRealMode()) {}

    ~RealModeScope() noexcept { ExitRealMode(msr.Hex()); }

    detail::SprInterface<MsrBits> msr;
  };

  class OverrideScope {
  public:
    OverrideScope(Msr msrOvr) noexcept {
      msr = MoveFrom();
      msrOvr.MoveTo();
    }

    ~OverrideScope() noexcept { msr.MoveTo(); }

    detail::SprInterface<MsrBits> msr;
  };

  class NoInterruptsScope {
  public:
    NoInterruptsScope() noexcept { prev = DisableInterrupts(); }

    ~NoInterruptsScope() noexcept {
      if (prev) {
        EnableInterrupts();
      }
    }

    bool prev;
  };

  class EnableInterruptsScope {
  public:
    EnableInterruptsScope() noexcept { prev = EnableInterrupts(); }

    ~EnableInterruptsScope() noexcept {
      if (!prev) {
        DisableInterrupts();
      }
    }

    bool prev;
  };
};

} // namespace wsh::ppc