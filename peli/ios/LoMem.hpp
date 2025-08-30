// peli/ios/LoMem.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../common/Types.h"
#include "../ios/di/Types.hpp"

namespace peli::ios {

enum class VideoMode : u32 {
  Ntsc = 0,
  Pal = 1,
  MPal = 2,
  Debug = 3,
  DebugPal = 4,
  Eurgb60 = 5,
};

struct OSContext {
  /* 0x000 */ u32 gprs[32];
  /* 0x080 */ u32 cr;
  /* 0x084 */ u32 lr;
  /* 0x088 */ u32 ctr;
  /* 0x08C */ u32 xer;
  /* 0x090 */ f64 fprs[32];
  /* 0x190 */ u32 fpscr_pad;
  /* 0x194 */ u32 fpscr;
  /* 0x198 */ u32 srr0;
  /* 0x19C */ u32 srr1;
  /* 0x1A0 */ u16 mode;
  /* 0x1A2 */ u16 state;
  /* 0x1A4 */ u32 gqrs[8];
  /* 0x1C4 */ u32 hid2; // Originally psf_pad
  /* 0x1C8 */ f64 psfs[32];
};

static_assert(sizeof(OSContext) == 0x2C8);

struct OSModuleLink;
struct OSThread;

struct OSThreadQueue {
  /* 0x00 */ OSThread *head;
  /* 0x04 */ OSThread *tail;
};

struct LoMem {
  /**
   * Disk ID of the currently running software or inserted disc.
   */
  /* 0x0000 */ di::DiskId disk_id;

  struct SystemInfo {
    enum class BootCode : u32 {
      Normal = 0x0D15EA5E // Why "disease"?
    };

    /**
     * Nintendo Standard Boot Code.
     */
    /* 0x0020 */ BootCode boot_code = BootCode::Normal;

    /**
     * Version set by apploader.
     */
    /* 0x0024 */ u32 version = 1;

    /**
     * Physical MEM1 size.
     */
    /* 0x0028 */ u32 mem1_size = 0x01800000;

    /**
     * Production Board Model.
     */
    /* 0x002C */ u32 board_model;

    /**
     * Arena Low.
     */
    /* 0x0030 */ u32 arena_low;

    /**
     * Arena High.
     */
    /* 0x0034 */ u32 arena_high;

    /**
     * Effective pointer to start of FST.
     */
    /* 0x0038 */ void *fst_start;

    /**
     * Maximum size of FST.
     */
    /* 0x003C */ u32 fst_size;
  } system_info;

  static_assert(sizeof(SystemInfo) == 0x20);

  struct Debugger {
    /**
     * DB Enabled.
     */
    /* 0x0040 */ u32 enabled;

    /**
     * DB Marked Exception Mask.
     */
    /* 0x0044 */ u32 exception_mask;

    /**
     * DB Exception Destination (effective address).
     */
    /* 0x0048 */ u32 exception_destination;

    /**
     * DB Return Address.
     */
    /* 0x004C */ u32 return_address;

    /* 0x0050 */ _PELI_PAD(0x0050, 0x0060);

    /**
     * OSDBIntegrator Debugger Hook; Hook to be jumped to by debugged
     * exceptions, but is disabled in production software. If nothing is
     * written to it, SDK titles will write the 0x20 bytes of instructions
     * here:
     * 0x60: li    r5, 0x40;       # r5 = &gLoMem.debugger
     * 0x64: mflr  r3;             # r3 = __builtin_return_address(0)
     * 0x68: stw   r3, 0x0C(r5);   # r5->returnAddress = r3
     * 0x6C: lwz   r3, 0x08(r5);   # r3 = r5->exceptionDestination
     * 0x70: oris  r3, r3, 0x8000; # r3 |= 0x8000 (phys to effective)
     * 0x74: mtlr  r3;             # __mtlr(r3)
     * 0x78: li    r3, 0x30;       # r3 = 0x30
     * 0x7C: mtmsr r3;             # __mtmsr(r3)
     * 0x80: blr;                  # call LR set at 0x74
     */
    /* 0x0060 */ u32 exception_code[0x24 / 4];

    /* 0x0084 */ _PELI_PAD(0x0084, 0x00C0);
  } debugger;

  static_assert(sizeof(Debugger) == 0x80);

  struct ThreadInfo {
    /**
     * Real mode pointer to current OSContext instance.
     */
    /* 0x00C0 */ OSContext *rm_current_context;

    /**
     * User Interrupt Mask.
     */
    /* 0x00C4 */ u32 user_interrupt_mask;

    /**
     * Dolphin/Revolution OS Interrupt Mask.
     */
    /* 0x00C8 */ u32 system_interrupt_mask;

    /**
     * Current TV Video Mode.
     */
    /* 0x00CC */ VideoMode video_mode;

    /**
     * GameCube ARAM size.
     */
    /* 0x00D0 */ u32 aram_size;

    /**
     * Effective pointer to current OSContext instance.
     */
    /* 0x00D4 */ OSContext *em_current_context;

    /**
     * Effective pointer to OSContext to save FPRs to (null if floating
     * point mode hasn't been used since the last interrupt).
     */
    /* 0x00D8 */ OSContext *floating_point_context;

    /**
     * Effective pointer to the earliest and latest created OSThread.
     */
    /* 0x00DC */ OSThreadQueue thread_list;

    /**
     * Effective pointer to current OSThread.
     */
    /* 0x00E4 */ OSThread *current_thread;

    /**
     * Debug Monitor Size (if present).
     */
    /* 0x00E8 */ u32 debug_monitor_size;

    /**
     * Debugger Monitor Address (if present, effective address).
     */
    /* 0x00EC */ u32 debug_monitor_address;

    /**
     * Simulated Memory Size (always 0x01800000 on retail).
     */
    /* 0x00F0 */ u32 simulated_memory_size;

    /**
     * Effective pointer to data read from partition's bi2.bin, set by
     * apploader, or the emulated bi2.bin created by the NAND Boot Program
     */
    /* 0x00F4 */ struct BI2 {
      _PELI_PAD(0x0, 0x30);
      u32 dual_layer_value;
      _PELI_PAD(0x30, 0x2000);
    } *bi2;

    /**
     * Console Bus Speed.
     */
    /* 0x00F8 */ u32 bus_speed = 0x0E7BE2C0;

    /**
     * Console CPU Speed.
     */
    /* 0x00FC */ u32 cpu_speed = 0x2B73A840;
  } thread_info;

  static_assert(sizeof(ThreadInfo) == 0x40);

  /**
   * PowerPC exception handlers.
   */
  struct ExceptionHandlers {
    /* 0x0100 */ u32 system_reset[0x100 / 4];
    /* 0x0200 */ u32 machine_check[0x100 / 4];
    /* 0x0300 */ u32 data_storage_interrupt[0x100 / 4];
    /* 0x0400 */ u32 instruction_storage_interrupt[0x100 / 4];
    /* 0x0500 */ u32 external_interrupt[0x100 / 4];
    /* 0x0600 */ u32 alignment[0x100 / 4];
    /* 0x0700 */ u32 program[0x100 / 4];
    /* 0x0800 */ u32 floating_point_unavailable[0x100 / 4];
    /* 0x0900 */ u32 decrementer[0x100 / 4];
    /* 0x0A00 */ _PELI_PAD(0x0A00, 0x0C00);
    /* 0x0C00 */ u32 system_call[0x100 / 4];
    /* 0x0D00 */ u32 trace[0x100 / 4];
    /* 0x0E00 */ u32 floating_point_assist[0x100 / 4];
    /* 0x0F00 */ u32 performance_monitor[0x100 / 4];
    /* 0x1000 */ _PELI_PAD(0x1000, 0x1300);
    /* 0x1300 */ u32 instruction_address_breakpoint[0x100 / 4];
    /* 0x1400 */ u32 reserved[0x100 / 4];
    /* 0x1500 */ _PELI_PAD(0x1500, 0x1700);
    /* 0x1700 */ u32 thermal_management[0x100 / 4];
  } exception_handlers;

  static_assert(sizeof(ExceptionHandlers) == 0x1700);

  /**
   * Not used or cleared by Dolphin or Revolution OS. Frequently used by
   * modders for data such as loader stubs or codehandlers.
   */
  /* 0x1800 */ u8 loader_stub[0x3000 - 0x1800];

  struct IOSVersion {
    /**
     * Major version number.
     */
    u16 number;

    /**
     * Minor version number / title revision.
     */
    u16 revision;
  };

  struct OSGlobals {
    /**
     * Exception Vector Area.
     */
    /* 0x3000 */ u32 exception_vector_area[16];

    /**
     * __OSInterrupt table.
     */
    /* 0x3040 */ u32 os_interrupt_table[32];

    /**
     * EXI Probe start times, for both channels 0 and 1.
     */
    /* 0x30C0 */ u32 exi_probe_start_time[2];

    /**
     * Effective pointer to the first loaded REL file.
     */
    /* 0x30C8 */ OSModuleLink *module_head;

    /**
     * Effective pointer to the last loaded REL file.
     */
    /* 0x30CC */ OSModuleLink *module_tail;

    /**
     * Effective pointer to a REL module name table, or 0. Added to the name
     * offset in each REL file.
     */
    /* 0x30D0 */ void *module_name_table;

    /* 0x30D4 */ _PELI_PAD(0x30D4, 0x30D8);

    /**
     * System time, measured as time since January 1st 2000 in units of
     * 1/40500000th of a second.
     */
    /* 0x30D8 */ s64 system_time;

    /* 0x30E0 */ _PELI_PAD(0x30E0, 0x30E4);

    /**
     * __OSPADButton. Apploader puts button state of GCN port 4 at game
     * start here for Gamecube NR disc support.
     */
    /* 0x30E4 */ u16 pad_button;

    /**
     * DVD Device Code Address.
     */
    /* 0x30E6 */ u16 dvd_device_code;

    /**
     * Debug-related info.
     */
    /* 0x30E8 */ u32 debug_flag;

    /* 0x30EC */ _PELI_PAD(0x30EC, 0x30F0);

    /**
     * DOL Execute Parameters.
     */
    /* 0x30F0 */ u32 dol_exec_param;

    /* 0x30F4 */ _PELI_PAD(0x30F4, 0x3100);

    /**
     * Physical MEM1 size.
     */
    /* 0x3100 */ u32 mem1_size_physical;

    /**
     * Simulated MEM1 size.
     */
    /* 0x3104 */ u32 mem1_size_simulated;

    /**
     * MEM1 end (effective address).
     */
    /* 0x3108 */ u32 mem1_end;

    /**
     * MEM1 Arena Start (start of usable memory by the software, effective
     * address).
     */
    /* 0x310C */ u32 mem1_arena_start;

    /**
     * MEM1 Arena End (end of usable memory by the software,
     * effective address).
     */
    /* 0x3110 */ u32 mem1_arena_end;

    /* 0x3114 */ _PELI_PAD(0x3114, 0x3118);

    /**
     * Physical MEM2 size.
     */
    /* 0x3118 */ u32 mem2_size_physical;

    /**
     * Simulated MEM2 size.
     */
    /* 0x311C */ u32 mem2_size_simulated;

    /**
     * End of MEM2 addressable to PowerPC (effective address).
     */
    /* 0x3120 */ u32 mem2_end;

    /**
     * MEM2 Arena Start (start of usable memory by the software, effective
     * address).
     */
    /* 0x3124 */ u32 mem2_arena_start;

    /**
     * MEM2 Arena End (end of usable memory by the software, effective
     * address).
     */
    /* 0x3128 */ u32 mem2_arena_end;

    /* 0x312C */ _PELI_PAD(0x312C, 0x3130);

    /**
     * IOS IPC Buffer Start (effective address).
     */
    /* 0x3130 */ u32 ipc_heap_start;

    /**
     * IOS IPC Buffer End (effective address).
     */
    /* 0x3134 */ u32 ipc_heap_end;

    /**
     * Hollywood version.
     */
    /* 0x3138 */ u32 hollywood_version;

    /* 0x313C */ _PELI_PAD(0x313C, 0x3140);

    /**
     * Current IOS version.
     */
    /* 0x3140 */ IOSVersion ios_version;

    /**
     * IOS build date (e.g. 0x00062507 = 06/25/07 = June 25, 2007)
     */
    /* 0x3144 */ u32 ios_build_date;

    /**
     * IOS DI Heap Start (effective address).
     */
    /* 0x3148 */ u32 ios_di_heap_start;

    /**
     * IOS DI Heap End (effective address).
     */
    /* 0x314C */ u32 ios_di_heap_end;

    /* 0x3150 */ _PELI_PAD(0x3150, 0x3158);

    /**
     * GDDR Vendor Code.
     */
    /* 0x3158 */ u32 gddr_vendor_code;

    /**
     * During the boot process, u32 0x315c is first set to 0xdeadbeef by IOS
     * in the IOS_LaunchElf syscall. The value is set to 0x80 by the NAND
     * Boot Program to indicate that it was loaded by the boot program (and
     * probably 0x81 by apploaders).
     */
    /* 0x315C */ u8 boot_program_flag;

    /**
     * "Enable legacy DI" mode? 0x81 = false, anything else means true
     * (though typically set to 0x80). Required to be set when loading the
     * Gamecube apploader.
     */
    /* 0x315D */ u8 legacy_di_flag;

    /**
     * "Devkit boot program version", written to by the system menu. The
     * value carries over to disc games. 0x0113 appears to mean v1.13.
     */
    /* 0x315E */ u16 boot_program_version;

    /**
     * Init semaphore (System Menu main() waits for this to clear).
     */
    /* 0x3160 */ u32 init_semaphore;

    /**
     * GC (MIOS) mode flag, set to 1 by boot2 when MIOS triggers a shutdown.
     * The System Menu reads this and turns off the console if it is set to
     * 1 and state.dat is set appropriately.
     */
    /* 0x3164 */ u32 mios_flag;

    /* 0x3168 */ _PELI_PAD(0x3168, 0x3180);

    /**
     * Game Code (e.g. 'RSPE' for Wii Sports). Used by DWC and NWC24.
     */
    /* 0x3180 */ char game_code[4];

    enum class AppType : u8 {
      NotSet = 0,

      Disc = 0x80,
      Nand = 0x81,
    };

    /**
     * Application type. 0x80 for disc games, 0x81 for channels.
     */
    /* 0x3184 */ AppType app_type;

    /* 0x3185 */ _PELI_PAD(0x3185, 0x3186);

    /**
     * Application type 2. Appears to be set to the when a game loads a
     * channel (e.g. Mario Kart Wii loading the region select menu will
     * result in this being 0x80 from the disc and the main application type
     * being 0x81, or the Wii Fit channel transitioning to the Wii Fit disc
     * will result in this being 0x81 and the main type being 0x80).
     */
    /* 0x3186 */ AppType launcher_app_type;

    /* 0x3187 */ _PELI_PAD(0x3187, 0x3188);

    /**
     * Minimum supported IOS version.
     */
    /* 0x3188 */ IOSVersion ios_minimum_version;

    /**
     * Launch Code.
     */
    /* 0x318C */ u32 launch_code;

    /**
     * Return Code.
     */
    /* 0x3190 */ u32 return_code;

    /**
     * While reading a disc, the system menu reads the first partition table
     * (0x20 bytes from 0x00040020) and stores a pointer to the data
     * partition entry. When launching the disc game, it copies the
     * partition type to 0x3194. The partition type for data partitions is
     * 0, so typically this location always has 0.
     */
    /* 0x3194 */ u32 partition_type;

    /**
     * While reading a disc, the system menu reads the first partition table
     * (0x20 bytes from 0x00040020) and stores a pointer to the data
     * partition entry. When launching the disc game, it copies the
     * partition offset to 0x3198.
     */
    /* 0x3198 */ u32 partition_offset;

    enum class DualLayerValue {
      NotSet = 0,

      SingleLayer = 0x80,
      DualLayer = 0x81,
    };

    /**
     * Set by the apploader to 0x80 for single-layer discs and 0x81 for
     * dual-layer discs (determined by whether 0x7ed40000 is the value at
     * offset 0x30 in the partition's bi2.bin; it seems that that value is 0
     * for single-layer discs). Early titles' apploaders do not set it at
     * all, leaving the value as 0. This controls the out-of-bounds Error
     * #001 read for titles that do make such a read: they try to read at
     * 0x7ed40000 for dual-layer discs and 0x460a0000 for single-layer
     * discs.
     */
    /* 0x319C */ DualLayerValue dual_layer_value;
  } os_globals;

  static_assert(sizeof(OSGlobals) == 0x1A0);

  /* 0x31A0 */ _PELI_PAD(0x31A0, 0x3400);

  /**
   * BS1 Boot Code.
   */
  /* 0x3400 */ u32 bs1_boot_code[0x400 / 4];

  /* 0x3800 */ _PELI_PAD(0x3800, 0x3F00);
};

static_assert(sizeof(LoMem) == 0x3F00);

static LoMem &g_lo_mem = *reinterpret_cast<LoMem *>(0x80000000);
static LoMem &g_lo_mem_uncached = *reinterpret_cast<LoMem *>(0xC0000000);
static LoMem &g_lo_mem_real_mode = *reinterpret_cast<LoMem *>(0x00000001 - 1);

} // namespace peli::ios