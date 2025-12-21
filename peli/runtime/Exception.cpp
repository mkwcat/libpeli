// peli/runtime/Exception.cpp - PowerPC exception handling
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "Exception.hpp"
#include "../common/Asm.h"
#include "../common/Types.hpp"
#include "../host/Config.h"
#include "../hw/Bit.hpp"
#include "../hw/Interrupt.hpp"
#include "../hw/ProcessorInterface.hpp"
#include "../hw/Wood.hpp"
#include "../ppc/Cache.hpp"
#include "../ppc/Context.hpp"
#include "../ppc/Exception.hpp"
#include "../ppc/Sync.hpp"
#include "../util/Halt.hpp"
#include "../util/VIConsole.hpp"
#include "SystemCall.hpp"

namespace peli::runtime {

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

using ExceptionHandler = void (*)(peli::ppc::Exception, ppc::Context *);

#define EXCEPTION_STACK_SIZE 0x800

namespace {

constinit InterruptHandler s_interrupt_handlers[32];
constinit IrqHandler s_irq_handlers[32];
constinit bool s_use_simple_irq = false;

extern "C" {
[[gnu::used]]
constinit u8 s_exception_stack[EXCEPTION_STACK_SIZE];
} // extern "C"

PELI_ASM_FUNCTION( // clang-format off
  void panicExceptionVector() noexcept,

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
  lis     %r1, (s_exception_stack + EXCEPTION_STACK_SIZE - 8)@h;
  ori     %r1, %r1, (s_exception_stack + EXCEPTION_STACK_SIZE - 8)@l;

  // Get the handler for this exception
  slwi    %r6, %r3, 2;
  lwz     %r5, INTERRUPT_HANDLER_ARRAY(%r6);
  mtsrr0  %r5;

  // Jump to the exception handler
  rfi;
  .long   0; // Terminator
                   // clang-format on
);

PELI_ASM_FUNCTION( // clang-format off
  [[maybe_unused]] void stubExceptionHandler() noexcept,
 
  // Just immediately return from the exception
  rfi;
  .long   0; // Terminator
                   // clang-format on
);

PELI_ASM_FUNCTION( // clang-format off
  void externalInterruptVector() noexcept,
  
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
  li      %r0, 0x30;
  mtsrr1  %r0;

  lis     %r0, returnFromExternalInterrupt@h;
  ori     %r0, %r0, returnFromExternalInterrupt@l;
  mtlr    %r0;

  lis     %r0, handleExternalInterrupt@h;
  ori     %r0, %r0, handleExternalInterrupt@l;
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

extern "C" {
PELI_ASM_FUNCTION( // clang-format off
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
} // extern "C"

#if !defined(PELI_ENABLE_PAIRED_SINGLE)

PELI_ASM_FUNCTION( // clang-format off
  void floatingPointUnavailableHandler() noexcept,

  stw     %r1, BACKUP_R1(0); // Backup r1
  // Enable floating-point
  li      %r1, 0x2000; // MSR.FP
  mtmsr   %r1;

  lwz     %r1, RM_CURRENT_CONTEXT(0); // r1 = Current context in real mode
  stw     %r0, 0x000(%r1); // OSContext.gprs[0]

  // Ensure MSR is updated
  isync;
  // Restore FPSCR
  lfd     %f3, 0x190(%r1); // OSContext.fpscr_pad
  mtfsf   0xFF, %f3; // Restore FPSCR

  lfd     %f0, 0x090(%r1); // OSContext.fprs[0]
  lfd     %f1, 0x098(%r1); // OSContext.fprs[1]
  lfd     %f2, 0x0A0(%r1); // OSContext.fprs[2]
  lfd     %f3, 0x0A8(%r1); // OSContext.fprs[3]
  lfd     %f4, 0x0B0(%r1); // OSContext.fprs[4]
  lfd     %f5, 0x0B8(%r1); // OSContext.fprs[5]
  lfd     %f6, 0x0C0(%r1); // OSContext.fprs[6]
  lfd     %f7, 0x0C8(%r1); // OSContext.fprs[7]
  lfd     %f8, 0x0D0(%r1); // OSContext.fprs[8]
  lfd     %f9, 0x0D8(%r1); // OSContext.fprs[9]
  lfd     %f10, 0x0E0(%r1); // OSContext.fprs[10]
  lfd     %f11, 0x0E8(%r1); // OSContext.fprs[11]
  lfd     %f12, 0x0F0(%r1); // OSContext.fprs[12]
  lfd     %f13, 0x0F8(%r1); // OSContext.fprs[13]
  lfd     %f14, 0x100(%r1); // OSContext.fprs[14]
  lfd     %f15, 0x108(%r1); // OSContext.fprs[15]
  lfd     %f16, 0x110(%r1); // OSContext.fprs[16]
  lfd     %f17, 0x118(%r1); // OSContext.fprs[17]
  lfd     %f18, 0x120(%r1); // OSContext.fprs[18]
  lfd     %f19, 0x128(%r1); // OSContext.fprs[19]
  lfd     %f20, 0x130(%r1); // OSContext.fprs[20]
  lfd     %f21, 0x138(%r1); // OSContext.fprs[21]
  lfd     %f22, 0x140(%r1); // OSContext.fprs[22]
  lfd     %f23, 0x148(%r1); // OSContext.fprs[23]
  lfd     %f24, 0x150(%r1); // OSContext.fprs[24]
  lfd     %f25, 0x158(%r1); // OSContext.fprs[25]
  lfd     %f26, 0x160(%r1); // OSContext.fprs[26]
  lfd     %f27, 0x168(%r1); // OSContext.fprs[27]
  lfd     %f28, 0x170(%r1); // OSContext.fprs[28]
  lfd     %f29, 0x178(%r1); // OSContext.fprs[29]
  lfd     %f30, 0x180(%r1); // OSContext.fprs[30]
  lfd     %f31, 0x188(%r1); // OSContext.fprs[31]

  lwz     %r0, 0x000(%r1); // OSContext.gprs[0]
  mfsrr1  %r1;
  ori     %r1, %r1, 0x2000; // Set MSR.FP
  mtsrr1  %r1; // Write back to MSR
  lwz     %r1, BACKUP_R1(0); // Restore r1
  rfi;
  .long   0; // Terminator
                   // clang-format on
);

#else // !PELI_ENABLE_PAIRED_SINGLE

PELI_ASM_FUNCTION(  // clang-format off
  void floatingPointUnavailableHandler() noexcept,

  stw     %r1, BACKUP_R1(0); // Backup r1
  // Enable floating-point
  li      %r1, 0x2000; // MSR.FP
  mtmsr   %r1;

  lwz     %r1, RM_CURRENT_CONTEXT(0); // r1 = Current context in real mode
  stw     %r0, 0x000(%r1); // OSContext.gprs[0]

  mfhid2  %r0; // OSContext.hid2
  stw     %r3, 0x00C(%r1); // OSContext.gprs[3]
  xoris   %r0, %r0, 0xA000; // Check HID2.LSQE and HID2.PSE
  mfcr    %r3;
  andis.  %r0, %r0, 0xA000;

  // Ensure MSR is updated
  isync;
  // Restore FPSCR
  lfd     %f3, 0x190(%r1); // OSContext.fpscr_pad
  mtfsf   0xFF, %f3; // Restore FPSCR

  bnea+   0x920; // Skip to just floating point registers

  psq_l   %f0, 0x1C8(%r1), 0, 0; // OSContext.psfs[0]
  psq_l   %f1, 0x1D0(%r1), 0, 0; // OSContext.psfs[1]
  psq_l   %f2, 0x1D8(%r1), 0, 0; // OSContext.psfs[2]
  psq_l   %f3, 0x1E0(%r1), 0, 0; // OSContext.psfs[3]
  psq_l   %f4, 0x1E8(%r1), 0, 0; // OSContext.psfs[4]
  psq_l   %f5, 0x1F0(%r1), 0, 0; // OSContext.psfs[5]
  psq_l   %f6, 0x1F8(%r1), 0, 0; // OSContext.psfs[6]
  psq_l   %f7, 0x200(%r1), 0, 0; // OSContext.psfs[7]
  psq_l   %f8, 0x208(%r1), 0, 0; // OSContext.psfs[8]
  psq_l   %f9, 0x210(%r1), 0, 0; // OSContext.psfs[9]
  psq_l   %f10, 0x218(%r1), 0, 0; // OSContext.psfs[10]
  psq_l   %f11, 0x220(%r1), 0, 0; // OSContext.psfs[11]
  psq_l   %f12, 0x228(%r1), 0, 0; // OSContext.psfs[12]
  psq_l   %f13, 0x230(%r1), 0, 0; // OSContext.psfs[13]
  psq_l   %f14, 0x238(%r1), 0, 0; // OSContext.psfs[14]
  psq_l   %f15, 0x240(%r1), 0, 0; // OSContext.psfs[15]
  psq_l   %f16, 0x248(%r1), 0, 0; // OSContext.psfs[16]
  psq_l   %f17, 0x250(%r1), 0, 0; // OSContext.psfs[17]
  psq_l   %f18, 0x258(%r1), 0, 0; // OSContext.psfs[18]
  psq_l   %f19, 0x260(%r1), 0, 0; // OSContext.psfs[19]
  psq_l   %f20, 0x268(%r1), 0, 0; // OSContext.psfs[20]
  psq_l   %f21, 0x270(%r1), 0, 0; // OSContext.psfs[21]
  psq_l   %f22, 0x278(%r1), 0, 0; // OSContext.psfs[22]
  psq_l   %f23, 0x280(%r1), 0, 0; // OSContext.psfs[23]
  psq_l   %f24, 0x288(%r1), 0, 0; // OSContext.psfs[24]
  psq_l   %f25, 0x290(%r1), 0, 0; // OSContext.psfs[25]
  psq_l   %f26, 0x298(%r1), 0, 0; // OSContext.psfs[26]
  psq_l   %f27, 0x2A0(%r1), 0, 0; // OSContext.psfs[27]
  psq_l   %f28, 0x2A8(%r1), 0, 0; // OSContext.psfs[28]
  psq_l   %f29, 0x2B0(%r1), 0, 0; // OSContext.psfs[29]
  psq_l   %f30, 0x2B8(%r1), 0, 0; // OSContext.psfs[30]
  psq_l   %f31, 0x2C0(%r1), 0, 0; // OSContext.psfs[31]
  ba      0x920;
  .long   0; // Terminator
                    // clang-format on
);

PELI_ASM_FUNCTION( // clang-format off
  void decrementerInterruptHandlerWithFloatLoad() noexcept,

  // Return from interrupt immediately
  rfi;
  // Align to 32-bit boundary
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;
  nop;

.L_RestoreRegularFloat:;
  // Restore regular floating point registers here because we ran out of space
  lfd     %f0, 0x090(%r1); // OSContext.fprs[0]
  lfd     %f1, 0x098(%r1); // OSContext.fprs[1]
  lfd     %f2, 0x0A0(%r1); // OSContext.fprs[2]
  lfd     %f3, 0x0A8(%r1); // OSContext.fprs[3]
  lfd     %f4, 0x0B0(%r1); // OSContext.fprs[4]
  lfd     %f5, 0x0B8(%r1); // OSContext.fprs[5]
  lfd     %f6, 0x0C0(%r1); // OSContext.fprs[6]
  lfd     %f7, 0x0C8(%r1); // OSContext.fprs[7]
  lfd     %f8, 0x0D0(%r1); // OSContext.fprs[8]
  lfd     %f9, 0x0D8(%r1); // OSContext.fprs[9]
  lfd     %f10, 0x0E0(%r1); // OSContext.fprs[10]
  lfd     %f11, 0x0E8(%r1); // OSContext.fprs[11]
  lfd     %f12, 0x0F0(%r1); // OSContext.fprs[12]
  lfd     %f13, 0x0F8(%r1); // OSContext.fprs[13]
  lfd     %f14, 0x100(%r1); // OSContext.fprs[14]
  lfd     %f15, 0x108(%r1); // OSContext.fprs[15]
  lfd     %f16, 0x110(%r1); // OSContext.fprs[16]
  lfd     %f17, 0x118(%r1); // OSContext.fprs[17]
  lfd     %f18, 0x120(%r1); // OSContext.fprs[18]
  lfd     %f19, 0x128(%r1); // OSContext.fprs[19]
  lfd     %f20, 0x130(%r1); // OSContext.fprs[20]
  lfd     %f21, 0x138(%r1); // OSContext.fprs[21]
  lfd     %f22, 0x140(%r1); // OSContext.fprs[22]
  lfd     %f23, 0x148(%r1); // OSContext.fprs[23]
  lfd     %f24, 0x150(%r1); // OSContext.fprs[24]
  lfd     %f25, 0x158(%r1); // OSContext.fprs[25]
  lfd     %f26, 0x160(%r1); // OSContext.fprs[26]
  lfd     %f27, 0x168(%r1); // OSContext.fprs[27]
  lfd     %f28, 0x170(%r1); // OSContext.fprs[28]
  lfd     %f29, 0x178(%r1); // OSContext.fprs[29]
  lfd     %f30, 0x180(%r1); // OSContext.fprs[30]
  lfd     %f31, 0x188(%r1); // OSContext.fprs[31]

  mtcr    %r3; // Restore CR
  lwz     %r3, 0x00C(%r1); // OSContext.gprs[3]
  lwz     %r0, 0x000(%r1); // OSContext.gprs[0]
  mfsrr1  %r1;
  ori     %r1, %r1, 0x2000; // Set MSR.FP
  mtsrr1  %r1; // Write back to MSR
  lwz     %r1, BACKUP_R1(0); // Restore r1
  rfi;
  .long   0; // Terminator
                   // clang-format on
);

#endif // PELI_ENABLE_PAIRED_SINGLE

extern "C" ppc::Context *handleExternalInterrupt(ppc::Context *context, u32 cr,
                                                 u32 lr, u32 srr0, u32 srr1,
                                                 u32 xer) noexcept {
  // Find out what device(s) the interrupt came from
  auto cause = hw::PI->INTSR.Hex();
  auto mask = hw::PI->INTMR.Hex();

  // Mask out unhandled interrupts
  hw::PI->INTSR = cause & ~mask;

  for (u32 i = 0; i < hw::IntCauseCount; i++) {
    if (mask & cause & (1 << i)) {
      auto handler = s_interrupt_handlers[i];
      if (handler) {
        handler(static_cast<peli::hw::IntCause>(i), context);
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
void handleIrq(hw::IntCause, ppc::Context *context) {
  u32 cause, mask;
  if (!s_use_simple_irq) {
    cause = hw::WOOD->PPCINTSTS;
    mask = hw::WOOD->PPCINTEN;
  } else {
    // Due to emulator limitations, we must assume the interrupt is from IPC
    cause = hw::BitMask(hw::Irq::IpcPpc);
    mask = hw::BitMask(hw::Irq::IpcPpc);
  }

  // Mask out unhandled interrupts
  hw::WOOD->PPCINTSTS = cause & ~mask;

  for (u32 i = 0; i < hw::IrqCount; i++) {
    if (mask & cause & (1 << i)) {
      auto handler = s_irq_handlers[i];
      if (handler) {
        handler(static_cast<peli::hw::Irq>(i), context);
      }
    }
  }
}

void writeFunctionToVector(peli::ppc::Exception type, void (*function)()) {
  u32 *instructions = reinterpret_cast<u32 *>(function);
  u32 *vector = peli::ppc::GetExceptionVectorAddress(type);
  if (vector) {
    ppc::Cache::DcZero(vector, 64 * sizeof(u32));
    for (size_t i = 0; i < 64 && instructions[i]; i++) {
      u32 instruction = instructions[i];
      if (instruction == 0x3860DEADu) {
        // li r3, 0xDEAD -> li r3, exception_type
        instruction = 0x38600000u | static_cast<u32>(type);
      }
      vector[i] = instruction;
    }
    ppc::Cache::DcFlush(vector, 64 * sizeof(u32));
    ppc::Cache::IcInvalidate(vector, 64 * sizeof(u32));
  }
}

u32 *checkStackAddr(u32 stackAddr) {
  if ((stackAddr >= 0x80003F00 && stackAddr <= 0x817FFFF8) ||
      (stackAddr >= 0x90000020 && stackAddr <= 0x935FFFF8)) {
    return reinterpret_cast<u32 *>(stackAddr);
  }

  return nullptr;
}

void printHex(util::VIConsole &console, u32 value) noexcept {
  static constexpr char HexDigits[] = "0123456789ABCDEF";

  using CharArray = char[10];
  console.Print(
      CharArray{
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
      },
      10);
}

_PELI_GNU_CLANG_ONLY([[gnu::noinline]])
void printStringList(util::VIConsole &console, size_t count,
                     const char *const *strings, u32 *values) {
  for (size_t i = 0; i < count; i++) {
    if ((i % 2) == 0) {
      console.Print(strings[i >> 1]);
    } else {
      printHex(console, values[i >> 1]);
    }
  }
}

[[noreturn]]
void defaultErrorHandler(peli::ppc::Exception type,
                         ppc::Context *context) noexcept {
  util::VIConsole console;

  console.Print("\n\n  ########### EXCEPTION (");
  console.Print(peli::ppc::GetExceptionName(type));

  using StringList = const char *const[];
  using IntList = u32[];

  // Print basic context info
  static constinit StringList s_strings0 = {") OCCURRED! ###########\n  PC:   ",
                                            "   MSR:  ",
                                            "   DSIR: ",
                                            "\n  CTR:  ",
                                            "   XER:  ",
                                            "   DAR:  ",
                                            "\n\n  Registers:"};
  printStringList(console, 13, s_strings0,
                  IntList{context->srr0, context->srr1,
                          ppc::GetSpr<ppc::Spr::DSISR>(), context->ctr,
                          context->xer, ppc::GetSpr<ppc::Spr::DAR>()});

  // Print registers
  for (size_t i = 0; i < 32; i++) {
    size_t reg = i % 4 * 8 + i / 4;
    using CharArray = char[10];
    console.Print(CharArray{i % 4 == 0 ? '\n' : ' ', ' ', ' ', 'r',
                            static_cast<char>('0' + reg / 10),
                            static_cast<char>('0' + reg % 10), ':', ' '},
                  8);
    printHex(console, context->gprs[reg]);
  }

  console.Print("\n\n  Address:      Back Chain    LR Save");
  u32 *stack = checkStackAddr(context->gprs[1]);
  // Print the first LR which may not be on the stack
  if (stack == nullptr || stack[1] != context->lr) {
    if (stack == nullptr || (stack = checkStackAddr(stack[0])) == nullptr ||
        stack[1] != context->lr) {
      console.Print("\n  ----------:   ----------    ");
      printHex(console, context->lr);
    }
  }

  for (u32 *stack = checkStackAddr(context->gprs[1]); stack != nullptr;
       stack = checkStackAddr(stack[0])) {
    static constinit StringList s_strings1 = {"\n  ", ":   ", "    "};
    printStringList(console, 6, s_strings1,
                    IntList{reinterpret_cast<u32>(stack), stack[0], stack[1]});
  }

  ppc::Sync();
  util::Halt();
}

} // namespace

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

void SetExceptionHandler(peli::ppc::Exception type,
                         ExceptionHandler handler) noexcept {
  if (type > peli::ppc::Exception::ThermalManagement) {
    return;
  }

  ios::g_lo_mem.os_globals.os_interrupt_table[static_cast<size_t>(type)] =
      reinterpret_cast<u32>(handler);
}

void SetInterruptEventHandler(hw::IntCause type,
                              InterruptHandler handler) noexcept {
  if (type >= hw::IntCause::Count) {
    return;
  }

  s_interrupt_handlers[static_cast<size_t>(type)] = handler;

  hw::PI->INTMR |= (1 << static_cast<u32>(type));
}

void SetIrqHandler(hw::Irq type, IrqHandler handler) noexcept {
  if (type >= hw::Irq::Count) {
    return;
  }

  s_irq_handlers[static_cast<size_t>(type)] = handler;

  hw::WOOD->PPCINTEN |= (1 << static_cast<u32>(type));
  ppc::Eieio();

  if (hw::WOOD->PPCINTEN == 0) {
    // For Dolphin, which doesn't support reading from IRQ
    s_use_simple_irq = true;
  }
}

void InitExceptions() noexcept {
  // Set all default exception handlers
  for (size_t i = 0;
       i < sizeof(ios::g_lo_mem.os_globals.os_interrupt_table) / sizeof(u32);
       i++) {
    ios::g_lo_mem.os_globals.os_interrupt_table[i] =
        reinterpret_cast<u32>(defaultErrorHandler);
  }

  constexpr ppc::Exception types[] = {
      ppc::Exception::SystemReset,
      ppc::Exception::MachineCheck,
      ppc::Exception::DataStorageInterrupt,
      ppc::Exception::InstructionStorageInterrupt,
      ppc::Exception::Alignment,
      ppc::Exception::Program,
#if !defined(PELI_ENABLE_FLOAT)
      ppc::Exception::FloatingPointUnavailable,
#endif // !PELI_ENABLE_FLOAT
      ppc::Exception::SystemCall,
      ppc::Exception::Trace,
      ppc::Exception::FloatingPointAssist,
      ppc::Exception::PerformanceMonitor,
      ppc::Exception::InstructionAddressBreakpoint,
      ppc::Exception::Reserved,
      ppc::Exception::ThermalManagement,
  };

  // Setup error vectors
  for (ppc::Exception type : types) {
    writeFunctionToVector(type, panicExceptionVector);
  }

  // Write the external interrupt vector
  writeFunctionToVector(ppc::Exception::ExternalInterrupt,
                        externalInterruptVector);

  // Stub the decrementer exception handler - it's not used

  writeFunctionToVector(ppc::Exception::Decrementer,
#if defined(PELI_ENABLE_FLOAT) && defined(PELI_ENABLE_PAIRED_SINGLE)
                        decrementerInterruptHandlerWithFloatLoad
#else
                        stubExceptionHandler
#endif
  );

  // Write the system call handler
  writeFunctionToVector(ppc::Exception::SystemCall, SystemCall::detail::SystemCallHandler);

#if defined(PELI_ENABLE_FLOAT)
  // Write the floating-point unavailable handler
  writeFunctionToVector(ppc::Exception::FloatingPointUnavailable,
                        floatingPointUnavailableHandler);
#endif // PELI_ENABLE_FLOAT

  ppc::SyncBroadcast();

  hw::PI->INTMR = 0;
  hw::WOOD->PPCINTEN = 0;

  // Register IRQ handler
  SetInterruptEventHandler(hw::IntCause::Irq, handleIrq);
}

} // namespace peli::runtime