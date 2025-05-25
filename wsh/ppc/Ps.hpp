// wsh/ppc/Ps.hpp - Paired Single
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.h"
#include "../util/Optimize.hpp"
#include "Hid2.hpp"
#include "SprRwCtl.hpp"

namespace wsh::ppc::Ps {

template <u32 GQR>
  requires(GQR < 8)
static inline void SetGQR(u32 value) noexcept {
  constexpr u32 gqr = static_cast<u32>(Spr::GQR0) + GQR;
  SetSpr<static_cast<Spr>(gqr)>(value);
}

template <u32 GQR>
  requires(GQR < 8)
static inline u32 GetGQR() noexcept {
  constexpr u32 gqr = static_cast<u32>(Spr::GQR0) + GQR;
  return GetSpr<static_cast<Spr>(gqr)>();
}

static inline void SetGQR(u32 gqr, u32 value) noexcept {
  switch (gqr) {
  case 0:
    SetSpr<Spr::GQR0>(value);
    break;
  case 1:
    SetSpr<Spr::GQR1>(value);
    break;
  case 2:
    SetSpr<Spr::GQR2>(value);
    break;
  case 3:
    SetSpr<Spr::GQR3>(value);
    break;
  case 4:
    SetSpr<Spr::GQR4>(value);
    break;
  case 5:
    SetSpr<Spr::GQR5>(value);
    break;
  case 6:
    SetSpr<Spr::GQR6>(value);
    break;
  case 7:
    SetSpr<Spr::GQR7>(value);
    break;

  default:
    _WSH_UNREACHABLE();
  }
}

static inline u32 GetGQR(u32 gqr) noexcept {
  switch (gqr) {
  case 0:
    return GetSpr<Spr::GQR0>();
  case 1:
    return GetSpr<Spr::GQR1>();
  case 2:
    return GetSpr<Spr::GQR2>();
  case 3:
    return GetSpr<Spr::GQR3>();
  case 4:
    return GetSpr<Spr::GQR4>();
  case 5:
    return GetSpr<Spr::GQR5>();
  case 6:
    return GetSpr<Spr::GQR6>();
  case 7:
    return GetSpr<Spr::GQR7>();

  default:
    _WSH_UNREACHABLE();
    return 0;
  }
}

static inline void ResetGQRs() noexcept {
  // Skip GQR0 as it's always assumed to be 0
  SetSpr<Spr::GQR1>(0);
  SetSpr<Spr::GQR2>(0);
  SetSpr<Spr::GQR3>(0);
  SetSpr<Spr::GQR4>(0);
  SetSpr<Spr::GQR5>(0);
  SetSpr<Spr::GQR6>(0);
  SetSpr<Spr::GQR7>(0);
}

static inline void Init() noexcept {
  // Set HID2 |= 0xA0000000
  SprRwCtl<Hid2>() |= Hid2Bits{
      .LSQE = 1, // Load/Store Quantized Enable
      .PSE = 1,  // Paired Single Enable
  };

  ResetGQRs();
}

static inline void Disable() noexcept {
  // Set HID2 &= ~0xA0000000
  SprRwCtl<Hid2>().Clear(Hid2Bits{
      .LSQE = 1, // Load/Store Quantized Disable
      .PSE = 1,  // Paired Single Disable
  });
}

} // namespace wsh::ppc::Ps