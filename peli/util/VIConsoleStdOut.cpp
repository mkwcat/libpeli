// peli/util/VIConsoleStdOut.cpp - Pipe stdout to an instance of VIConsole
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "../host/Config.h"

#if defined(PELI_NEWLIB)

#include "VIConsoleStdOut.hpp"
#include <sys/iosupport.h>
#include <sys/reent.h>
#include <sys/stat.h>

namespace peli::util {

VIConsole *VIConsoleStdOut::s_console = nullptr;

void VIConsoleStdOut::Register(VIConsole &console) {
  s_console = &console;

  static devoptab_t s_devoptab = {
      .name = "stdout",
      .structSize = 0,
      .open_r = nullptr,
      .close_r = nullptr,
      .write_r = VIConsoleStdOut::SysWrite,
      .read_r = nullptr,
      .seek_r = nullptr,
      .fstat_r = VIConsoleStdOut::SysFstat,
      .stat_r = nullptr,
      .link_r = nullptr,
      .unlink_r = nullptr,
      .chdir_r = nullptr,
      .rename_r = nullptr,
      .mkdir_r = nullptr,
      .dirStateSize = 0,
      .diropen_r = nullptr,
      .dirreset_r = nullptr,
      .dirnext_r = nullptr,
      .dirclose_r = nullptr,
      .statvfs_r = nullptr,
      .ftruncate_r = nullptr,
      .fsync_r = nullptr,
      .deviceData = nullptr,
      .chmod_r = nullptr,
      .fchmod_r = nullptr,
      .rmdir_r = nullptr,
      .lstat_r = nullptr,
      .utimes_r = nullptr,
      .fpathconf_r = nullptr,
      .pathconf_r = nullptr,
      .symlink_r = nullptr,
      .readlink_r = nullptr,
  };

  devoptab_list[STD_OUT] = &s_devoptab;
  devoptab_list[STD_ERR] = &s_devoptab;
}

int VIConsoleStdOut::SysWrite([[maybe_unused]] struct _reent *r,
                              [[maybe_unused]] void *fd, const char *ptr,
                              size_t len) {
  s_console->Print(ptr, len);
  return len;
}

int VIConsoleStdOut::SysFstat([[maybe_unused]] struct _reent *r,
                              [[maybe_unused]] void *fd, struct stat *st) {
  *st = {};
  st->st_mode = S_IFCHR;
  return 0;
}

} // namespace peli::util

#endif