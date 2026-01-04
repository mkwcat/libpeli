// peli/ios/stm/eventhook/Ioctl.hpp
//   Written by mkwcat
//
// Copyright (c) 2026 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../../../cmn/Types.hpp"

namespace peli::ios::stm::eventhook {

/**
 * I/O Control commands for /dev/stm/eventhook.
 */
enum class Ioctl : u32 {
  EventHook = 0x1000,
};

} // namespace peli::ios::stm::eventhook
