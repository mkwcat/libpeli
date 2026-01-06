// bindings/ogc/include/ogc/system.h
//   Written by mkwcat
//
// Copyright (c) 2026 mkwcat
// SPDX-License-Identifier: MIT

#if !defined(__SYSTEM_H__)
#define __SYSTEM_H__

#include "gctypes.h"
#include "gx_struct.h"

#if defined(__cplusplus)
extern "C" {
namespace ogc {
#endif

enum {
  SYS_BASE_CACHED = 0x80000000,
  SYS_BASE_UNCACHED = 0xC0000000,
};

/**
 * SYS_ResetSystem reset types.
 */
enum {
  SYS_RESTART = 0,
  SYS_HOTRESET = 1,
  SYS_SHUTDOWN = 2,
  SYS_RETURNTOMENU = 3,
  SYS_POWEROFF = 4,
  SYS_POWEROFF_STANDBY = 5,
  SYS_POWEROFF_IDLE = 6,
};

/**
 * Memory casting utilities.
 */

inline uintptr_t MEM_VIRTUAL_TO_PHYSICAL(uintptr_t addr) {
  return ((uintptr_t)(addr) & ~SYS_BASE_CACHED);
}

inline uintptr_t MEM_PHYSICAL_TO_VIRTUAL(uintptr_t addr) {
  return ((uintptr_t)(addr) | SYS_BASE_CACHED);
}

inline uintptr_t MEM_PHYSICAL_TO_K0(uintptr_t addr) {
  return ((uintptr_t)(addr) | SYS_BASE_CACHED);
}

inline uintptr_t MEM_PHYSICAL_TO_K1(uintptr_t addr) {
  return ((uintptr_t)(addr) | SYS_BASE_UNCACHED);
}

inline uintptr_t MEM_K0_TO_PHYSICAL(uintptr_t addr) {
  return ((uintptr_t)(addr) & ~SYS_BASE_CACHED);
}

inline uintptr_t MEM_K1_TO_PHYSICAL(uintptr_t addr) {
  return ((uintptr_t)(addr) & ~SYS_BASE_UNCACHED);
}

inline uintptr_t MEM_K0_TO_K1(uintptr_t addr) {
  return MEM_PHYSICAL_TO_K1(MEM_K0_TO_PHYSICAL(addr));
}

inline uintptr_t MEM_K1_TO_K0(uintptr_t addr) {
  return MEM_PHYSICAL_TO_K0(MEM_K1_TO_PHYSICAL(addr));
}

/**
 * Libogc system initialization. Does nothing in libpeli.
 */
inline void SYS_Init(void) {}

void *SYS_AllocateFramebuffer(GXRModeObj *rmode);

#if defined(__cplusplus)
} // namespace ogc
using namespace ogc;
} // extern "C"
#endif

#endif // __SYSTEM_H__