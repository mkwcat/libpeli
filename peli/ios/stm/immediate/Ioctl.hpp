// peli/ios/stm/immediate/Ioctl.hpp
//   Written by mkwcat
//
// Copyright (c) 2026 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../../../cmn/Types.hpp"

namespace peli::ios::stm::immediate {

/**
 * I/O Control commands for /dev/stm/immediate.
 */
enum class Ioctl : u32 {
  HotReset = 0x2001,
  HotResetForPD = 0x2002,
  Shutdown = 0x2003,
  IdleMode = 0x2004,
  Wakeup = 0x2005, // ISTM_CMD_WAKEUP

  GetState = 0x3001, // ISTM_CMD_GETSTATE
  Release = 0x3002,

  ReadDdrReg = 0x4001,
  ReadDDrReg2 = 0x4002,

  ViDimming = 0x5001,

  LedFlash = 0x6001,
  LedMode = 0x6002,

  ReadVer = 0x7001,

  WriteDmcu = 0x8001,
};

} // namespace peli::ios::stm::immediate
