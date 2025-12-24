// peli/ios/sdio/Interface.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../../common/Types.hpp"
#include "../Interface.hpp"
#include "Ioctl.hpp"
#include "Types.hpp"

namespace peli::ios::sdio {

struct Interface : public ios::Interface<Ioctl> {
  using WriteHostCtrlReg = IoctlDef<Ioctl::SD_IOWHCREG, RegOp, void>;
  using ReadHostCtrlReg = IoctlDef<Ioctl::SD_IORHCREG, RegOp, u32>;
  using Reset = IoctlDef<Ioctl::SD_RESET, void, u32>;
  using SetClock = IoctlDef<Ioctl::SD_SETCLK, u32, void>;
  using Command = IoctlDef<Ioctl::SD_CMD, Command, u32[4]>;
  using DataCommand =
      IoctlVecDef<Ioctl::SD_CMD, In<Command, void *>, Out<u32[4]>>;
  using GetStatus = IoctlDef<Ioctl::SD_GET_STATUS, void, u32>;
  using GetOcr = IoctlDef<Ioctl::SD_GET_OCR, void, u32>;
};

} // namespace peli::ios::sdio
