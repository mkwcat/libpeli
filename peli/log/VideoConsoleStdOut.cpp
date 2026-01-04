// peli/log/VideoConsoleStdOut.cpp - Pipe stdout to an instance of VideoConsole
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "VideoConsoleStdOut.hpp"

#if defined(PELI_NEWLIB)

#include <stdio.h>
#include <sys/iosupport.h>
#include <sys/reent.h>
#include <sys/stat.h>

namespace peli::log {

VideoConsole *VideoConsoleStdOut::s_console = nullptr;
const void *VideoConsoleStdOut::s_default_stdout = nullptr;
const void *VideoConsoleStdOut::s_default_stderr = nullptr;

void VideoConsoleStdOut::Register(VideoConsole &console) {
  bool registered = !!s_console;
  s_console = &console;

  static const devoptab_t s_devoptab = {
      .name = "stdout",
      .structSize = 0,
      .open_r = nullptr,
      .close_r = nullptr,
      .write_r = VideoConsoleStdOut::SysWrite,
      .read_r = nullptr,
      .seek_r = nullptr,
      .fstat_r = VideoConsoleStdOut::SysFstat,
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

  if (!registered) {
    s_default_stdout = devoptab_list[STD_OUT];
    s_default_stderr = devoptab_list[STD_ERR];
  }

  devoptab_list[STD_OUT] = &s_devoptab;
  devoptab_list[STD_ERR] = &s_devoptab;
}

void VideoConsoleStdOut::Deregister() {
  if (!s_console) {
    return;
  }

  ::fflush(stdout);
  devoptab_list[STD_OUT] = static_cast<const devoptab_t *>(s_default_stdout);
  ::fflush(stderr);
  devoptab_list[STD_ERR] = static_cast<const devoptab_t *>(s_default_stderr);
  s_console = nullptr;
}

::ssize_t VideoConsoleStdOut::SysWrite([[maybe_unused]] struct _reent *r,
                                       [[maybe_unused]] void *fd,
                                       const char *ptr, __SIZE_TYPE__ len) {
  s_console->Print(ptr, len);
  return len;
}

int VideoConsoleStdOut::SysFstat([[maybe_unused]] struct _reent *r,
                                 [[maybe_unused]] void *fd, struct stat *st) {
  *st = {};
  st->st_mode = S_IFCHR;
  return 0;
}

} // namespace peli::log

#endif // PELI_NEWLIB
