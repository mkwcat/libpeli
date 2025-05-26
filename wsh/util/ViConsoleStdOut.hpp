// wsh/util/VIConsoleStdOut.hpp - Pipe stdout to an instance of VIConsole
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#if defined(WSH_NEWLIB)

#include "VIConsole.hpp"
#include <stddef.h>

struct _reent;
struct stat;

namespace wsh::util {

class VIConsoleStdOut {
public:
  static void Register(VIConsole &console);

private:
  static int SysWrite(struct _reent *r, void *fd, const char *ptr, size_t len);
  static int SysFstat(struct _reent *r, void *fd, struct stat *st);

  static VIConsole *s_console;
};

} // namespace wsh::util

#endif