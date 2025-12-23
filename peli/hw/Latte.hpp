// peli/hw/Latte.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Macro.h"
#include "../common/Types.hpp"
#include "Namespace.hpp"
#include "Register.hpp"
#include "Wood.hpp"

namespace peli::hw {

struct Latte : Wood {
  /**
   * General purpose IPC message register 1 for PPC core 0. Conventionally set
   * by PPC.
   *
   * Address: 0x0D000400
   * Size: u32
   */
  Register<IpcMsg> IPCPPCMSG0;

  /**
   * PPC IPC control flags for PPC core 0.
   *
   * Address: 0x0D000404
   * Size: u32
   */
  Register<IpcCtrl> IPCPPCCTRL0;

  /**
   * General purpose IPC message register 2 for PPC core 0. Conventionally set
   * by IOP.
   *
   * Address: 0x0D000408
   * Size: u32
   */
  Register<IpcMsg> IPCIOPMSG0;

  /**
   * IOP IPC control flags for PPC core 0. This register requires kernel access.
   *
   * Address: 0x0D00040C
   * Size: u32
   */
  Register<IpcCtrl> IPCIOPCTRL0;

  /**
   * General purpose IPC message register 1 for PPC core 1. Conventionally set
   * by PPC.
   *
   * Address: 0x0D000410
   * Size: u32
   */
  Register<IpcMsg> IPCPPCMSG1;

  /**
   * PPC IPC control flags for PPC core 1.
   *
   * Address: 0x0D000414
   * Size: u32
   */
  Register<IpcCtrl> IPCPPCCTRL1;

  /**
   * General purpose IPC message register 2 for PPC core 1. Conventionally set
   * by IOP.
   *
   * Address: 0x0D000418
   * Size: u32
   */
  Register<IpcMsg> IPCIOPMSG1;

  /**
   * IOP IPC control flags for PPC core 1. This register requires kernel access.
   *
   * Address: 0x0D00041C
   * Size: u32
   */
  Register<IpcCtrl> IPCIOPCTRL1;

  /**
   * General purpose IPC message register 1 for PPC core 2. Conventionally set
   * by PPC.
   *
   * Address: 0x0D000420
   * Size: u32
   */
  Register<IpcMsg> IPCPPCMSG2;

  /**
   * PPC IPC control flags for PPC core 2.
   *
   * Address: 0x0D000424
   * Size: u32
   */
  Register<IpcCtrl> IPCPPCCTRL2;

  /**
   * General purpose IPC message register 2 for PPC core 2. Conventionally set
   * by IOP.
   *
   * Address: 0x0D000428
   * Size: u32
   */
  Register<IpcMsg> IPCIOPMSG2;

  /**
   * IOP IPC control flags for PPC core 2. This register requires kernel access.
   *
   * Address: 0x0D00042C
   * Size: u32
   */
  Register<IpcCtrl> IPCIOPCTRL2;

  _PELI_PAD(0x430, 0x5A0);

  struct ChipRevId {
    using Size = u32;

    /* 16-31 */ volatile u32 MAGIC : 16;

    /* 8-16 */ u32 : 8;

    /* 4-8 */ volatile u32 HI : 4;

    /* 0-4 */ volatile u32 LO : 4;
  };

  /**
   * Hardware version number (read only).
   *
   * Address: 0x0D0005A0
   * Size: u32
   */
  Register<ChipRevId> CHIPREVID;

  _PELI_PAD(0x5A4, 0x5E0);

  struct RstCtrl {
    using Size = u32;

    /* 1-31 */ u32 : 31;

    /**
     * System reset.
     */
    /* 0 */ volatile u32 RSTINB : 1;
  };

  /**
   * Reset control register. Reset/off = 0, run/on = 1.
   *
   * Address: 0x0D0005E0
   * Size: u32
   */
  Register<RstCtrl> RSTCTRL;

  _PELI_PAD(0x5E4, 0x1000);
};

static_assert(sizeof(Latte) == 0x1000);

namespace ppc {
inline Latte *const LATTE = reinterpret_cast<Latte *>(0xCD000000);
}
namespace iop {
inline Latte *const LATTE = reinterpret_cast<Latte *>(0xCD800000);
}
using PELI_HW_HOST_NAME::LATTE;

} // namespace peli::hw