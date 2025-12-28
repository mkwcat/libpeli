// peli/ios/sdio/Card.hpp - SD Card reader
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../../util/NoConstruct.hpp"
#include "../Error.hpp"
#include "../Resource.hpp"
#include "Interface.hpp"

namespace peli::ios::sdio {

/**
 * Disk interface for interacting with an SD Card over IOS.
 */
class Card : public Resource<Interface>, Interface {
public:
  static constexpr u32 SectorSize = 512, MaxTransferSize = 4;

  explicit Card(util::NoConstruct) noexcept : m_request(util::NoConstruct{}) {}
  explicit Card(const char *path = Slot0, u32 flags = 0) noexcept
      : Resource(path, flags) {}

  ~Card() { m_request.Sync(); }

  // Disk interface
  inline bool Device_Available() const noexcept;
  IOSError Device_Init() noexcept;
  static inline size_t Device_GetBlockSize() noexcept { return SectorSize; }
  IOSError Device_BlockTransfer(size_t first, size_t count, void *buffer,
                                bool is_write) noexcept;

  void ReserveBlockBuffer();

  inline BufCommand::Request &SendCommand(BufCommand::Request &request,
                                          const Command &command) noexcept;
  inline BufCommand::Request &Select(BufCommand::Request &request) noexcept;
  inline BufCommand::Request &Deselect(BufCommand::Request &request) noexcept;
  inline BufCommand::Request &SetBlockLength(BufCommand::Request &request,
                                             u32 block_length) noexcept;
  inline BufCommand::Request &AppCommand(BufCommand::Request &request,
                                         u32 arg) noexcept;
  inline BufCommand::Request &SetBusWidth(BufCommand::Request &request,
                                          u32 bus_width) noexcept;

private:
  void *m_block_buffer;

  u16 m_relative_card_address;
  bool m_high_capacity;
  BufCommand::Request m_request;
};

inline bool Card::Device_Available() const noexcept {
  Status status;
  if (Ioctl<GetStatus>() //
          .Sync()
          .CopyOutput(status)
          .GetError()) {
    return false;
  }
  return !!(status & Status::Inserted) && !!(status & Status::TypeMemory);
}

inline Interface::BufCommand::Request &
Card::SendCommand(BufCommand::Request &request,
                  const Command &command) noexcept {
  return request.New(
      GetHandle(), command,
      ios::low::IOVector{command.buffer,
                         command.block_count * command.block_size});
}

inline Interface::BufCommand::Request &
Card::Select(BufCommand::Request &request) noexcept {
  return SendCommand(request,
                     {
                         .cmd = Cmd::SD_CMD7_SELECT,
                         .cmd_type = 3,
                         .response_type = ResponseType::R1B,
                         .arg = static_cast<u32>(m_relative_card_address << 16),
                     });
}

inline Interface::BufCommand::Request &
Card::Deselect(BufCommand::Request &request) noexcept {
  return SendCommand(request, {
                                  .cmd = Cmd::SD_CMD7_SELECT,
                                  .cmd_type = 3,
                                  .response_type = ResponseType::R1B,
                                  .arg = 0,
                              });
}

inline Interface::BufCommand::Request &
Card::SetBlockLength(BufCommand::Request &request, u32 block_length) noexcept {
  return SendCommand(request, {
                                  .cmd = Cmd::SD_CMD7_SELECT,
                                  .cmd_type = 3,
                                  .response_type = ResponseType::R1,
                                  .arg = block_length,
                              });
}

inline Interface::BufCommand::Request &
Card::AppCommand(BufCommand::Request &request, u32 arg) noexcept {
  return SendCommand(request, {
                                  .cmd = Cmd::SD_CMD55_APP_CMD,
                                  .cmd_type = 3,
                                  .response_type = ResponseType::R1,
                                  .arg = arg,
                              });
}

inline Interface::BufCommand::Request &
Card::SetBusWidth(BufCommand::Request &request, u32 bus_width) noexcept {
  return SendCommand(request, {
                                  .cmd = Cmd::SD_ACMD6_WIDE_BUS_SELECT,
                                  .cmd_type = 3,
                                  .response_type = ResponseType::R1,
                                  .arg = bus_width,
                              });
}

} // namespace peli::ios::sdio