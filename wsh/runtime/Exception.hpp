// wsh/runtime/Exception.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../hw/Interrupt.hpp"
#include "../ppc/Context.hpp"
#include "../ppc/Exception.hpp"

namespace wsh::runtime {

using ExceptionHandler = void (*)(ppc::Exception, ppc::Context *);
using InterruptHandler = void (*)(hw::IntCause, ppc::Context *);
using IrqHandler = void (*)(hw::Irq, ppc::Context *);

void SetExceptionHandler(ppc::Exception type,
                         ExceptionHandler handler) noexcept;

void SetInterruptEventHandler(hw::IntCause type,
                              InterruptHandler handler) noexcept;
void SetIrqHandler(hw::Irq type, IrqHandler handler) noexcept;

void StubExceptionHandlers() noexcept;

void InitExceptions() noexcept;

} // namespace wsh::runtime