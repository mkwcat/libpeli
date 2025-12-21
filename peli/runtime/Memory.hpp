// peli/runtime/Memory.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.hpp"

namespace peli::runtime {

class Memory {
public:
  static u8 *Mem1ArenaStart;
  static u8 *Mem1ArenaEnd;
  static u8 *Mem2ArenaStart;
  static u8 *Mem2ArenaEnd;
  static u8 *IpcHeapStart;
  static u8 *IpcHeapEnd;

  static void InitArena();

  static u8 *AllocFromMem1ArenaLo(size_t size, size_t align);
  static u8 *AllocFromMem1ArenaHi(size_t size, size_t align);
  static u8 *AllocFromMem2ArenaLo(size_t size, size_t align);
  static u8 *AllocFromMem2ArenaHi(size_t size, size_t align);

  static u8 *SbrkAlloc(size_t size);
  static u8 *SbrkFree(size_t size);
};

} // namespace peli::runtime