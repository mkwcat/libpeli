// peli/ios/net/ip/top/Types.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../../../../common/Types.h"

namespace peli::ios::net::ip::top {

struct InAddr {
  /* 0x00 */ u32 addr;
};
static_assert(sizeof(InAddr) == 0x4);

struct SockAddr {
  /* 0x00 */ u8 len;
  /* 0x01 */ u8 family;
  /* 0x02 */ u16 in_port;
  /* 0x04 */ InAddr in_addr;
  /* 0x08 */ u8 zero[8] = {};
};
static_assert(sizeof(SockAddr) == 0x10);

struct PollSocket {
  /* 0x00 */ u32 sock;
  /* 0x04 */ u32 events;
  /* 0x08 */ u32 revents;
};
static_assert(sizeof(PollSocket) == 0x0C);

struct BindParams {
  /* 0x00 */ u32 socket;
  /* 0x04 */ u32 has_name;
  /* 0x08 */ u8 name[28];
};
static_assert(sizeof(BindParams) == 0x24);

struct ConnectParams {
  /* 0x00 */ u32 socket;
  /* 0x04 */ u32 has_addr;
  /* 0x08 */ u8 addr[28];
};
static_assert(sizeof(ConnectParams) == 0x24);

struct FcntlParams {
  /* 0x00 */ u32 socket;
  /* 0x04 */ u32 cmd;
  /* 0x08 */ u32 flags;
};
static_assert(sizeof(FcntlParams) == 0x0C);

struct ListenParams {
  /* 0x00 */ u32 socket;
  /* 0x04 */ u32 backlog;
};
static_assert(sizeof(ListenParams) == 0x08);

struct RecvFromParams {
  /* 0x00 */ u32 socket;
  /* 0x04 */ u32 flags;
};
static_assert(sizeof(RecvFromParams) == 0x08);

struct SendToParams {
  /* 0x00 */ u32 socket;
  /* 0x04 */ u32 flags;
  /* 0x08 */ u32 has_addr;
  /* 0x0C */ u8 dest_addr[28];
};
static_assert(sizeof(SendToParams) == 0x28);

struct ShutdownParams {
  /* 0x00 */ u32 socket;
  /* 0x04 */ u32 how;
};
static_assert(sizeof(ShutdownParams) == 0x08);

struct SetSockOptParams {
  /* 0x00 */ u32 socket;
  /* 0x04 */ u32 level;
  /* 0x08 */ u32 optname;
  /* 0x0C */ u32 optlen;
  /* 0x10 */ u8 optval[28];
};
static_assert(sizeof(SetSockOptParams) == 0x2C);

} // namespace peli::ios::net::ip::top