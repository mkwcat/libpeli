// wsh/hw/Interrupt.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "reg/ProcessorInterface.hpp"
#include "reg/Wood.hpp"

namespace wsh::hw {

using Irq = reg::Wood::Irq;
constexpr u32 IrqCount = reg::Wood::IrqCount;

using IntCause = reg::ProcessorInterface::Interrupt;
constexpr u32 IntCauseCount = reg::ProcessorInterface::InterruptCount;

} // namespace wsh::hw