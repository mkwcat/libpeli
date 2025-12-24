// peli/ios/sdio/Ioctl.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../../common/Types.hpp"

namespace peli::ios::sdio {

/**
 * I/O Control commands for SDIO.
 */
enum class Ioctl : u32 {
  SD_IOWHCREG = 1,
  SD_IORHCREG = 2,
  SD_RESET = 4,
  SD_SETCLK = 6,
  SD_CMD = 7,
  SD_GET_STATUS = 11,
  SD_GET_OCR = 12,
};

} // namespace peli::ios::sdio
