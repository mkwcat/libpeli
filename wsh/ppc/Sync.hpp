// wsh/ppc/Sync.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

namespace wsh::ppc {

static inline void Sync() noexcept { __asm__ __volatile__("sync;"); }

static inline void ISync() noexcept { __asm__ __volatile__("isync;"); }

static inline void Eieio() noexcept { __asm__ __volatile__("eieio;"); }

} // namespace wsh::ppc