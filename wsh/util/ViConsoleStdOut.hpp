#pragma once

#include "ViConsole.hpp"
#include <cstddef>

struct _reent;
struct stat;

namespace wsh::util {

class ViConsoleStdOut {
public:
  static void Register(ViConsole &console);

private:
  static int SysWrite(struct _reent *r, void *fd, const char *ptr,
                          std::size_t len);
  static int SysFstat(struct _reent *r, void *fd, struct stat *st);

  static ViConsole *s_console;
};

} // namespace wsh::util