// peli/ppc/Exception.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../cmn/Types.hpp"
#include "../ios/LoMem.hpp"

namespace peli::ppc {

enum class Exception {
  SystemReset,
  MachineCheck,
  DataStorageInterrupt,
  InstructionStorageInterrupt,
  ExternalInterrupt,
  Alignment,
  Program,
  FloatingPointUnavailable,
  Decrementer,
  SystemCall,
  Trace,
  FloatingPointAssist,
  PerformanceMonitor,
  InstructionAddressBreakpoint,
  Reserved,
  ThermalManagement,
};

constexpr u32 ExceptionCount =
    static_cast<u32>(Exception::ThermalManagement) + 1;

constexpr const char *GetExceptionName(Exception type) noexcept {
  if (type < Exception::SystemReset || type > Exception::ThermalManagement) {
    return "UNKNOWN";
  }

  static constexpr const char *ExceptionNameTable[] = {
      "SRESET",       "MACHINE CHECK",
      "DSI",          "ISI",
      "EXI",          "ALIGNMENT",
      "PROGRAM",      "FP UNAVAILABLE",
      "DECREMENTER",  "SYSCALL",
      "TRACE",        "FP ASSIST",
      "PERF MONITOR", "INSTRUCTION ADDR BREAKPOINT",
      "RESERVED",     "THERMAL MANAGEMENT"};

  return ExceptionNameTable[static_cast<u32>(type)];
}

inline u32 *GetExceptionVectorAddress(Exception type) noexcept {
  ios::LoMem &lo_mem = ios::g_lo_mem;

  switch (type) {
  case Exception::SystemReset:
    return &lo_mem.exception_handlers.system_reset[0];
  case Exception::MachineCheck:
    return &lo_mem.exception_handlers.machine_check[0];
  case Exception::DataStorageInterrupt:
    return &lo_mem.exception_handlers.data_storage_interrupt[0];
  case Exception::InstructionStorageInterrupt:
    return &lo_mem.exception_handlers.instruction_storage_interrupt[0];
  case Exception::ExternalInterrupt:
    return &lo_mem.exception_handlers.external_interrupt[0];
  case Exception::Alignment:
    return &lo_mem.exception_handlers.alignment[0];
  case Exception::Program:
    return &lo_mem.exception_handlers.program[0];
  case Exception::FloatingPointUnavailable:
    return &lo_mem.exception_handlers.floating_point_unavailable[0];
  case Exception::Decrementer:
    return &lo_mem.exception_handlers.decrementer[0];
  case Exception::SystemCall:
    return &lo_mem.exception_handlers.system_call[0];
  case Exception::Trace:
    return &lo_mem.exception_handlers.trace[0];
  case Exception::FloatingPointAssist:
    return &lo_mem.exception_handlers.floating_point_assist[0];
  case Exception::PerformanceMonitor:
    return &lo_mem.exception_handlers.performance_monitor[0];
  case Exception::InstructionAddressBreakpoint:
    return &lo_mem.exception_handlers.instruction_address_breakpoint[0];
  case Exception::Reserved:
    return &lo_mem.exception_handlers.reserved[0];
  case Exception::ThermalManagement:
    return &lo_mem.exception_handlers.thermal_management[0];
  default:
    return nullptr;
  }
}

} // namespace peli::ppc