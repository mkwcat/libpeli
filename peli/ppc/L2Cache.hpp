// peli/ppc/L2Cache.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "L2cr.hpp"
#include "Msr.hpp"
#include "SprRwCtl.hpp"
#include "Sync.hpp"

namespace peli::ppc::L2Cache {

inline void Enable(bool enable) noexcept { SprRwCtl<L2cr>()->L2E = enable; }

inline bool IsEnabled() noexcept { return SprRwCtl<L2cr>()->L2E; }

inline void Invalidate() noexcept {
  // Enable Global Invalidate
  SprRwCtl<L2cr>()->L2I = true;
  Sync();

  // Wait for completion
  while (L2cr::MoveFrom().L2IP) {
  }

  SprRwCtl<L2cr>()->L2I = false;
  Sync();

  // Wait for completion, although this should never be 1
  while (L2cr::MoveFrom().L2IP) {
  }
}

inline void Flush() noexcept {}

inline void Init() noexcept {
  {
    Msr::OverrideScope msrOvr(MsrBits{
        .IR = true, // Instruction Address Translation
        .DR = true, // Data Address Translation
    });

    Sync();

    // Disable L2 Cache and reset configuration
    L2cr{}.MoveTo();
    Sync();

    Invalidate();
  }

  SprRwCtl<L2cr>() <=> [](L2cr l2cr) {
    l2cr.L2E = true;
    l2cr.L2I = false;
    return l2cr;
  };
}

inline void SetWriteThrough(bool enable) noexcept {
  SprRwCtl<L2cr>()->L2WT = enable;
}

} // namespace peli::ppc::L2Cache