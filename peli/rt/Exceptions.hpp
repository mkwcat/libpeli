// peli/rt/Exceptions.hpp - PowerPC exception handling
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../hw/Interrupt.hpp"
#include "../ppc/Context.hpp"
#include "../ppc/Exception.hpp"

namespace peli::rt {

struct Exceptions {
  using ExceptionHandler = void (*)(ppc::Exception, ppc::Context *);
  using InterruptHandler = void (*)(hw::IntCause, ppc::Context *);
  using IrqHandler = void (*)(hw::Irq, ppc::Context *);

  static void Init() noexcept;

  static void SetHandler(ppc::Exception type,
                         ExceptionHandler handler) noexcept;

  static void SetInterruptEventHandler(hw::IntCause type,
                                       InterruptHandler handler) noexcept;
  static void SetIrqHandler(hw::Irq type, IrqHandler handler) noexcept;

  static void StubHandlers() noexcept;

  [[__noreturn__]]
  static void DefaultErrorHandler(peli::ppc::Exception type,
                                  ppc::Context *context) noexcept;
};

} // namespace peli::rt