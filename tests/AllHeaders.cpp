// peli/tests/AllHeaders.cpp - Simply includes all* headers
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include <peli/cmn/Macro.h>
#include <peli/cmn/Types.hpp>
#include <peli/host/Config.h>
#include <peli/host/Host.hpp>
#include <peli/host/MessageQueue.hpp>
#include <peli/host/Mutex.hpp>
#include <peli/hw/Bit.hpp>
#include <peli/hw/Interrupt.hpp>
#include <peli/hw/Latte.hpp>
#include <peli/hw/Namespace.hpp>
#include <peli/hw/ProcessorInterface.hpp>
#include <peli/hw/Register.hpp>
#include <peli/hw/SDHostController.hpp>
#include <peli/hw/Video.hpp>
#include <peli/hw/VideoInterface.hpp>
#include <peli/hw/Wood.hpp>
#include <peli/ios/Error.hpp>
#include <peli/ios/Interface.hpp>
#include <peli/ios/LoMem.hpp>
#include <peli/ios/Reply.hpp>
#include <peli/ios/Request.hpp>
#include <peli/ios/Resource.hpp>
#include <peli/ios/di/Types.hpp>
#include <peli/ios/es/Types.hpp>
#include <peli/ios/fs/File.hpp>
#include <peli/ios/fs/Interface.hpp>
#include <peli/ios/fs/Nand.hpp>
#include <peli/ios/fs/Types.hpp>
#include <peli/ios/iosc/Types.hpp>
#include <peli/ios/low/Ipc.hpp>
#include <peli/ios/net/ip/top/Types.hpp>
#include <peli/ios/sdio/Card.hpp>
#include <peli/ios/sdio/HcReg.hpp>
#include <peli/ios/sdio/Interface.hpp>
#include <peli/ios/sdio/Ioctl.hpp>
#include <peli/ios/sdio/Types.hpp>
#include <peli/nand/File.hpp>
#include <peli/nand/Nand.hpp>
#include <peli/nand/conf/Bt.hpp>
#include <peli/nand/conf/Dev.hpp>
#include <peli/nand/conf/Dvd.hpp>
#include <peli/nand/conf/Ipl.hpp>
#include <peli/nand/conf/Mpls.hpp>
#include <peli/nand/conf/Net.hpp>
#include <peli/nand/conf/SysConf.hpp>
#include <peli/nand/conf/Www.hpp>
#include <peli/ppc/Bat.hpp>
#include <peli/ppc/Cache.hpp>
#include <peli/ppc/Context.hpp>
#include <peli/ppc/Exception.hpp>
#include <peli/ppc/Gpr.hpp>
#include <peli/ppc/Hid0.hpp>
#include <peli/ppc/Hid2.hpp>
#include <peli/ppc/Hid4.hpp>
#include <peli/ppc/L2Cache.hpp>
#include <peli/ppc/L2cr.hpp>
#include <peli/ppc/Msr.hpp>
#include <peli/ppc/PairedSingle.hpp>
#include <peli/ppc/Spr.hpp>
#include <peli/ppc/SprInterface.hpp>
#include <peli/ppc/SprRwCtl.hpp>
#include <peli/ppc/Sync.hpp>
#include <peli/runtime/Args.hpp>
#include <peli/runtime/Cond.hpp>
#include <peli/runtime/Exception.hpp>
#include <peli/runtime/Memory.hpp>
#include <peli/runtime/MessageQueue.hpp>
#include <peli/runtime/Mutex.hpp>
#include <peli/runtime/Once.hpp>
#include <peli/runtime/SystemCall.hpp>
#include <peli/runtime/Thread.hpp>
#include <peli/runtime/ThreadQueue.hpp>
#include <peli/util/Address.hpp>
#include <peli/util/Bit.hpp>
#include <peli/util/Concept.hpp>
#include <peli/util/CpuCache.hpp>
#include <peli/util/Defer.hpp>
#include <peli/util/Enum.hpp>
#include <peli/util/Halt.hpp>
#include <peli/util/List.hpp>
#include <peli/util/Memory.hpp>
#include <peli/util/NoConstruct.hpp>
#include <peli/util/Optimize.hpp>
#include <peli/util/String.hpp>
#include <peli/util/Time.hpp>
#include <peli/util/Transform.hpp>
#include <peli/util/VIConsole.hpp>
#include <peli/util/VIConsoleStdOut.hpp>

#if defined(PELI_ASM)
#error "<peli/cmn/Asm.h> should not be included"
#endif

#if defined(r0)
#error "<peli/cmn/AsmRegisters.h> should not be included"
#endif

int main() {}
