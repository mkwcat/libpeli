// wsh/hw/Interrupt.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "ProcessorInterface.hpp"
#include "Wood.hpp"

namespace wsh::hw {

using Irq = Wood::Irq;
constexpr u32 IrqCount = Wood::IrqCount;

using IntCause = ProcessorInterface::Interrupt;
constexpr u32 IntCauseCount = ProcessorInterface::InterruptCount;

} // namespace wsh::hw