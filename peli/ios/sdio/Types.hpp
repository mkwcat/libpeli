// peli/ios/sdio/Types.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../../common/Types.hpp"

namespace peli::ios::sdio {

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

struct RegOp {
  /* 0x00 */ u32 reg;
  /* 0x04 */ u32 x04 = 0;
  /* 0x08 */ u32 x08 = 0;
  /* 0x0C */ u32 size;
  /* 0x10 */ u32 val;
  /* 0x14 */ u32 x14 = 0;
};

struct Command {
  /* 0x00 */ Cmd cmd;
  /* 0x04 */ u32 cmd_type;
  /* 0x08 */ u32 response_type;
  /* 0x0C */ u32 arg;
  /* 0x10 */ u32 block_count;
  /* 0x14 */ u32 block_size;
  /* 0x18 */ void *buffer;
  /* 0x1C */ u32 is_dma;
  /* 0x20 */ u32 x20 = 0;
};

} // namespace peli::ios::sdio
