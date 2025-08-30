// peli/runtime/Memory.cpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "Memory.hpp"
#include "../common/Types.h"
#include "../ios/LoMem.hpp"
#include "../ppc/Msr.hpp"
#include "../util/Address.hpp"
#include <cerrno>
#include <reent.h>

namespace peli::runtime {

u8 *Memory::Mem1ArenaStart = nullptr;
u8 *Memory::Mem1ArenaEnd = nullptr;
u8 *Memory::Mem2ArenaStart = nullptr;
u8 *Memory::Mem2ArenaEnd = nullptr;
u8 *Memory::IpcHeapStart = nullptr;
u8 *Memory::IpcHeapEnd = nullptr;

void Memory::InitArena() {
  extern u8 LdArena1Lo[] asm("__Arena1Lo");
  extern u8 LdArena1Hi[] asm("__Arena1Hi");
  extern u8 LdArena2Lo[] asm("__Arena2Lo");
  extern u8 LdArena2Hi[] asm("__Arena2Hi");
  extern u8 LdIpcBufferLo[] asm("__ipcbufferLo");
  extern u8 LdIpcBufferHi[] asm("__ipcbufferHi");

  Mem1ArenaStart =
      reinterpret_cast<u8 *>(ios::g_lo_mem.os_globals.mem1_arena_start);
  Mem1ArenaEnd =
      reinterpret_cast<u8 *>(ios::g_lo_mem.os_globals.mem1_arena_end);
  Mem2ArenaStart =
      reinterpret_cast<u8 *>(ios::g_lo_mem.os_globals.mem2_arena_start);
  Mem2ArenaEnd =
      reinterpret_cast<u8 *>(ios::g_lo_mem.os_globals.mem2_arena_end);
  IpcHeapStart =
      reinterpret_cast<u8 *>(ios::g_lo_mem.os_globals.ipc_heap_start);
  IpcHeapEnd = reinterpret_cast<u8 *>(ios::g_lo_mem.os_globals.ipc_heap_end);

  if (Mem1ArenaStart == nullptr) {
    Mem1ArenaStart = LdArena1Lo;
  }
  if (Mem1ArenaEnd == nullptr) {
    Mem1ArenaEnd = LdArena1Hi;
  }

  if (Mem2ArenaStart == nullptr) {
    Mem2ArenaStart = LdArena2Lo;
  }
  if (Mem2ArenaEnd == nullptr) {
    Mem2ArenaEnd = LdArena2Hi;
  }

  if (IpcHeapStart == nullptr) {
    IpcHeapStart = LdIpcBufferLo;
  }
  if (IpcHeapEnd == nullptr) {
    IpcHeapEnd = LdIpcBufferHi;
  }
}

u8 *Memory::AllocFromMem1ArenaLo(std::size_t size, std::size_t align) {
  ppc::Msr::NoInterruptsScope nis;

  u8 *ptr = util::AlignUp(align, Mem1ArenaStart);
  u8 *end = ptr + size;

  if (end > Mem1ArenaEnd) {
    return nullptr;
  }

  Mem1ArenaStart = end;
  return ptr;
}

u8 *Memory::AllocFromMem1ArenaHi(std::size_t size, std::size_t align) {
  ppc::Msr::NoInterruptsScope nis;

  u8 *ptr = util::AlignDown(align, Mem1ArenaEnd - size);
  if (ptr < Mem1ArenaStart) {
    return nullptr;
  }

  Mem1ArenaEnd = ptr;
  return ptr;
}

u8 *Memory::AllocFromMem2ArenaLo(std::size_t size, std::size_t align) {
  ppc::Msr::NoInterruptsScope nis;

  u8 *ptr = util::AlignUp(align, Mem2ArenaStart);
  u8 *end = ptr + size;

  if (end > Mem2ArenaEnd) {
    return nullptr;
  }

  Mem2ArenaStart = end;
  return ptr;
}

u8 *Memory::AllocFromMem2ArenaHi(std::size_t size, std::size_t align) {
  ppc::Msr::NoInterruptsScope nis;

  u8 *ptr = util::AlignDown(align, Mem2ArenaEnd - size);
  if (ptr < Mem2ArenaStart) {
    return nullptr;
  }

  Mem2ArenaEnd = ptr;
  return ptr;
}

u8 *Memory::SbrkAlloc(std::size_t size) {
  ppc::Msr::NoInterruptsScope nis;

  // Check if the requested size exceeds the available memory
  if (Mem1ArenaStart + size <= Mem1ArenaEnd) {
    // In MEM1
    u8 *ptr = Mem1ArenaStart;
    Mem1ArenaStart += size;
    return ptr;
  } else if (Mem2ArenaStart + size <= Mem2ArenaEnd) {
    // In MEM2
    u8 *ptr = Mem2ArenaStart;
    Mem2ArenaStart += size;
    return ptr;
  } else {
    // Not enough memory available
    return nullptr;
  }
}

u8 *Memory::SbrkFree(std::size_t size) {
  // TODO
  Mem1ArenaStart -= size;
  return Mem1ArenaStart;
}

extern "C" void *_sbrk_r(struct _reent *r, ptrdiff_t size) {
  // Check if the requested size is valid
  if (size == 0) {
    r->_errno = EINVAL;
    return nullptr;
  }

  if (size < 0) {
    // Free memory
    u8 *ptr = Memory::SbrkFree(-size);
    if (ptr == nullptr) {
      r->_errno = ENOMEM;
      return reinterpret_cast<void *>(-1);
    }
    return ptr;
  }

  // Allocate memory using SbrkMemory
  u8 *ptr = Memory::SbrkAlloc(size);
  if (ptr == nullptr) {
    r->_errno = ENOMEM;
    return reinterpret_cast<void *>(-1);
  }

  return ptr;
}

} // namespace peli::runtime