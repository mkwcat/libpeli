// peli/rt/Arena.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../cmn/Types.hpp"

namespace peli::rt {

class Arena {
public:
  static u8 *Mem1Start;
  static u8 *Mem1End;
  static u8 *Mem2Start;
  static u8 *Mem2End;

  static void Reset();

  static u8 *AllocFromMem1Lo(size_t size, size_t align);
  static u8 *AllocFromMem1Hi(size_t size, size_t align);
  static u8 *AllocFromMem2Lo(size_t size, size_t align);
  static u8 *AllocFromMem2Hi(size_t size, size_t align);

  static u8 *SbrkAlloc(size_t size);
  static u8 *SbrkFree(size_t size);
};

} // namespace peli::rt