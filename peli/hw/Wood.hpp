// peli/hw/Wood.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Macro.h"
#include "../common/Types.hpp"
#include "Namespace.hpp"
#include "Register.hpp"

namespace peli::hw {

struct Wood {
  /**
   * Test if the host chip is Latte. This can be used to tell if the console is
   * a Wii U.
   */
  [[__gnu__::__const__]]
  static inline bool IsLatte();

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

  _PELI_PAD(0x010, 0x024);

  struct ViSolid {
    using Size = u32;

    /**
     * U component.
     */
    /* 24-31 */ volatile u32 U : 8 = 0;

    /**
     * V component.
     */
    /* 16-23 */ volatile u32 V : 8 = 0;

    /**
     * Y component.
     */
    /* 8-15 */ volatile u32 Y : 8 = 0;

    u32 : 7;

    /**
     * Enable solid color.
     */
    /* 0 */ volatile u32 E : 1 = 0;
  };

  /**
   * VI solid color register.
   *
   * Address: 0x0D000024
   * Size: u32
   */
  Register<ViSolid> VISOLID;

  _PELI_PAD(0x028, 0x030);

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
   * Size: 32
   */
  Register<IrqBits> IOPINTSTS;

  /**
   * IOP IRQ mask register.
   *
   * Address: 0x0D00003C
   * Size: 32
   */
  Register<IrqBits> IOPIRQINTEN;

  _PELI_PAD(0x040, 0x0C0);

  struct Gpio {
    using Size = u32;

    enum Dir : Size {
      In = 0,
      Out = 1,
    };

    // TODO
    /* 0-31 */ u32 : 32;
  };

  /**
   * GPIO outputs (PPC access).
   *
   * Address: 0x0D0000C0
   * Size: 32
   */
  Register<Gpio> GPIOPPCOUT;

  /**
   * GPIO directions (PPC access). 0 = in, 1 = out.
   *
   * Address: 0x0D0000C4
   * size: 32
   */
  Register<Gpio> GPIOPPCOE;

  /**
   * GPIO inputs (PPC access).
   *
   * Address: 0x0D0000C8
   * Size: 32
   */
  Register<Gpio> GPIOPPCIN;

  /**
   * GPIO input interrupt polarity (PPC access).
   *
   * Address: 0x0D0000CC
   * Size: 32
   */
  Register<Gpio> GPIOPPCINTPOL;

  /**
   * GPIO input interrupt status (PPC access).
   *
   * Address: 0x0D0000D0
   * Size: 32
   */
  Register<Gpio> GPIOPPCINTSTS;

  /**
   * GPIO input interrupt enable (PPC access).
   *
   * Address: 0x0D0000D4
   * Size: 32
   */
  Register<Gpio> GPIOPPCINTEN;

  /**
   * GPIO straps (PPC access; read-only).
   *
   * Address: 0x0D0000D8
   * Size: 32
   */
  Register<Gpio> GPIOPPCSTRAPS;

  _PELI_PAD(0x0DC, 0x194);

  struct RstCtrl {
    using Size = u32;

    /* 27-31 */ u32 : 5;

    /**
     * Unlock external DRAM reset?
     */
    /* 26 */ volatile u32 NLCKB_EDRAM : 1;

    /**
     * External DRAM reset.
     */
    /* 25 */ volatile u32 EDRAM : 1;

    /**
     * AHB reset; kills DI, sets slot LED on, hangs IOP...
     */
    /* 24 */ volatile u32 AHB : 1;

    /**
     * IOP reset.
     */
    /* 23 */ volatile u32 IOP : 1;

    /**
     * DSP reset.
     */
    /* 22 */ volatile u32 DSP : 1;

    /**
     * VI1 reset?
     */
    /* 21 */ volatile u32 VI1 : 1;

    /**
     * Video Interface reset.
     */
    /* 20 */ volatile u32 VI : 1;

    /**
     * Processor Interface IO reset.
     */
    /* 19 */ volatile u32 IOPI : 1;

    /**
     * MEM IO reset.
     */
    /* 18 */ volatile u32 IOMEM : 1;

    /**
     * Drive Interface IO reset.
     */
    /* 17 */ volatile u32 IODI : 1;

    /**
     * Expansion Interface IO reset.
     */
    /* 16 */ volatile u32 IOEXI : 1;

    /**
     * Serial Interface IO reset.
     */
    /* 15 */ volatile u32 IOSI : 1;

    /**
     * Audio Interface I2S3 reset.
     */
    /* 14 */ volatile u32 AI_I2S3 : 1;

    /**
     * GPU (GX) reset.
     */
    /* 13 */ volatile u32 GFX : 1;

    /**
     * GPU (GX) TC/CP/PE reset.
     */
    /* 12 */ volatile u32 GFXTCPE : 1;

    /**
     * MEM reset.
     */
    /* 11 */ volatile u32 MEM : 1;

    /**
     * Drive Interface reset.
     */
    /* 10 */ volatile u32 DI : 1;

    /**
     * Processor Interface reset.
     */
    /* 9 */ volatile u32 PI : 1;

    /**
     * MEM reset again?
     */
    /* 8 */ volatile u32 MEMRSTB : 1;

    /**
     * Unlock SYSPLL reset?
     */
    /* 7 */ volatile u32 NLCKB_SYSPLL : 1;

    /**
     * SYSPLL reset.
     */
    /* 6 */ volatile u32 SYSPLL : 1;

    /**
     * PowerPC SRESET.
     */
    /* 5 */ volatile u32 CPU_SRESET : 1;

    /**
     * PowerPC HRESET.
     */
    /* 4 */ volatile u32 CPU_HRESET : 1;

    /**
     * DSKPLL reset.
     */
    /* 3 */ volatile u32 DSKPLL : 1;

    /**
     * General reset.
     */
    /* 2 */ volatile u32 RSTB : 1;

    /**
     * CPU reset?
     */
    /* 1 */ volatile u32 CRSTB : 1;

    /**
     * System reset.
     */
    /* 0 */ volatile u32 RSTINB : 1;
  };

  /**
   * Reset control register. Reset/off = 0, run/on = 1.
   *
   * Address: 0x0D000194
   * Size: u32
   */
  Register<RstCtrl> RSTCTRL;

  _PELI_PAD(0x198, 0x214);

  struct ChipRevId {
    using Size = u32;

    /* 8-31 */ u32 : 24;

    /* 4-8 */ volatile u32 HI : 4;

    /* 0-4 */ volatile u32 LO : 4;
  };

  /**
   * Hardware version number (read only).
   *
   * Address: 0x0D000214
   * Size: u32
   */
  Register<ChipRevId> CHIPREVID;

  _PELI_PAD(0x218, 0x400);
};

static_assert(sizeof(Wood) == 0x400);

namespace ppc {
inline Wood *const WOOD = reinterpret_cast<Wood *>(0xCD000000);
}
namespace iop {
inline Wood *const WOOD = reinterpret_cast<Wood *>(0xCD800000);
}
using PELI_HW_HOST_NAME::WOOD;

[[__gnu__::__const__]]
inline bool Wood::IsLatte() {
  // On original Wii, WOOD[3] would mirror and reflect the same values as
  // WOOD[0]; on Wii U it would be zero, and on Dolphin it would be -1.
  // GPIOPPCSTRAPS is used as it's read-only and accessible from PPC without
  // PPCKERN.
  return WOOD[3].GPIOPPCSTRAPS == 0;
}

} // namespace peli::hw