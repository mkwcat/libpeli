// SimpleApp.cpp - Testing libpeli

#include <peli/hw/Wood.hpp>
#include <peli/ios/fs/Interface.hpp>
#include <peli/ios/low/Ipc.hpp>
#include <peli/rt/Thread.hpp>
#include <peli/log/VideoConsole.hpp>
#include <peli/log/VideoConsoleStdOut.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *ThreadEntryTest(void *) {
  printf("Thread Entry Test\n");

  peli::s32 fd = peli::ios::low::IOS_Open("/meow", 0);
  return (void *)fd;
}

int testIoctl() {
  return peli::ios::fs::Interface::ReadDir::Request(
             -1, "/shared2", 4, 4 * sizeof(peli::ios::fs::NodeName))
      .Sync()
      .GetResult();
}

int main() {
  peli::log::VideoConsole console(false);

  console.Print("\nmeow!\n");

  // Register the console as stdout
  peli::log::VideoConsoleStdOut::Register(console);

  peli::rt::Thread thread(ThreadEntryTest, nullptr, nullptr, 0x1000, 80,
                               false);
  int num = 4;
  thread.Join((void **)&num);

  printf("Thread exited and joined with Arden's favorite number %d\n", num);
  printf("Thread exited and joined with Arden's favorite number %d\n", num);
  printf("Thread exited and joined with Arden's favorite number %d\n", num);
  printf("Thread exited and joined with Arden's favorite number %d\n", num);

  fflush(stdout);

  peli::s32 fd = peli::ios::low::IOS_Open("/dev/es", 0);
  printf("Opened /dev/es with fd %d\n", fd);

  if (fd >= 0) {
    peli::s32 ret = peli::ios::low::IOS_Ioctl(fd, 0, nullptr, 0, nullptr, 0);
    if (ret < 0) {
      printf("IOS_Ioctl failed with error %d\n", ret);
    } else {
      printf("IOS_Ioctl succeeded with return value %d\n", ret);
    }
  }

  printf("peli::ios::fs::Interface::ReadDir::Request %d\n", testIoctl());

  while (true) {
  }

  return 0;
}