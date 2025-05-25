// wsh/hw/reg/ProcessorInterface.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../../common/Types.h"
#include "../../util/Detail.hpp"
#include "detail/Register.hpp"

namespace wsh::hw::reg {

static struct ProcessorInterface {
  enum class Interrupt {
    Pi = 0,
    Rsw = 1,
    Di = 2,
    Si = 3,
    Exi = 4,
    Ai = 5,
    Dsp = 6,
    Mi = 7,
    Vi = 8,
    PeToken = 9,
    PeFinish = 10,
    Cp = 11,
    Debug = 12,
    Hsp = 13,
    Irq = 14,
    Rswst = 16,

    Count = 32,
  };

  static constexpr u32 InterruptCount = static_cast<u32>(Interrupt::Count);

  struct InterruptBits {
    using Size = u32;

    /* 17-31 */ u32 : 15;

    /**
     * Reset Switch State (1 when pressed).
     */
    /* 16 */ volatile u32 RSWST : 1;

    /* 15 */ u32 : 1;

    /**
     * IRQ.
     */
    /* 14 */ volatile u32 IRQ : 1;

    /**
     * High Speed Port.
     */
    /* 13 */ volatile u32 HSP : 1;

    /**
     * External Debugger.
     */
    /* 12 */ volatile u32 DEBUG : 1;

    /**
     * Command FIFO.
     */
    /* 11 */ volatile u32 CP : 1;

    /**
     * Pixel Encoder: Frame is ready.
     */
    /* 10 */ volatile u32 PE_FINISH : 1;

    /**
     * Pixel Encoder: Token assertion in command list.
     */
    /* 9 */ volatile u32 PE_TOKEN : 1;

    /**
     * Video Interface.
     */
    /* 8 */ volatile u32 VI : 1;

    /**
     * Memory Interface.
     */
    /* 7 */ volatile u32 MI : 1;

    /**
     * DSP.
     */
    /* 6 */ volatile u32 DSP : 1;

    /**
     * Audio Interface (Streaming).
     */
    /* 5 */ volatile u32 AI : 1;

    /**
     * Expansion Interface.
     */
    /* 4 */ volatile u32 EXI : 1;

    /**
     * Serial Interface.
     */
    /* 3 */ volatile u32 SI : 1;

    /**
     *  DVD Interface.
     */
    /* 2 */ volatile u32 DI : 1;

    /**
     * Reset Switch.
     */
    /* 1 */ volatile u32 RSW : 1;

    /**
     * Processor Interface.
     */
    /* 0 */ volatile u32 PI : 1;
  };

  /**
   * Interrupt Cause.
   *
   * Address: 0xCC003000
   * Size: u32
   */
  detail::Register<InterruptBits> INTSR;

  /**
   * Interrupt Mask.
   *
   * Address: 0xCC003004
   * Size: u32
   */
  detail::Register<InterruptBits> INTMR;

  _WSH_PAD(0x008, 0x100);
} *const PI = reinterpret_cast<ProcessorInterface *>(0xCC003000);

static_assert(sizeof(ProcessorInterface) == 0x100);

} // namespace wsh::hw::reg