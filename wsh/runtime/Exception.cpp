// wsh/runtime/Exception.cpp - PowerPC exception handling
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "Exception.hpp"
#include "../common/Asm.h"
#include "../common/Types.h"
#include "../hw/Bit.hpp"
#include "../hw/Interrupt.hpp"
#include "../hw/reg/ProcessorInterface.hpp"
#include "../hw/reg/Wood.hpp"
#include "../ppc/Cache.hpp"
#include "../ppc/Context.hpp"
#include "../ppc/Exception.hpp"
#include "../util/Halt.hpp"
#include "../util/ViConsole.hpp"
#include <array>
#include <cstddef>
#include <cstring>

namespace wsh::runtime {

#define GQR0 912
#define GQR1 913
#define GQR2 914
#define GQR3 915
#define GQR4 916
#define GQR5 917
#define GQR6 918
#define GQR7 919

#define BACKUP_R1 0x00BC
#define RM_CURRENT_CONTEXT 0x00C0
#define EM_CURRENT_CONTEXT 0x00D4
#define INTERRUPT_HANDLER_ARRAY 0x3040

using ExceptionHandler = void (*)(wsh::ppc::Exception, ppc::Context *);

#define EXCEPTION_STACK_SIZE 0x800
[[gnu::used]]
static constinit u8
    s_exception_stack[EXCEPTION_STACK_SIZE] asm("_WshExceptionStack");

static constinit InterruptHandler s_interrupt_handlers[32];
static constinit IrqHandler s_irq_handlers[32];

ppc::Context *handleExternalInterrupt(ppc::Context *context, u32 cr, u32 lr,
                                      u32 srr0, u32 srr1, u32 xer) noexcept
    asm("_WshHandleExternalInterrupt");
void returnFromExternalInterrupt(ppc::Context *context) noexcept
    asm("_WshReturnFromExternalInterrupt");

WSH_ASM_FUNCTION( // clang-format off
  static void panicExceptionVector() noexcept,

  stw     %r1, BACKUP_R1(0); // Backup r1
  lwz     %r1, RM_CURRENT_CONTEXT(0); // r1 = Current context in real mode

  // Save initial registers to context
  stmw    %r2, 0x008(%r1); // OSContext.gprs[2+]
  li      %r3, -(0x10000 - 0xDEAD); // Exception type; set on runtime
  stw     %r0, 0x000(%r1); // OSContext.gprs[0]

  mfsrr0  %r5;
  stw     %r5, 0x198(%r1); // OSContext.srr0
  mfsrr1  %r6;
  stw     %r6, 0x19C(%r1); // OSContext.srr1

  // Save special purpose registers
  mfcr    %r0;
  stw     %r0, 0x080(%r1); // OSContext.cr
  mflr    %r5;
  stw     %r5, 0x084(%r1); // OSContext.lr
  mfctr   %r6;
  stw     %r6, 0x088(%r1); // OSContext.ctr
  mfxer   %r0;
  stw     %r0, 0x090(%r1); // OSContext.xer

  // Save GQR values
  mfspr   %r5, GQR0;
  stw     %r5, 0x1A4(%r1); // OSContext.gqrs[0]
  mfspr   %r6, GQR1;
  stw     %r6, 0x1A8(%r1); // OSContext.gqrs[1]
  mfspr   %r0, GQR2;
  stw     %r0, 0x1AC(%r1); // OSContext.gqrs[2]
  mfspr   %r5, GQR3;
  stw     %r5, 0x1B0(%r1); // OSContext.gqrs[3]
  mfspr   %r6, GQR4;
  stw     %r6, 0x1B4(%r1); // OSContext.gqrs[4]
  mfspr   %r0, GQR5;
  stw     %r0, 0x1B8(%r1); // OSContext.gqrs[5]
  mfspr   %r5, GQR6;
  stw     %r5, 0x1BC(%r1); // OSContext.gqrs[6]
  mfspr   %r6, GQR7;
  stw     %r6, 0x1C0(%r1); // OSContext.gqrs[7]

  // Restore memory address translation after rfi
  li      %r0, 0x30;
  mtsrr1  %r0;

  // Copy backup r1 to the context
  lwz     %r5, BACKUP_R1(0);
  stw     %r5, 0x004(%r1); // OSContext.gprs[1]

  lwz     %r4, EM_CURRENT_CONTEXT(0); // Context for callee

  // Set up the stack pointer
  lis     %r1, (_WshExceptionStack + EXCEPTION_STACK_SIZE - 8)@h;
  ori     %r1, %r1, (_WshExceptionStack + EXCEPTION_STACK_SIZE - 8)@l;

  // Get the handler for this exception
  slwi    %r6, %r3, 2;
  lwz     %r5, INTERRUPT_HANDLER_ARRAY(%r6);
  mtsrr0  %r5;

  // Jump to the exception handler
  rfi;
  .long   0; // Terminator
                  // clang-format on
);

WSH_ASM_FUNCTION( // clang-format off
  static void stubExceptionHandler() noexcept,
 
  // Just immediately return from the exception
  rfi;
  .long   0; // Terminator
                  // clang-format on
);

WSH_ASM_FUNCTION( // clang-format off
  static void externalInterruptVector() noexcept,
  
  stw     %r1, BACKUP_R1(0);
  lwz     %r1, RM_CURRENT_CONTEXT(0); // r1 = Current context in real mode

  // Save volatile registers to context, the rest are saved by the callee
  stw     %r0, 0x000(%r1); // OSContext.gprs[0]
  stw     %r3, 0x00C(%r1); // OSContext.gprs[3]
  stw     %r4, 0x010(%r1); // OSContext.gprs[4]
  stw     %r5, 0x014(%r1); // OSContext.gprs[5]
  stw     %r6, 0x018(%r1); // OSContext.gprs[6]
  stw     %r7, 0x01C(%r1); // OSContext.gprs[7]
  stw     %r8, 0x020(%r1); // OSContext.gprs[8]
  stw     %r9, 0x024(%r1); // OSContext.gprs[9]
  stw     %r10, 0x028(%r1); // OSContext.gprs[10]
  stw     %r11, 0x02C(%r1); // OSContext.gprs[11]
  stw     %r12, 0x030(%r1); // OSContext.gprs[12]

  // Save the CTR here as it's skipped by a fast context switch
  mfctr   %r0;
  stw     %r0, 0x088(%r1); // OSContext.ctr

  // Load registers to pass onto the handler
  lwz     %r3, EM_CURRENT_CONTEXT(0);
  mfcr    %r4;
  mflr    %r5;
  mfsrr0  %r6;
  mfsrr1  %r7;
  mfxer   %r8;

  // Restore memory address translation after rfi
  li      %r0, 0x2030;
  mtsrr1  %r0;

  lis     %r0, _WshReturnFromExternalInterrupt@ha;
  ori     %r0, %r0, _WshReturnFromExternalInterrupt@l;
  mtlr    %r0;

  lis     %r0, _WshHandleExternalInterrupt@ha;
  ori     %r0, %r0, _WshHandleExternalInterrupt@l;
  mtsrr0  %r0;

  // Load stack pointer from backup. It should be okay to use the stack
  // pointer like this
  lwz     %r1, BACKUP_R1(0);
  subi    %r1, %r1, 0x10;

  // Jump to the external interrupt handler
  rfi;
  .long   0; // Terminator
                  // clang-format on
);

WSH_ASM_FUNCTION( // clang-format off
  void returnFromExternalInterrupt(ppc::Context*) noexcept,
  // Restore some special purpose registers
  lwz     %r0, 0x080(%r3); // OSContext.cr
  mtcr    %r0;
  lwz     %r0, 0x084(%r3); // OSContext.lr
  mtlr    %r0;
  lwz     %r5, 0x088(%r3); // OSContext.ctr
  mtctr   %r5;

  // Restore volatile registers from context
  lwz     %r0, 0x000(%r3); // OSContext.gprs[0]
  lwz     %r4, 0x010(%r3); // OSContext.gprs[4]
  lwz     %r5, 0x014(%r3); // OSContext.gprs[5]
  lwz     %r6, 0x018(%r3); // OSContext.gprs[6]
  lwz     %r7, 0x01C(%r3); // OSContext.gprs[7]
  lwz     %r8, 0x020(%r3); // OSContext.gprs[8]
  lwz     %r9, 0x024(%r3); // OSContext.gprs[9]
  lwz     %r10, 0x028(%r3); // OSContext.gprs[10]
  lwz     %r11, 0x02C(%r3); // OSContext.gprs[11]
  lwz     %r12, 0x030(%r3); // OSContext.gprs[12]
  lwz     %r3, 0x00C(%r3); // OSContext.gprs[3]
    
  // Return from interrupt
  addi    %r1, %r1, 0x10;
  rfi;
                  // clang-format on
);

void StubExceptionHandlers() noexcept {
  u32 stubInst = 0x48000000; // b +0
  auto &handlers = ios::g_lo_mem_uncached.exception_handlers;

  handlers.system_reset[0] = stubInst;
  handlers.machine_check[0] = stubInst;
  handlers.data_storage_interrupt[0] = stubInst;
  handlers.instruction_storage_interrupt[0] = stubInst;
  handlers.external_interrupt[0] = stubInst;
  handlers.alignment[0] = stubInst;
  handlers.program[0] = stubInst;
  handlers.floating_point_unavailable[0] = stubInst;
  handlers.decrementer[0] = stubInst;
  handlers.system_call[0] = stubInst;
  handlers.trace[0] = stubInst;
  handlers.floating_point_assist[0] = stubInst;
  handlers.performance_monitor[0] = stubInst;
  handlers.instruction_address_breakpoint[0] = stubInst;
  handlers.reserved[0] = stubInst;
  handlers.thermal_management[0] = stubInst;
}

void SetExceptionHandler(wsh::ppc::Exception type,
                         ExceptionHandler handler) noexcept {
  if (type > wsh::ppc::Exception::ThermalManagement) {
    return;
  }

  ios::g_lo_mem.os_globals.os_interrupt_table[static_cast<std::size_t>(type)] =
      reinterpret_cast<u32>(handler);
}

void SetInterruptEventHandler(hw::IntCause type,
                              InterruptHandler handler) noexcept {
  if (type >= hw::IntCause::Count) {
    return;
  }

  s_interrupt_handlers[static_cast<std::size_t>(type)] = handler;

  hw::reg::PI->INTMR |= (1 << static_cast<u32>(type));
}

static bool s_use_simple_irq = false;

void SetIrqHandler(hw::Irq type, IrqHandler handler) noexcept {
  if (type >= hw::Irq::Count) {
    return;
  }

  s_irq_handlers[static_cast<std::size_t>(type)] = handler;

  hw::reg::WOOD->PPCINTEN |= (1 << static_cast<u32>(type));
  ppc::Eieio();

  if (hw::reg::WOOD->PPCINTEN == 0) {
    // For Dolphin, which doesn't support reading from IRQ
    s_use_simple_irq = true;
  }
}

ppc::Context *handleExternalInterrupt(ppc::Context *context, u32 cr, u32 lr,
                                      u32 srr0, u32 srr1, u32 xer) noexcept {
  using hw::reg::PI;

  // Find out what device(s) the interrupt came from
  auto cause = PI->INTSR.Hex();
  auto mask = PI->INTMR.Hex();

  // Mask out unhandled interrupts
  PI->INTSR = cause & ~mask;

  for (u32 i = 0; i < hw::IntCauseCount; i++) {
    if (mask & cause & (1 << i)) {
      auto handler = s_interrupt_handlers[i];
      if (handler) {
        handler(static_cast<wsh::hw::IntCause>(i), context);
      }
    }
  }

  context->cr = cr;
  context->lr = lr;
  ppc::SetSpr<ppc::Spr::SRR0>(srr0);
  ppc::SetSpr<ppc::Spr::SRR1>(srr1);
  ppc::SetSpr<ppc::Spr::XER>(xer);

  return context;
}

// Second layer interrupt handler for IRQ
static void handleIrq(hw::IntCause, ppc::Context *context) {
  using hw::reg::WOOD;

  u32 cause, mask;
  if (!s_use_simple_irq) {
    cause = WOOD->PPCINTSTS;
    mask = WOOD->PPCINTEN;
  } else {
    // Due to emulator limitations, we must assume the interrupt is from IPC
    cause = hw::BitMask(hw::Irq::IpcPpc);
    mask = hw::BitMask(hw::Irq::IpcPpc);
  }

  // Mask out unhandled interrupts
  WOOD->PPCINTSTS = cause & ~mask;

  for (u32 i = 0; i < hw::IrqCount; i++) {
    if (mask & cause & (1 << i)) {
      auto handler = s_irq_handlers[i];
      if (handler) {
        handler(static_cast<wsh::hw::Irq>(i), context);
      }
    }
  }
}

static void writeFunctionToVector(wsh::ppc::Exception type, u32 *function) {
  u32 *vector = wsh::ppc::GetExceptionVectorAddress(type);
  if (vector) {
    ppc::Cache::DcZero<false>(vector, 64 * sizeof(u32));
    for (std::size_t i = 0; i < 64 && function[i]; i++) {
      u32 instruction = function[i];
      if (instruction == 0x3860DEADu) {
        // li r3, 0xDEAD -> li r3, exception_type
        instruction = 0x38600000u | static_cast<u32>(type);
      }
      vector[i] = instruction;
    }
    ppc::Cache::DcFlush<false>(vector, 64 * sizeof(u32));
    ppc::Cache::IcInvalidate<true>(vector, 64 * sizeof(u32));
  }
}

static u32 *checkStackAddr(u32 stackAddr) {
  if ((stackAddr >= 0x80003F00 && stackAddr <= 0x817FFFF8) ||
      (stackAddr >= 0x90000020 && stackAddr <= 0x935FFFF8)) {
    return reinterpret_cast<u32 *>(stackAddr);
  }

  return nullptr;
}

static void printHex(util::ViConsole &console, u32 value) {
  static constexpr char HexDigits[] = "0123456789ABCDEF";

  console.Print(
      std::array<char, 10>{
          '0',
          'x',
          HexDigits[(value >> 28) & 0xF],
          HexDigits[(value >> 24) & 0xF],
          HexDigits[(value >> 20) & 0xF],
          HexDigits[(value >> 16) & 0xF],
          HexDigits[(value >> 12) & 0xF],
          HexDigits[(value >> 8) & 0xF],
          HexDigits[(value >> 4) & 0xF],
          HexDigits[value & 0xF],
      }
          .data(),
      10);
}

[[noreturn]]
static void defaultErrorHandler(wsh::ppc::Exception type,
                                ppc::Context *context) {
  util::ViConsole console;

  console.Print("\n\n   ########### EXCEPTION (");
  console.Print(wsh::ppc::GetExceptionName(type));
  console.Print(") OCCURRED! ###########\n"
                "   PC:   ");
  printHex(console, context->srr0);
  console.Print("   MSR:  ");
  printHex(console, context->srr1);
  console.Print("   DSIR: ");
  printHex(console, ppc::GetSpr<ppc::Spr::DSISR>());

  console.Print("\n   CTR:  ");
  printHex(console, context->ctr);
  console.Print("   XER:  ");
  printHex(console, context->xer);
  console.Print("   DAR:  ");
  printHex(console, ppc::GetSpr<ppc::Spr::DAR>());

  console.Print("\n\n"
                "   Registers:\n   ");
  for (std::size_t i = 0; i < 32; i++) {
    std::size_t reg = i % 4 * 8 + i / 4;

    console.Print(
        std::array<char, 5>{
            'r',
            static_cast<char>('0' + reg / 10),
            static_cast<char>('0' + reg % 10),
            ':',
            ' ',
        }
            .data(),
        5);
    printHex(console, context->gprs[reg]);

    if (i % 4 == 3) {
      console.Print("\n   ");
    } else {
      console.Print("    ");
    }
  }

  console.Print("\n   Address:      Back Chain    LR Save");
  u32 *stack = checkStackAddr(context->gprs[1]);
  // Print the first LR which may not be on the stack
  if (stack == nullptr || stack[1] != context->lr) {
    if (stack == nullptr || (stack = checkStackAddr(stack[0])) == nullptr ||
        stack[1] != context->lr) {
      console.Print("\n   ----------:   ----------    ");
      printHex(console, context->lr);
    }
  }

  for (u32 *stack = checkStackAddr(context->gprs[1]); stack != nullptr;
       stack = checkStackAddr(stack[0])) {
    console.Print("\n   ");
    printHex(console, reinterpret_cast<u32>(stack));
    console.Print(":   ");
    printHex(console, stack[0]);
    console.Print("    ");
    printHex(console, stack[1]);
  }

  ppc::Sync();
  util::Halt();
};

void InitExceptions() noexcept {
  // Set all default exception handlers
  for (size_t i = 0; i < std::size(ios::g_lo_mem.os_globals.os_interrupt_table);
       i++) {
    ios::g_lo_mem.os_globals.os_interrupt_table[i] =
        reinterpret_cast<u32>(defaultErrorHandler);
  }

  // Setup error vectors
  for (ppc::Exception type : {
           ppc::Exception::SystemReset,
           ppc::Exception::MachineCheck,
           ppc::Exception::DataStorageInterrupt,
           ppc::Exception::InstructionStorageInterrupt,
           ppc::Exception::Alignment,
           ppc::Exception::Program,
           ppc::Exception::FloatingPointUnavailable,
           ppc::Exception::SystemCall,
           ppc::Exception::Trace,
           ppc::Exception::FloatingPointAssist,
           ppc::Exception::PerformanceMonitor,
           ppc::Exception::InstructionAddressBreakpoint,
           ppc::Exception::Reserved,
           ppc::Exception::ThermalManagement,
       }) {
    writeFunctionToVector(type, reinterpret_cast<u32 *>(panicExceptionVector));
  }

  // Write the external interrupt vector
  writeFunctionToVector(ppc::Exception::ExternalInterrupt,
                        reinterpret_cast<u32 *>(externalInterruptVector));

  // Stub the decrementer exception handler - it's not used
  writeFunctionToVector(ppc::Exception::Decrementer,
                        reinterpret_cast<u32 *>(stubExceptionHandler));

  hw::reg::PI->INTMR = 0;
  hw::reg::WOOD->PPCINTEN = 0;

  // Register IRQ handler
  SetInterruptEventHandler(hw::IntCause::Irq, handleIrq);
}

} // namespace wsh::runtime