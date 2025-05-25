// wsh/runtime/Memory.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.h"
#include <cstddef>

namespace wsh::runtime {

class Memory {
public:
  static u8 *Mem1ArenaStart;
  static u8 *Mem1ArenaEnd;
  static u8 *Mem2ArenaStart;
  static u8 *Mem2ArenaEnd;
  static u8 *IpcHeapStart;
  static u8 *IpcHeapEnd;

  static void InitArena();

  static u8 *AllocFromMem1ArenaLo(std::size_t size, std::size_t align);
  static u8 *AllocFromMem1ArenaHi(std::size_t size, std::size_t align);
  static u8 *AllocFromMem2ArenaLo(std::size_t size, std::size_t align);
  static u8 *AllocFromMem2ArenaHi(std::size_t size, std::size_t align);

  static u8 *SbrkAlloc(std::size_t size);
  static u8 *SbrkFree(std::size_t size);
};

} // namespace wsh::runtime