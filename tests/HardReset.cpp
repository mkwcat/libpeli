// peli/tests/HardReset.cpp - Hard reset the console
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

// This test requires BUSPROT be disabled.

// This is a bare minimum test that doesn't require any video initialization to
// verify that it worked.

#include <peli/hw/Latte.hpp>
#include <peli/hw/Wood.hpp>
#include <peli/util/Halt.hpp>

int main() {
  if (peli::hw::Wood::IsLatte()) {
    peli::hw::iop::LATTE->RSTCTRL.RSTINB = false;
  } else {
    peli::hw::iop::WOOD->RSTCTRL.RSTINB = false;
  }
  peli::util::Halt();
}
