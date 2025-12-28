// peli/ios/sdio/Types.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../../cmn/Types.hpp"
#include "../../util/Enum.hpp"

namespace peli::ios::sdio {

constexpr inline size_t DmaBlockSize = 32;

constexpr inline const char *Slot0 = "/dev/sdio/slot0",
                            *Slot1 = "/dev/sdio/slot1",
                            *SlotW = "/dev/sdio/slotw",
                            *HaiWiFi = "/dev/sdio/WiFi",
                            *HaiMLC01 = "/dev/sdio/MLC01";

enum class Cmd : u32 {
  SD_CMD3_SEND_RELATIVE_ADDR = 3,
  SD_CMD6_SWITCH_FUNC = 6,
  SD_ACMD6_WIDE_BUS_SELECT = 6,
  SD_CMD7_SELECT = 7,
  SD_CMD9_SEND_CSD = 9,
  SD_CMD10_SEND_CID = 10,
  SD_CMD13_SEND_STATUS = 13,
  SD_CMD16_BLK_L = 16,
  SD_CMD17_BLK_RD = 17,
  SD_CMD18_MBLK_RD = 18,
  SD_CMD24_BLK_WR = 24,
  SD_CMD25_MBLK_WR = 25,
  SD_ACMD41_SD_SEND_OP_COND = 41,
  SD_ACMD51_SEND_SCR = 51,
  SD_CMD55_APP_CMD = 55,
};

enum class Status : u32 {
  Inserted = 1 << 0,
  TypeMemory = 1 << 16,
  TypeSdhc = 1 << 20,
};

enum class ResponseType : u32 {
  R1 = 1,
  R1B = 2,
};

_PELI_ENUM_ALLOW_BITWISE(Status);

struct HcRegOp {
  /* 0x00 */ u32 reg;
  /* 0x04 */ u32 x04 = 0;
  /* 0x08 */ u32 x08 = 0;
  /* 0x0C */ u32 size;
  /* 0x10 */ u32 value;
  /* 0x14 */ u32 x14 = 0;
};

struct Command {
  /* 0x00 */ Cmd cmd;
  /* 0x04 */ u32 cmd_type;
  /* 0x08 */ ResponseType response_type;
  /* 0x0C */ u32 arg;
  /* 0x10 */ u32 block_count = 0;
  /* 0x14 */ u32 block_size = 0;
  /* 0x18 */ void *buffer = nullptr;
  /* 0x1C */ u32 is_dma = !!buffer;
  /* 0x20 */ u32 x20 = 0;
};

} // namespace peli::ios::sdio
