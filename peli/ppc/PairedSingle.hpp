// peli/ppc/PairedSingle.hpp - Paired Single
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.hpp"
#include "../util/Optimize.hpp"
#include "Hid2.hpp"
#include "SprRwCtl.hpp"
#include "peli/ppc/Spr.hpp"

namespace peli::ppc::PairedSingle {

/**
 * Sets the Graphics Quantization Register in the template argument to the
 * specified value.
 */
template <unsigned GQR>
  requires(GQR < 8)
inline void SetGQR(u32 value) noexcept {
  constexpr u32 gqr = PELI_SPR_GQR0 + GQR;
  [[assume(gqr >= PELI_SPR_GQR0 && gqr <= PELI_SPR_GQR7)]];
  MoveTo<static_cast<Spr>(gqr)>(value);
}

/**
 * Gets the value from the Graphics Quantization Register specified in the
 * template argument.
 */
template <u32 GQR>
  requires(GQR < 8)
inline u32 GetGQR() noexcept {
  constexpr u32 gqr = PELI_SPR_GQR0 + GQR;
  return MoveFrom<static_cast<Spr>(gqr)>();
}

/**
 * Sets the specified Graphics Quantization Register to the specified value. Use
 * this instead of the template version only if runtime evaluation is needed for
 * which register to write to.
 * @param gqr The number specifying the Graphics Quantization Register to write
 * to. A value not in the 0-7 range will lead to undefined behavior.
 */
inline void SetGQR(u32 gqr, u32 value) noexcept {
  switch (gqr) {
  case 0:
    MoveTo<Spr::GQR0>(value);
    break;
  case 1:
    MoveTo<Spr::GQR1>(value);
    break;
  case 2:
    MoveTo<Spr::GQR2>(value);
    break;
  case 3:
    MoveTo<Spr::GQR3>(value);
    break;
  case 4:
    MoveTo<Spr::GQR4>(value);
    break;
  case 5:
    MoveTo<Spr::GQR5>(value);
    break;
  case 6:
    MoveTo<Spr::GQR6>(value);
    break;
  case 7:
    MoveTo<Spr::GQR7>(value);
    break;

  default:
    _PELI_UNREACHABLE();
  }
}

/**
 * Gets the value from the specified Graphics Quantization Register. Use this
 * instead of the template version only if runtime evaluation is needed for
 * which register to write to.
 * @param gqr The number specifying the Graphics Quantization Register to read
 * from. A value not in the 0-7 range will lead to undefined behavior.
 */
inline u32 GetGQR(u32 gqr) noexcept {
  switch (gqr) {
  case 0:
    return MoveFrom<Spr::GQR0>();
  case 1:
    return MoveFrom<Spr::GQR1>();
  case 2:
    return MoveFrom<Spr::GQR2>();
  case 3:
    return MoveFrom<Spr::GQR3>();
  case 4:
    return MoveFrom<Spr::GQR4>();
  case 5:
    return MoveFrom<Spr::GQR5>();
  case 6:
    return MoveFrom<Spr::GQR6>();
  case 7:
    return MoveFrom<Spr::GQR7>();

  default:
    _PELI_UNREACHABLE();
    return 0;
  }
}

inline void ResetGQRs() noexcept {
  // Skip GQR0 as it's always assumed to be 0
  MoveTo<Spr::GQR1>(0);
  MoveTo<Spr::GQR2>(0);
  MoveTo<Spr::GQR3>(0);
  MoveTo<Spr::GQR4>(0);
  MoveTo<Spr::GQR5>(0);
  MoveTo<Spr::GQR6>(0);
  MoveTo<Spr::GQR7>(0);
}

inline void Init() noexcept {
  // Set HID2 |= 0xA0000000
  SprRwCtl<Hid2>() |= Hid2Bits{
      .LSQE = 1, // Load/Store Quantized Enable
      .PSE = 1,  // Paired Single Enable
  };

  ResetGQRs();
}

inline void Disable() noexcept {
  // Set HID2 &= ~0xA0000000
  SprRwCtl<Hid2>().Clear(Hid2Bits{
      .LSQE = 1, // Load/Store Quantized Disable
      .PSE = 1,  // Paired Single Disable
  });
}

} // namespace peli::ppc::PairedSingle