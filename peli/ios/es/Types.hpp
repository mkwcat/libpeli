// peli/ios/es/Types.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../../common/Types.h"
#include "../iosc/Types.hpp"

namespace peli::ios::es {

using Error = s32;
using TitleId = u64;
using TitleType = u32;
using DeviceId = u32;
using Version = u8;

constexpr u32 LimitCount = 8;

enum class LimitCode : u32 {
  None = 0,
  DurationTime = 1,
  AbsoluteTime = 2,
  NumTitles = 3,
  NumLaunch = 4,
  ElapsedTime = 5,
};

struct LpEntry {
  LimitCode code;
  u32 limit;
};

static_assert(sizeof(LpEntry) == 0x8);

using TicketId = u64;
using TicketVersion = u16;

enum class LicenseType : u8 {
  Permanent = 0,
  Demo = 1,
  Trial = 2,
  Rental = 3,
  Subscription = 4,
  Service = 5,
};

using SysAccessMask = u8[2];
using CidxMask = u8[64];
using TicketCustomData = u8[20];
using TicketReserved = u8[25];
using PropertyMask = u16;

struct _PELI_PACKED TicketInfo {
  /* 0x004 / 0x1D0 */ TicketId ticket_id;
  /* 0x00C / 0x1D8 */ DeviceId device_id;
  /* 0x010 / 0x1DC */ TitleId title_id;
  /* 0x018 / 0x1E4 */ SysAccessMask sys_access_mask;
  /* 0x01A / 0x1E6 */ TicketVersion ticket_version;
  /* 0x01C / 0x1E8 */ u32 access_title_id;
  /* 0x020 / 0x1EC */ u32 access_title_mask;
  /* 0x024 / 0x1F0 */ LicenseType license_type;
  /* 0x025 / 0x1F1 */ u8 key_id;
  /* 0x026 / 0x1F2 */ PropertyMask property_mask;
  /* 0x028 / 0x1F4 */ TicketCustomData custom_data;
  /* 0x03C / 0x208 */ TicketReserved reserved;
  /* 0x055 / 0x221 */ u8 audit;
  /* 0x056 / 0x222 */ CidxMask cidx_mask;
  /* 0x096 / 0x262 */ u16 pad;
  /* 0x098 / 0x264 */ LpEntry limits[LimitCount];
};

static_assert(sizeof(TicketInfo) == 0xD4);

template <class Signature = iosc::SigRsa2048> struct Ticket {
  /* 0x000 */ Signature sig;
  /* 0x180 */ iosc::EccPublicKey server_public_key;
  /* 0x1BC */ Version version;
  /* 0x1BD */ Version ca_crl_version;
  /* 0x1BE */ Version signer_crl_version;
  /* 0x1BF */ iosc::AesKey title_key;
  /* 0x1CF */ u8 pad;
  /* 0x1D0 */ TicketInfo info;
};

static_assert(sizeof(Ticket<iosc::SigRsa2048>) == 0x2A4);

struct TicketView {
  u32 view;
  TicketInfo info;
};

static_assert(sizeof(TicketView) == 0xD8);

using ContentId = u32;
using ContentIndex = u16;
using ContentType = u16;

struct ContentInfo {
  ContentId id;
  ContentIndex index;
  ContentType type;
  u64 size;
};

using TitleVersion = u16;
using MinorTitleVersion = u16;
using SysVersion = TitleId;

using TmdCustomData = u8[32];
using TmdReserved = u8[30];

} // namespace peli::ios::es