// peli/rt/Linker.hpp -- Linker-script provided symbols
//   Written by mkwcat
//
// Copyright (c) 2026 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../cmn/Types.hpp"

namespace peli::rt::ld {
extern "C" {

extern u8 __rodata_start[], __rodata_end[];
extern u8 __data_start[], __data_end[];
extern u8 __bss_start[], __bss_end[];
extern u8 __sdata_start[], __sdata_end[];
extern u8 __sbss_start[], __sbss_end[];
extern u8 __sdata2_start[], __sdata2_end[];
extern u8 __sbss2_start[], __sbss2_end[];

extern u8 __preinit_array_start[], __preinit_array_end[];
extern u8 __init_array_start[], __init_array_end[];
extern u8 __fini_array_start[], __fini_array_end[];

extern u8 __mem1_arena_start[], __mem1_arena_end[];
extern u8 __mem2_arena_start[], __mem2_arena_end[];

} // extern "C"
} // namespace peli::rt::ld