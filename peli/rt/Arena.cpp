// peli/rt/Arena.cpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "Arena.hpp"
#include "../cmn/Types.hpp"
#include "../host/Config.h"
#include "../ppc/Msr.hpp"
#include "../util/Address.hpp"
#include "Linker.hpp"

#if defined(PELI_NEWLIB)
#include <errno.h>
#include <reent.h>
#endif

namespace peli::rt {

constinit u8 *Arena::Mem1Start = ld::__mem1_arena_start;
constinit u8 *Arena::Mem1End = ld::__mem1_arena_end;
constinit u8 *Arena::Mem2Start = ld::__mem2_arena_start;
constinit u8 *Arena::Mem2End = ld::__mem2_arena_end;

void Arena::Reset() {
  Mem1Start = ld::__mem1_arena_start;
  Mem1End = ld::__mem1_arena_end;
  Mem2Start = ld::__mem2_arena_start;
  Mem2End = ld::__mem2_arena_end;
}

u8 *Arena::AllocFromMem1Lo(size_t size, size_t align) {
  ppc::Msr::NoInterruptsScope nis;

  u8 *ptr = util::AlignUp(align, Mem1Start);
  u8 *end = ptr + size;

  if (end > Mem1End) {
    return nullptr;
  }

  Mem1Start = end;
  return ptr;
}

u8 *Arena::AllocFromMem1Hi(size_t size, size_t align) {
  ppc::Msr::NoInterruptsScope nis;

  u8 *ptr = util::AlignDown(align, Mem1End - size);
  if (ptr < Mem1Start) {
    return nullptr;
  }

  Mem1End = ptr;
  return ptr;
}

u8 *Arena::AllocFromMem2Lo(size_t size, size_t align) {
  ppc::Msr::NoInterruptsScope nis;

  u8 *ptr = util::AlignUp(align, Mem2Start);
  u8 *end = ptr + size;

  if (end > Mem2End) {
    return nullptr;
  }

  Mem2Start = end;
  return ptr;
}

u8 *Arena::AllocFromMem2Hi(size_t size, size_t align) {
  ppc::Msr::NoInterruptsScope nis;

  u8 *ptr = util::AlignDown(align, Mem2End - size);
  if (ptr < Mem2Start) {
    return nullptr;
  }

  Mem2End = ptr;
  return ptr;
}

u8 *Arena::SbrkAlloc(size_t size) {
  ppc::Msr::NoInterruptsScope nis;

  // Check if the requested size exceeds the available memory
  if (Mem1Start + size <= Mem1End) {
    // In MEM1
    u8 *ptr = Mem1Start;
    Mem1Start += size;
    return ptr;
  } else if (Mem2Start + size <= Mem2End) {
    // In MEM2
    u8 *ptr = Mem2Start;
    Mem2Start += size;
    return ptr;
  } else {
    // Not enough memory available
    return nullptr;
  }
}

u8 *Arena::SbrkFree(size_t size) {
  // TODO
  Mem1Start -= size;
  return Mem1Start;
}

#if defined(PELI_NEWLIB)

extern "C" void *_sbrk_r(struct _reent *r, ptrdiff_t size) {
  // Check if the requested size is valid
  if (size == 0) {
    r->_errno = EINVAL;
    return nullptr;
  }

  if (size < 0) {
    // Free memory
    u8 *ptr = Arena::SbrkFree(-size);
    if (ptr == nullptr) {
      r->_errno = ENOMEM;
      return reinterpret_cast<void *>(-1);
    }
    return ptr;
  }

  // Allocate memory using SbrkMemory
  u8 *ptr = Arena::SbrkAlloc(size);
  if (ptr == nullptr) {
    r->_errno = ENOMEM;
    return reinterpret_cast<void *>(-1);
  }

  return ptr;
}

#endif

} // namespace peli::rt