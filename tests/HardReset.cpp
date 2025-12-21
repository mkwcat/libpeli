// peli/tests/HardReset.cpp - Hard reset the console
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

// This test requires BUSPROT be disabled.

// This is a bare minimum test that doesn't require any video initialization to
// verify that it worked.

#include <peli/hw/Wood.hpp>
#include <peli/util/Halt.hpp>

int main() {
  peli::hw::WOOD->RSTCTRL.RSTB = false;
  peli::util::Halt();
}
