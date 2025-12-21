// peli/nand/conf/Ipl.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "SysConf.hpp"

namespace peli::nand::conf {

inline bool GetCounterBias(u32 &value) noexcept {
  return GetSysConf().Get("IPL.CB", value, 0) != SysConf::EntryType::None;
}

enum class AspectRatio : s8 {
  Standard = 0,
  Wide = 1,
};

inline bool GetAspectRatio(AspectRatio &value) noexcept {
  return GetSysConf().Get("IPL.AR", value, 1) != SysConf::EntryType::None;
}

enum class AutoRunMode : s8 {
  Off = 0,
  On = 1,
};

inline bool GetAutoRunMode(AutoRunMode &value) noexcept {
  return GetSysConf().Get("IPL.ARN", value, -1) != SysConf::EntryType::None;
}

inline bool GetConfigDone(bool &value) noexcept {
  return GetSysConf().Get("IPL.CD", value, 3) != SysConf::EntryType::None;
}

inline bool GetConfigDone2(bool &value) noexcept {
  return GetSysConf().Get("IPL.CD2", value, 4) != SysConf::EntryType::None;
}

inline bool GetDisplayOffsetHorizontal(s8 &value) noexcept {
  return GetSysConf().Get("IPL.DH", value, 5) != SysConf::EntryType::None;
}

enum class EuRgb60Mode : s8 {
  Pal50 = 0,
  EuRgb60 = 1,
};

inline bool GetEurgb60Mode(EuRgb60Mode &value) noexcept {
  return GetSysConf().Get("IPL.E60", value, 6) != SysConf::EntryType::None;
}

inline bool GetEulaDone(bool &value) noexcept {
  return GetSysConf().Get("IPL.EULA", value, 7) != SysConf::EntryType::None;
}

inline bool GetFreeChannelAppCount(u32 &value) noexcept {
  return GetSysConf().Get("IPL.FRC", value, 8) != SysConf::EntryType::None;
}

struct IdleMode {
  enum class SlotIllumination : s8 {
    Off = 0,
    Dim = 1,
    Bright = 2,
  };

  bool wc24;
  SlotIllumination slot_illumination;
};

inline bool GetIdleMode(IdleMode &value) noexcept {
  const u8 *data;
  size_t size;
  if (auto type = GetSysConf().Get("IPL.IDL", data, size, 9);
      (type != SysConf::EntryType::SmallArray &&
       type != SysConf::EntryType::BigArray) ||
      size < 2) {
    return false;
  }

  value.wc24 = data[0];
  value.slot_illumination = static_cast<IdleMode::SlotIllumination>(data[1]);
  return true;
}

inline bool GetInstalledChannelAppCount(u32 &value) noexcept {
  return GetSysConf().Get("IPL.INC", value, 10) != SysConf::EntryType::None;
}

enum class Language : s8 {
  Japanese = 0,
  English = 1,
  German = 2,
  French = 3,
  Spanish = 4,
  Italian = 5,
  Dutch = 6,
  SimplifiedChinese = 7,
  TraditionalChinese = 8,
  Korean = 9,
};

inline bool GetLanguage(Language &value) noexcept {
  return GetSysConf().Get("IPL.LNG", value, 11) != SysConf::EntryType::None;
}

struct ConsoleNickname {
  u16 name[10];
  u16 length;
};

inline bool GetConsoleNickname(ConsoleNickname &value) noexcept {
  const u8 *data;
  size_t size;
  if (auto type = GetSysConf().Get("IPL.NIK", data, size, 12);
      (type != SysConf::EntryType::SmallArray &&
       type != SysConf::EntryType::BigArray) ||
      size < 22) {
    return false;
  }

  for (int i = 0; i < 20; i++) {
    value.name[i] = util::ImmRead<u16, host::Endian::Big>(data, i * 2);
  }
  value.length = util::ImmRead<u16, host::Endian::Big>(data, 20);
  return true;
}

struct ParentalControls {
  enum class Enabled : s8 {
    Off = 0,
    On = -128,
  };

  enum class SecretQuestion : s8 {
    MothersMaidenName = 0,
    PlaceOfBirth = 1,
    FavoriteSportsTeam = 2,
    FavoriteBirthdayPresent = 3,
    FavoriteMovie = 4,
    WhereWouldYouGo = 5,
  };

  Enabled enabled;
  u8 organization;
  u8 maximum_rating;
  char pin[4];
  SecretQuestion secret_question;
  u16 answer[20];
  u16 answer_length;
};

inline bool GetParentalControls(ParentalControls &value) noexcept {
  const u8 *data;
  size_t size;
  if (auto type = GetSysConf().Get("IPL.PC", data, size, 13);
      (type != SysConf::EntryType::SmallArray &&
       type != SysConf::EntryType::BigArray) ||
      size < 74) {
    return false;
  }

  value.enabled = static_cast<ParentalControls::Enabled>(data[0]);
  value.organization = data[1];
  value.maximum_rating = data[2];
  for (int i = 0; i < 4; i++) {
    value.pin[i] = data[3 + i];
  }
  value.secret_question =
      static_cast<ParentalControls::SecretQuestion>(data[7]);
  for (int i = 0; i < 20; i++) {
    value.answer[i] = util::ImmRead<u16, host::Endian::Big>(data, 8 + i * 2);
  }
  value.answer_length = util::ImmRead<u16, host::Endian::Big>(data, 72);
  return true;
}

enum class ProgressiveMode : u8 {
  Off = 0,
  On = 1,
};

inline bool GetProgressiveMode(ProgressiveMode &value) noexcept {
  return GetSysConf().Get("IPL.PGS", value, 14) != SysConf::EntryType::None;
}

enum class ScreenSaverMode : u8 {
  Off = 0,
  On = 1,
};

inline bool GetScreenSaverMode(ScreenSaverMode &value) noexcept {
  return GetSysConf().Get("IPL.SSV", value, 15) != SysConf::EntryType::None;
}

struct SimpleAddress {
  u8 country;
  u8 region;
  u16 city;
  u16 country_name[16][64];
  u16 region_name[16][64];
  u16 latitude;
  u16 longitude;
};

inline bool GetSimpleAddress(SimpleAddress &value) noexcept {
  const u8 *data;
  size_t size;
  if (auto type = GetSysConf().Get("IPL.SAD", data, size, 16);
      (type != SysConf::EntryType::SmallArray &&
       type != SysConf::EntryType::BigArray) ||
      size < 4104) {
    return false;
  }

  value.country = data[0];
  value.region = data[1];
  value.city = util::ImmRead<u16, host::Endian::Big>(data, 2);
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 64; j++) {
      value.country_name[i][j] =
          util::ImmRead<u16, host::Endian::Big>(data, 4 + (i * 64 + j) * 2);
    }
  }
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 64; j++) {
      value.region_name[i][j] =
          util::ImmRead<u16, host::Endian::Big>(data, 2052 + (i * 64 + j) * 2);
    }
  }
  value.latitude = util::ImmRead<u16, host::Endian::Big>(data, 4100);
  value.longitude = util::ImmRead<u16, host::Endian::Big>(data, 4102);
  return true;
}

enum class SoundMode : u8 {
  Mono = 0,
  Stereo = 1,
  Surround = 2,
};

inline bool GetSoundMode(SoundMode &value) noexcept {
  return GetSysConf().Get("IPL.SND", value, 17) != SysConf::EntryType::None;
}

enum class UpdateType : u8 {
  None = 0,
  Disc = 1,
  Internet = 2,
};

inline bool GetUpdateType(UpdateType &value) noexcept {
  return GetSysConf().Get("IPL.UPT", value, 18) != SysConf::EntryType::None;
}

inline bool GetTemporaryTitleID(u64 &value) noexcept {
  return GetSysConf().Get("IPL.TID", value, 36) != SysConf::EntryType::None;
}

inline bool GetAutoPowerDown(s32 &value) noexcept {
  return GetSysConf().Get("IPL.APD", value, 38) != SysConf::EntryType::None;
}

} // namespace peli::nand::conf
