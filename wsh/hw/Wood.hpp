// wsh/hw/Wood.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.h"
#include "../util/Detail.hpp"
#include "Register.hpp"

namespace wsh::hw {

static struct Wood {
  struct IpcMsg {
    using Size = u32;

    /* 0-31 */ volatile void *PTR;
  };

  struct IpcCtrl {
    using Size = u32;

    /* 6-31 */ u32 : 26;

    /**
     * Enable interrupt #30 when flag Y2 is set.
     */
    /* 5 */ volatile u32 IY2 : 1 = 0;

    /**
     * Enable interrupt #30 when flag Y1 is set.
     */
    /* 4 */ volatile u32 IY1 : 1 = 0;

    /**
     * Read/write flag X2.
     */
    /* 3 */ volatile u32 X2 : 1 = 0;

    /**
     * Read flag Y1. To clear the flag, write 1 to this bit.
     */
    /* 2 */ volatile u32 Y1 : 1 = 0;

    /**
     * Read flag Y2. To clear the flag, write 1 to this bit.
     */
    /* 1 */ volatile u32 Y2 : 1 = 0;

    /**
     * Read/write flag X1.
     */
    /* 0 */ volatile u32 X1 : 1 = 0;
  };

  /**
   * General purpose IPC message register 1. Conventionally set by PPC.
   *
   * Address: 0x0D000000
   * Size: u32
   */
  Register<IpcMsg> IPCPPCMSG;

  /**
   * PPC IPC control flags.
   *
   * Address: 0x0D000004
   * Size: u32
   */
  Register<IpcCtrl> IPCPPCCTRL;

  /**
   * General purpose IPC message register 2. Conventionally set by IOP.
   *
   * Address: 0x0D000008
   * Size: u32
   */
  Register<IpcMsg> IPCIOPMSG;

  /**
   * IOP IPC control flags. This register requires kernel access.
   *
   * Address: 0x0D00000C
   * Size: u32
   */
  Register<IpcCtrl> IPCIOPCTRL;

  _WSH_PAD(0x010, 0x030);

  enum class Irq {
    Timer = 0,
    Nand = 1,
    Aes = 2,
    Sha = 3,
    Ehci = 4,
    Ohci0 = 5,
    Ohci1 = 6,
    Sdhc = 7,
    Wl = 8,

    GpioPpc = 10,
    GpioIop = 11,

    ResetBtn = 17,
    Di = 18,

    IpcPpc = 30,
    IpcIop = 31,

    Count = 32,
  };

  static constexpr u32 IrqCount = static_cast<u32>(Irq::Count);

  struct IrqBits {
    using Size = u32;

    /* 31 */ volatile u32 IPC_IOP : 1;

    /* 30 */ volatile u32 IPC_PPC : 1;

    /* 19-29 */ u32 : 11;

    /* 18 */ volatile u32 DI : 1;

    /* 17 */ volatile u32 RESET_BTN : 1;

    /* 12-16 */ u32 : 5;

    /* 11 */ volatile u32 GPIO_IOP : 1;

    /* 10 */ volatile u32 GPIO_PPC : 1;

    /* 9 */ u32 : 1;

    /* 8 */ volatile u32 WL : 1;

    /* 7 */ volatile u32 SDHC : 1;

    /* 6 */ volatile u32 OHCI1 : 1;

    /* 5 */ volatile u32 OHCI0 : 1;

    /* 4 */ volatile u32 EHCI : 1;

    /* 3 */ volatile u32 SHA : 1;

    /* 2 */ volatile u32 AES : 1;

    /* 1 */ volatile u32 NAND : 1;

    /* 0 */ volatile u32 TIMER : 1;
  };

  /**
   * PowerPC IRQ status register.
   *
   * Address: 0x0D000030
   * Size: u32
   */
  Register<IrqBits> PPCINTSTS;

  /**
   * PowerPC IRQ mask register.
   *
   * Address: 0x0D000034
   * Size: u32
   */
  Register<IrqBits> PPCINTEN;

  /**
   * IOP IRQ status register.
   *
   * Address: 0x0D000038
   * Size: u32
   */
  Register<IrqBits> IOPINTSTS;

  /**
   * IOP IRQ mask register.
   *
   * Address: 0x0D00003C
   * Size: u32
   */
  Register<IrqBits> IOPIRQINTEN;

  _WSH_PAD(0x040, 0x400);

} *const WOOD = reinterpret_cast<Wood *>(0xCD000000);

static_assert(sizeof(Wood) == 0x400);

} // namespace wsh::hw