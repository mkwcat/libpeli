// VideoConsole.cpp - Testing VideoConsole

#include <cstdio>
#include <peli/log/VideoConsole.hpp>
#include <peli/log/VideoConsoleStdOut.hpp>

int main(int argc, char **argv) {
  peli::log::VideoConsole console(false);

  console.Print("\nMeow! Welcome to libpeli!!!\n");

  // Register the console as stdout
  peli::log::VideoConsoleStdOut::Register(console);

  std::printf("Arden's favorite number is %d\n", 4);

  return 0;
}