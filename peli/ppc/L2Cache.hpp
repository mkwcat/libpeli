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

static inline void Enable(bool enable) noexcept {
  SprRwCtl<L2cr>()->L2E = enable;
}

static inline bool IsEnabled() noexcept { return SprRwCtl<L2cr>()->L2E; }

static inline void Init() noexcept {
  {
    Msr::OverrideScope msrOvr(MsrBits{
        .IR = true, // Instruction Address Translation
        .DR = true, // Data Address Translation
    });

    Sync();
    Sync();

    // Disable in case it's enabled
    SprRwCtl<L2cr>()->L2E = false;
    Sync();
    Sync();

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

  SprRwCtl<L2cr>() <=> [](L2cr l2cr) {
    l2cr.L2E = true;
    return l2cr.L2I = false;
  };
}

} // namespace peli::ppc::L2Cache