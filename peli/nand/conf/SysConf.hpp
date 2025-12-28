// peli/nand/conf/SysConf.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../../cmn/Types.hpp"
#include "../../host/Mutex.hpp"
#include "../../ios/Request.hpp"
#include "../../util/Concept.hpp"
#include "../../util/Memory.hpp"
#include "string.h"

namespace peli::nand::conf {

enum class VideoMode : u8 {
  Ntsc = 0,
  Pal = 1,
};

enum class SensorBarPosition : u8 {
  Bottom = 0,
  Top = 1,
};

struct BtDeviceInfo {
  struct Entry {
    u8 mac_address[6];
    char name[64];
  };

  u8 count;
  Entry registered[10];
  Entry active[6];
};

class SysConf {
public:
  static constexpr size_t Size = 0x4000;
  static constexpr u32 HeaderMagic = 0x53437630; // "SCv0"
  static constexpr u32 EndMagic = 0x53436564;    // "SCed"
  static constexpr size_t LookupTableOffset = Size - 0x52;
  static constexpr size_t LookupTableCount = 49;

  enum class EntryType : s8 {
    None = 0,
    BigArray = 1,
    SmallArray = 2,
    Byte = 3,
    Short = 4,
    Long = 5,
    LongLong = 6,
    Bool = 7,
  };

  constexpr SysConf() noexcept
      : m_data{}, m_request{}, m_state(State::Invalid) {}
  SysConf(const char *path) noexcept;
  ~SysConf() noexcept { clear(); }

  void Open() noexcept;
  void Flush() noexcept;

  EntryType Get(const char *key, u64 &out, size_t lookup = -1) noexcept;

  EntryType Get(const char *key, const u8 *&out, size_t &out_size,
                size_t lookup = -1) noexcept;

  EntryType Get(size_t index, const u8 *&out_data, size_t &out_size,
                const char *&out_key, size_t &out_key_length) noexcept;

  template <class T>
  EntryType Get(const char *key, T &out, size_t lookup = -1) noexcept {
    if constexpr (util::IntegralType<T> || util::EnumType<T>) {
      u64 value;
      EntryType result = Get(key, value, lookup);
      out = static_cast<T>(value);
      return result;
    } else {
      const u8 *data;
      size_t size;
      EntryType result = Get(key, data, size, lookup);
      if (result != EntryType::None) {
        ::memcpy(static_cast<void *>(&out), data,
                 sizeof(T) < size ? sizeof(T) : size);
      }
      return result;
    }
  }

  size_t GetCount() const noexcept {
    return m_state != State::Valid ? 0 : getCount();
  }

private:
  void clear() noexcept;
  host::Mutex *lock() noexcept;

  size_t getEntryOffset(size_t index) const noexcept;
  size_t findEntryOffset(const char *key, size_t lookup = -1) const noexcept;
  EntryType getEntry(size_t offset, const u8 *&out_data, size_t &out_size,
                     const char *&out_key,
                     size_t &out_key_length) const noexcept;

  u8 getEntryKeyLength(size_t offset) const noexcept {
    return (util::ImmRead<u8>(m_data, offset) & 0x1F) + 1;
  }
  static size_t getEntryDataOffset(size_t offset, size_t key_length) noexcept {
    return offset + 1 + key_length;
  }
  static bool testEntryLength(size_t offset, size_t key_length = 1,
                              size_t data_length = 1) noexcept {
    return getEntryDataOffset(offset, key_length) + data_length <=
           LookupTableOffset;
  }

  u16 getCount() const noexcept {
    return util::ImmRead<u16, host::Endian::Big>(m_data, 0x4);
  }
  u16 getHeaderSize() const noexcept { return 0x8 + getCount() * 0x2; }

private:
  alignas(ios::low::Alignment) u8 m_data[Size];
  ios::Request m_request;
  mutable host::Mutex m_mutex;

  enum class State : u8 {
    Invalid,
    Valid,
    Error,
  } m_state;
};

extern SysConf &GetSysConf() noexcept;

} // namespace peli::nand::conf