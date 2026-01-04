// peli/log/VideoConsoleStdOut.hpp - Pipe stdout to an instance of VideoConsole
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../host/Config.h"

#if defined(PELI_NEWLIB)

#include "VideoConsole.hpp"
#include <stdio.h>

struct _reent;
struct stat;

namespace peli::log {

class VideoConsoleStdOut {
public:
  static void Register(VideoConsole &console);
  static void Deregister();

private:
  static ::ssize_t SysWrite(struct _reent *r, void *fd, const char *ptr,
                            __SIZE_TYPE__ len);
  static int SysFstat(struct _reent *r, void *fd, struct stat *st);

  static VideoConsole *s_console;
  static const void *s_default_stdout;
  static const void *s_default_stderr;
};

} // namespace peli::log

#endif