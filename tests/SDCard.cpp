// peli/tests/SDCard.cpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <cstdlib>
#include <peli/ios/sdio/Card.hpp>
#include <peli/util/VIConsole.hpp>
#include <peli/util/VIConsoleStdOut.hpp>

int main() {
  peli::util::VIConsole console(false);

  console.Print("\nMeow! SD Card test:\n");

  // Register the console as stdout
  peli::util::VIConsoleStdOut::Register(console);

  peli::ios::sdio::Card card;

  if (!card.Disk_Available()) {
    std::printf("Disk unavailable\n");
    return EXIT_FAILURE;
  }

  if (peli::ios::IOSError error = card.Disk_Init()) {
    std::printf("Disk_Init() failed: %d\n", error);
    while (true) {
    }
  }

  card.ReserveBlockBuffer();

  peli::u8 sector[512];
  if (peli::ios::IOSError error =
          card.Disk_BlockTransfer(0, 1, sector, false)) {
    std::printf("Disk_BlockTransfer() failed: %d\n", error);
    while (true) {
    }
  }

  std::printf("Success! %s\n", sector + 0x52);
  while (true) {
  }

  return EXIT_SUCCESS;
}
