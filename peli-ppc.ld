/* peli-ppc.ld - libpeli PowerPC linker script
 *   Written by mkwcat
 *
 * Copyright (c) 2025-2026 mkwcat
 * SPDX-License-Identifier: MIT */

OUTPUT_FORMAT("elf32-powerpc", "elf32-powerpc", "elf32-powerpc")
OUTPUT_ARCH(powerpc:common)
EXTERN(_start)
ENTRY(_start)

PHDRS
{
  stub       PT_LOAD FLAGS(0x5); /* RX */
  init       PT_LOAD FLAGS(0x5); /* RX */
  extab      PT_LOAD FLAGS(0x4); /* R */
  extabindex PT_LOAD FLAGS(0x4); /* R */
  text       PT_LOAD FLAGS(0x5); /* RX */
  ctors      PT_LOAD FLAGS(0x4); /* R */
  dtors      PT_LOAD FLAGS(0x4); /* R */
  rodata     PT_LOAD FLAGS(0x4); /* R */
  data       PT_LOAD FLAGS(0x6); /* RW */
  bss        PT_LOAD FLAGS(0x6); /* RW */
  sdata      PT_LOAD FLAGS(0x6); /* RW */
  sbss       PT_LOAD FLAGS(0x6); /* RW */
  sdata2     PT_LOAD FLAGS(0x6); /* RW */
  sbss2      PT_LOAD FLAGS(0x6); /* RW */
}

MEMORY
{
  stub (rx)  : ORIGIN = 0x80003400, LENGTH = 0x400

  /* Standard range allowed by official apploader */
  /* mem1 (rwx) : ORIGIN = 0x80004000, LENGTH = 0x8FC000 */
  mem1 (rwx) : ORIGIN = 0x80004000, LENGTH = 0x017FC000

  mem2 (rwx) : ORIGIN = 0x90000020, LENGTH = 0x03600000
}

SECTIONS
{
  .stub : ALIGN(0x20) { 
    *(.stub)
    *(.stub.*)
  } > stub : stub = 0

  .init : ALIGN(0x20) {
    KEEP(*(.init))
    KEEP(*(.init.*))
    KEEP(*(.fini))
    . = ALIGN(0x20);
  } > mem1 : init = 0

  extab : ALIGN(0x20) {
    *(extab)
    *(.gcc_except_table)
    . = ALIGN(0x20);
  } > mem1 : extab = 0

  extabindex : ALIGN(0x20) {
    *(extabindex)
    *(.gcc_except_table.*)
    . = ALIGN(0x20);
  } > mem1 : extabindex = 0

  .text : ALIGN(0x20) {
    *(.text)
    *(.text.*)
    *(.gnu.linkonce.t.*)
    *(.gnu.warning.*)
    . = ALIGN(0x20);
  } > mem1 : text = 0

  .ctors : ALIGN(0x20) {
    KEEP(*crtbegin.o(.ctors))
	KEEP(*crtbegin?.o(.ctors))
	KEEP(*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors))
	KEEP(*(SORT(.ctors.*)))
	KEEP(*(.ctors))
    . = ALIGN(0x20);
  } > mem1 : ctors = 0

  .dtors : ALIGN(0x20) {
    KEEP(*crtbegin.o(.dtors))
 	KEEP(*crtbegin?.o(.dtors))
 	KEEP(*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors))
 	KEEP(*(SORT(.dtors.*)))
 	KEEP(*(.dtors))
    . = ALIGN(0x20);
  } > mem1 : dtors = 0

  .rodata : ALIGN(0x20) {
    __rodata_start = .;
    *(.rodata)
    *(.rodata.*)
    *(.gnu.linkonce.r.*)

    . = ALIGN(0x4);
	PROVIDE_HIDDEN(__preinit_array_start = .);
	KEEP(*(.preinit_array))
	PROVIDE_HIDDEN(__preinit_array_end = .);

	. = ALIGN(0x4);
	PROVIDE_HIDDEN(__init_array_start = .);
	KEEP(*(SORT(.init_array.*)))
	KEEP(*(.init_array))
	PROVIDE_HIDDEN(__init_array_end = .);

	. = ALIGN(0x4);
	PROVIDE_HIDDEN(__fini_array_start = .);
	KEEP(*(SORT(.fini_array.*)))
	KEEP(*(.fini_array))
	PROVIDE_HIDDEN(__fini_array_end = .);

    . = ALIGN(0x20);
    __rodata_end = .;
  } > mem1 : rodata = 0

  .data : ALIGN(0x20) {
    __data_start = .;
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d.*)
    . = ALIGN(0x20);
    __data_end = .;
  } > mem1 : data = 0

  .bss : ALIGN(0x20) {
    __bss_start = .;
    *(.bss)
    *(.bss.*)
    . = ALIGN(0x20);
    __bss_end = .;
  } > mem1 : bss = 0

  .sdata : ALIGN(0x20) {
    __sdata_start = .;
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    . = ALIGN(0x20);
    __sdata_end = .;
  } > mem1 : sdata = 0

  .sbss : ALIGN(0x20) {
    __sbss_start = .;
    *(.sbss)
    *(.sbss.*)
    . = ALIGN(0x20);
    __sbss_end = .;
  } > mem1 : sbss = 0

  .sdata2 : ALIGN(0x20) {
    __sdata2_start = .;
    *(.sdata2)
    *(.sdata2.*)
    *(.gnu.linkonce.s2.*)
    . = ALIGN(0x20);
    __sdata2_end = .;
  } > mem1 : sdata2 = 0

  .sbss2 : ALIGN(0x20) {
    __sbss2_start = .;
    *(.sbss2)
    *(.sbss2.*)
    . = ALIGN(0x20);
    __sbss2_end = .;
  } > mem1 : sbss2 = 0

  /* Heap arena for MEM1 */
  .mem1_arena (NOLOAD) : ALIGN(0x20) {
    __mem1_arena_start = .;
    . = ORIGIN(mem1) + LENGTH(mem1);
    __mem1_arena_end = .;
  } > mem1

  /* Heap arena for MEM2 */
  .mem2_arena (NOLOAD) : ALIGN(0x20) {
    __mem2_arena_start = .;
    . = ORIGIN(mem2) + LENGTH(mem2);
    __mem2_arena_end = .;
  } > mem2
}