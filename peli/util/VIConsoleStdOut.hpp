// peli/util/VIConsoleStdOut.hpp - Pipe stdout to an instance of VIConsole
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../host/Config.h"

#if defined(PELI_NEWLIB)

#include "VIConsole.hpp"
#include <stdio.h>

struct _reent;
struct stat;

namespace peli::util {

class VIConsoleStdOut {
public:
  static void Register(VIConsole &console);
  static void Deregister();

private:
  static ::ssize_t SysWrite(struct _reent *r, void *fd, const char *ptr,
                            __SIZE_TYPE__ len);
  static int SysFstat(struct _reent *r, void *fd, struct stat *st);

  static VIConsole *s_console;
  static const void *s_default_stdout;
  static const void *s_default_stderr;
};

} // namespace peli::util

#endif