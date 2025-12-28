// peli/ios/sdio/Card.cpp - SD Card reader
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "Card.hpp"
#include "../../host/Host.hpp"
#include "../../hw/SDHostController.hpp"
#include "../../util/Address.hpp"
#include "../Error.hpp"
#include "HcReg.hpp"
#include "Interface.hpp"
#include "Types.hpp"

#if defined(PELI_HOST_IOS)
#include "../../util/CpuCache.hpp"
#endif

namespace peli::ios::sdio {

using Sdhc = hw::SDHostController;

IOSError Card::Device_Init() noexcept {
  u32 reset_output;
  auto request_reset = Ioctl<Reset>();
  auto request_get_status = Ioctl<GetStatus>();
  if (IOSError error = request_reset //
                           .Sync()
                           .CopyOutput(reset_output)
                           .GetError()) {
    return error;
  }
  m_relative_card_address = reset_output >> 16;

  Status status;
  if (IOSError error = request_get_status //
                           .Sync()
                           .CopyOutput(status)
                           .GetError()) {
    return error;
  }
  if (!(status & Status::Inserted)) {
    // No card inserted
    return IOSError::SD_ERROR_REMOVE;
  }
  if (!(status & Status::TypeMemory)) {
    // Not a memory card
    return IOSError::SD_ERROR_REMOVE;
  }
  m_high_capacity = !!(status & Status::TypeSdhc);

  // Enable 4-bit bus width in the interface
  if (IOSError error = HcReg::Mask(GetHandle(), &Sdhc::HC1R, {},
                                   {.DTW = Sdhc::Hc1r::DTW_4BIT})) {
    return error;
  }

  auto request_set_clock = Ioctl<SetClock>(1);

  BufCommand::Request cmd_request[4];
  Select(cmd_request[0]);
  SetBlockLength(cmd_request[1], SectorSize);
  AppCommand(cmd_request[2], m_relative_card_address << 16);
  SetBusWidth(cmd_request[3], 2);
  Deselect(m_request);

  if (IOSError error = request_set_clock //
                           .Sync()
                           .GetError()) {
    return error;
  }
  for (size_t i = 0; i < 4; i++) {
    if (IOSError error = cmd_request[i] //
                             .Sync()
                             .GetError()) {
      return error;
    }
  }

  return IOSError::SD_ERROR_OK;
}

IOSError Card::Device_BlockTransfer(size_t first, size_t count, void *buffer,
                                  bool is_write) noexcept {
  if (count == 0) {
    return IOSError::SD_ERROR_OK;
  }

  m_request.Sync();
  Select(m_request);

  Command command = {
      .cmd = is_write ? Cmd::SD_CMD25_MBLK_WR : Cmd::SD_CMD18_MBLK_RD,
      .cmd_type = 3,
      .response_type = ResponseType::R1,
      .arg = 0,
      .block_size = Device_GetBlockSize(),
  };
  IOSError error = IOSError::IOS_ERROR_OK;
  bool use_temp_buffer = false;
  if (m_block_buffer) {
    command.buffer = m_block_buffer;
    use_temp_buffer = true;
  } else if (util::IsAligned(DmaBlockSize, buffer)) {
    command.buffer = buffer;
  } else {
    return IOSError::IOS_ERROR_INVALID;
  }

  const bool high_capacity = m_high_capacity;

  // Check error from Select request
  if (error = m_request.Sync().GetError(); error) {
    Deselect(m_request);
    return error;
  }

  do {
    command.block_count = count <= MaxTransferSize ? count : MaxTransferSize;

    if (is_write) {
      if (use_temp_buffer) {
        __builtin_memcpy(command.buffer, buffer,
                         command.block_count * command.block_size);
      }
#if defined(PELI_HOST_IOS)
      util::CpuCache::DcFlush(command.buffer,
                              command.block_count * command.block_size);
#endif
    }
    command.arg = high_capacity ? first * command.block_size : first;

    if (error = SendCommand(m_request, command) //
                    .Sync()
                    .GetError();
        error) {
      break;
    }

    if (!is_write) {
#if defined(PELI_HOST_IOS)
      util::CpuCache::DcInvalidate(use_temp_buffer ? command.buffer : buffer,
                                   command.block_count * command.block_size);
#endif
      if (use_temp_buffer) {
        // Copy previous transfer's data
        __builtin_memcpy(buffer, command.buffer,
                         command.block_count * command.block_size);
      }
    }

    buffer =
        static_cast<u8 *>(buffer) + command.block_count * command.block_size;
    if (!use_temp_buffer) {
      command.buffer = buffer;
    }
  } while (--count);

  Deselect(m_request);
  return error;
}

void Card::ReserveBlockBuffer() {
  m_block_buffer =
      host::Alloc(DmaBlockSize, MaxTransferSize * Device_GetBlockSize());
}

} // namespace peli::ios::sdio