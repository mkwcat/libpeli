// wsh/util/ViConsoleStdOut.hpp - Pipe stdout to an instance of ViConsole
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#if defined(WSH_NEWLIB)

#include "ViConsole.hpp"
#include <stdio.h>

struct _reent;
struct stat;

namespace wsh::util {

class ViConsoleStdOut {
public:
  static void Register(ViConsole &console);

private:
  static ssize_t SysWrite(struct _reent *r, void *fd, const char *ptr,
                          size_t len);
  static ssize_t SysFstat(struct _reent *r, void *fd, struct stat *st);

  static ViConsole *s_console;
};

} // namespace wsh::util

#endif