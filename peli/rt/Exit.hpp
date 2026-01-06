// peli/rt/Exit.hpp
//   Written by mkwcat
//
// Copyright (c) 2026 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

namespace peli::rt {

constexpr inline int ExitSuccess = 0;
constexpr inline int ExitFailure = 1;

[[noreturn]]
void Exit(int status = 0) noexcept;

void PrepareExit() noexcept;

} // namespace peli::rt