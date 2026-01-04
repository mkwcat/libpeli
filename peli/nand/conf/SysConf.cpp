// peli/nand/conf/SysConf.cpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "SysConf.hpp"
#include "../../host/Mutex.hpp"
#include "../../ios/fs/Types.hpp"
#include "../../util/Address.hpp"
#include "../../util/Defer.hpp"
#include "../../util/Memory.hpp"
#include "../../util/String.hpp"

namespace peli::nand::conf {

SysConf &GetSysConf() noexcept {
  static SysConf instance("/shared2/sys/SYSCONF");
  return instance;
}

SysConf::SysConf(const char *path) noexcept : m_state(State::Error) {
  s32 fd = ios::low::IOS_Open(path, static_cast<u32>(ios::fs::OpenMode::Read));
  if (fd < 0) {
    return;
  }
  m_state = State::Invalid;
  new (m_request) ios::Request::Read(fd, m_data, Size);
}

void SysConf::clear() noexcept {}

host::Mutex *SysConf::lock() noexcept {
  if (m_state == State::Error) {
    return nullptr;
  }

  m_mutex.Lock();
  auto defer_unlock = util::Defer([this]() { m_mutex.Unlock(); });

  // Check again after lock
  if (m_state == State::Error) {
    return nullptr;
  }

  if (m_state == State::Invalid) {
    m_state = State::Error;
    if (m_request.Sync().GetResult() != Size) {
      return nullptr;
    }

    // Validate SysConf data
    if (util::ImmRead<u32, host::Endian::Big>(m_data) != HeaderMagic) {
      return nullptr;
    }
    if (util::ImmRead<u32, host::Endian::Big>(m_data + Size - 4) != EndMagic) {
      return nullptr;
    }
    if (u16 size = getHeaderSize(); size < 8 || size > LookupTableOffset) {
      return nullptr;
    }

    m_state = State::Valid;
  }

  defer_unlock.Cancel();
  return &m_mutex;
}

size_t SysConf::getEntryOffset(size_t index) const noexcept {
  size_t offset =
      util::ImmRead<u16, host::Endian::Big>(m_data, 0x6 + index * 2);
  if (offset < getHeaderSize() || !testEntryLength(offset)) {
    return 0;
  }
  return offset;
}

size_t SysConf::findEntryOffset(const char *key, size_t lookup) const noexcept {
  size_t key_len = util::StrLen(key);
  if (key_len == 0) {
    return 0;
  }

  if (lookup < LookupTableCount) {
    size_t entry_offset = util::ImmRead<u16, host::Endian::Big>(
        m_data, (Size - 0x6) - lookup * 0x2);
    if (entry_offset >= 0x0006 && entry_offset < getHeaderSize() &&
        util::IsAligned(sizeof(u16), entry_offset)) {
      size_t offset =
          util::ImmRead<u16, host::Endian::Big>(m_data, entry_offset);
      if (offset >= getHeaderSize() && testEntryLength(offset)) {
        u8 length = getEntryKeyLength(offset);
        if (length == key_len && testEntryLength(offset, length) &&
            util::MemoryEqual(m_data + offset + 1, key, length)) {
          return offset;
        }
      }
    }
  }

  size_t count = getCount();

  for (size_t i = 0; i < count; ++i) {
    size_t offset = getEntryOffset(i);
    u8 length = getEntryKeyLength(offset);
    if (length != key_len || !testEntryLength(offset, length)) {
      continue;
    }
    if (util::MemoryEqual(m_data + offset + 1, key, length)) {
      return offset;
    }
  }

  return 0;
}

SysConf::EntryType SysConf::Get(const char *key, u64 &out,
                                size_t lookup) noexcept {
  if (key == nullptr) {
    return EntryType::None;
  }

  rt::Mutex *mutex = const_cast<SysConf *>(this)->lock();
  if (mutex == nullptr) {
    return EntryType::None;
  }
  auto defer_unlock = util::Defer([mutex]() { mutex->Unlock(); });

  size_t offset = findEntryOffset(key, lookup);
  if (offset == 0) {
    return EntryType::None;
  }

  const u8 *data;
  size_t size;
  switch (getEntry(offset, data, size, key, size)) {
  default:
    return EntryType::None;

  case EntryType::Byte:
    out = util::ImmRead<s8>(data);
    return EntryType::Byte;

  case EntryType::Short:
    out = util::ImmReadMisaligned<s16, host::Endian::Big>(data);
    return EntryType::Short;

  case EntryType::Long:
    out = util::ImmReadMisaligned<s32, host::Endian::Big>(data);
    return EntryType::Long;

  case EntryType::LongLong:
    out = util::ImmReadMisaligned<s64, host::Endian::Big>(data);
    return EntryType::LongLong;

  case EntryType::Bool:
    out = util::ImmRead<u8>(data) != 0;
    return EntryType::Bool;
  }
}

SysConf::EntryType SysConf::Get(const char *key, const u8 *&out,
                                size_t &out_size, size_t lookup) noexcept {
  if (key == nullptr) {
    return EntryType::None;
  }

  rt::Mutex *mutex = const_cast<SysConf *>(this)->lock();
  if (mutex == nullptr) {
    return EntryType::None;
  }
  auto defer_unlock = util::Defer([mutex]() { mutex->Unlock(); });

  size_t offset = findEntryOffset(key, lookup);
  if (offset == 0) {
    return EntryType::None;
  }

  size_t key_length;
  return getEntry(offset, out, out_size, key, key_length);
}

SysConf::EntryType SysConf::Get(size_t index, const u8 *&out_data,
                                size_t &out_size, const char *&out_key,
                                size_t &out_key_length) noexcept {
  if (index >= GetCount()) {
    return EntryType::None;
  }

  rt::Mutex *mutex = const_cast<SysConf *>(this)->lock();
  if (mutex == nullptr) {
    return EntryType::None;
  }
  auto defer_unlock = util::Defer([mutex]() { mutex->Unlock(); });

  // Check again in case the data changed
  if (index >= getCount()) {
    return EntryType::None;
  }

  size_t offset = getEntryOffset(index);
  if (offset == 0) {
    return EntryType::None;
  }

  return getEntry(offset, out_data, out_size, out_key, out_key_length);
}

SysConf::EntryType SysConf::getEntry(size_t offset, const u8 *&out_data,
                                     size_t &out_size, const char *&out_key,
                                     size_t &out_key_length) const noexcept {

  u8 type_len = util::ImmRead<u8>(m_data, offset);
  EntryType type = static_cast<EntryType>(type_len >> 5);
  u8 key_length = getEntryKeyLength(offset);
  size_t data_offset = getEntryDataOffset(offset, key_length);

  u16 array_size = 1;
  size_t size;
  switch (type) {
  case EntryType::None:
    out_data = nullptr;
    out_size = 0;
    return EntryType::None;

  case EntryType::Byte:
  case EntryType::Bool:
    size = sizeof(u8);
    break;

  case EntryType::Short:
    size = sizeof(u16);
    break;

  case EntryType::Long:
    size = sizeof(u32);
    break;

  case EntryType::LongLong:
    size = sizeof(u64);
    break;

  case EntryType::BigArray:
    array_size =
        (util::ImmRead<u8, host::Endian::Big>(m_data, data_offset++) << 1) + 1;
    [[fallthrough]];
  case EntryType::SmallArray:
    if (!testEntryLength(offset, key_length, 2)) {
      return EntryType::None;
    }
    array_size += util::ImmRead<u8>(m_data, data_offset++);
    size = array_size;
    break;
  }

  if (!testEntryLength(offset, key_length, size)) {
    return EntryType::None;
  }

  out_data = m_data + data_offset;
  out_size = size;
  out_key = reinterpret_cast<const char *>(m_data + offset + 1);
  out_key_length = key_length;
  return type;
}

} // namespace peli::nand::conf
