// peli/rt/Exit.cpp
//   Written by mkwcat
//
// Copyright (c) 2026 mkwcat
// SPDX-License-Identifier: MIT

#include "Exit.hpp"
#include "../cmn/Types.hpp"
#include "../util/Halt.hpp"
#include "../util/Memory.hpp"
#include "../util/Optimize.hpp"

namespace peli::rt {

constexpr inline u32 StubAddr = 0x80001800;
constexpr inline u64 StubHaxx = 0x5354554248415858ull; // "STUBHAXX"

void Exit(int status) noexcept {
  PrepareExit();

  if (util::ImmRead<u64>(StubAddr + 0x4) == StubHaxx) {
    reinterpret_cast<void (*)(int)>(StubAddr)(status);
    _PELI_UNREACHABLE();
  }

  util::Halt();
}

void PrepareExit() noexcept {}

} // namespace peli::rt