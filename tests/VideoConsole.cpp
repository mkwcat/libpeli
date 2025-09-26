// VideoConsole.cpp - Testing VIConsole

#include <cstdio>
#include <peli/util/VIConsole.hpp>
#include <peli/util/VIConsoleStdOut.hpp>

int main(int argc, char **argv) {
  peli::util::VIConsole console(false);

  console.Print("\nMeow! Welcome to libpeli!!!\n");

  // Register the console as stdout
  peli::util::VIConsoleStdOut::Register(console);

  std::printf("Arden's favorite number is %d\n", 4);

  return 0;
}