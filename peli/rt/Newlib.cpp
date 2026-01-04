// peli/rt/Newlib.cpp
//   Written by mkwcat
//
// Copyright (c) 2026 mkwcat
// SPDX-License-Identifier: MIT

#include "../host/Config.h"
#include "Exit.hpp"

#if defined(PELI_NEWLIB)

namespace peli::rt {
extern "C" {

[[gnu::noreturn]]
void __syscall_exit(int status) {
  Exit(status);
}

} // extern "C"
} // namespace peli::rt

#endif