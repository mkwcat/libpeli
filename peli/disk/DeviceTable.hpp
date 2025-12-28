// peli/disk/DeviceTable.cpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../cmn/Types.hpp"
#include "../util/Concept.hpp"
#include "../util/Transform.hpp"

namespace peli::disk {

template <class T>
concept ImplementsDeviceTable = requires(T t) {
  { t.Device_Available() } -> util::IsConvertibleTo<bool>;
  { t.Device_Init() } -> util::IsConvertibleTo<int>;
  { t.Device_GetBlockSize() } -> util::IsConvertibleTo<size_t>;
  {
    t.Device_BlockTransfer(size_t(), size_t(), (void *)nullptr, bool())
  } -> util::IsConvertibleTo<size_t>;
};

struct DeviceTable {
  void *m_object;
  bool (*m_available)(void *obj);
  int (*m_init)(void *obj);
  size_t (*m_get_block_size)(void *obj);
  int (*m_block_transfer)(void *obj, size_t first, size_t count, void *buffer,
                          bool is_write);

  constexpr DeviceTable() noexcept = default;

  constexpr DeviceTable(ImplementsDeviceTable auto &&object) noexcept {
    using T = typename util::Transform<decltype(object)>::RemCVR::T;
    m_object = static_cast<void *>(&object);

    m_available = [](void *obj) -> bool {
      auto result = static_cast<T *>(obj)->Device_Available();
      if constexpr (util::EnumType<decltype(result)> ||
                    (util::IntegralType<decltype(result)> &&
                     !util::SameAs<decltype(result), bool>)) {
        return result == 0;
      } else {
        return static_cast<bool>(result);
      }
    };

    m_init = [](void *obj) -> int {
      auto result = static_cast<T *>(obj)->Device_Init();
      if constexpr (util::SameAs<decltype(result), bool>) {
        return static_cast<int>(!result);
      } else {
        return static_cast<int>(result);
      }
    };

    m_get_block_size = [](void *obj) -> size_t {
      auto result = static_cast<T *>(obj)->Device_GetBlockSize();
      return static_cast<size_t>(result);
    };

    m_block_transfer = [](void *obj, size_t first, size_t count, void *buffer,
                          bool is_write) -> int {
      auto result = static_cast<T *>(obj)->Device_BlockTransfer(
          first, count, buffer, is_write);
      if constexpr (util::SameAs<decltype(result), bool>) {
        return static_cast<int>(!result);
      } else {
        return static_cast<int>(result);
      }
    };
  }
};

} // namespace peli::disk