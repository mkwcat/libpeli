// peli/tests/SDCard.cpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "peli/disk/DeviceTable.hpp"
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
  peli::disk::DeviceTable table = card;

  if (!table.m_available(table.m_object)) {
    std::printf("Disk unavailable\n");
    return EXIT_FAILURE;
  }

  if (int error = table.m_init(table.m_object)) {
    std::printf("Disk_Init() failed: %d\n", error);
    while (true) {
    }
  }

  card.ReserveBlockBuffer();

  peli::u8 sector[512];
  if (int error = table.m_block_transfer(table.m_object, 0, 1, sector, false)) {
    std::printf("Disk_BlockTransfer() failed: %d\n", error);
    while (true) {
    }
  }

  std::printf("Success! %s\n", sector + 0x52);
  while (true) {
  }

  return EXIT_SUCCESS;
}
