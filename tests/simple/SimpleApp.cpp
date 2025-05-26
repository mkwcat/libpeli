// SimpleApp.cpp - Testing wsh

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wsh/hw/Wood.hpp>
#include <wsh/ios/ipc/Low.hpp>
#include <wsh/runtime/Thread.hpp>
#include <wsh/util/VIConsole.hpp>
#include <wsh/util/VIConsoleStdOut.hpp>

void *ThreadEntryTest(void *) {
  printf("Thread Entry Test\n");

  s32 fd = wsh::ios::ipc::low::IOS_Open("/meow", 0);
  return (void *)fd;
}

int main(int argc, char **argv) {
  wsh::util::VIConsole console(true);

  console.Print("\nmeow!\n");

  // Register the console as stdout
  wsh::util::VIConsoleStdOut::Register(console);

  wsh::ios::ipc::low::Init();

  wsh::runtime::Thread thread(ThreadEntryTest, nullptr, nullptr, 0x1000, 80,
                              false);
  int num = 4;
  thread.Join((void **)&num);

  printf("Thread exited and joined with Arden's favorite number %d\n", num);

  s32 fd = wsh::ios::ipc::low::IOS_Open("/dev/es", 0);
  printf("Opened /dev/es with fd %d\n", fd);

  if (fd >= 0) {
    s32 ret = wsh::ios::ipc::low::IOS_Ioctl(fd, 0, nullptr, 0, nullptr, 0);
    if (ret < 0) {
      printf("IOS_Ioctl failed with error %d\n", ret);
    } else {
      printf("IOS_Ioctl succeeded with return value %d\n", ret);
    }
  }

  while (true) {
  }

  return 0;
}