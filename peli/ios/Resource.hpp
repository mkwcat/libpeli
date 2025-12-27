#pragma once

#include "../common/Types.hpp"
#include "Error.hpp"
#include "Interface.hpp"
#include "Request.hpp"
#include "low/Ipc.hpp"

namespace peli::ios {

template <class TInterfaceType = void> class Resource;

template <> class Resource<void> {
  const s32 m_handle = IOSError::IOS_ERROR_NOEXISTS;

public:
  constexpr Resource() noexcept = default;
  constexpr Resource(const Resource &) = delete;
  constexpr Resource(Resource &&other) noexcept : m_handle(other.m_handle) {}

  Resource(const char *path, u32 flags = 0) noexcept
      : m_handle(low::IOS_Open(path, flags)) {}

  constexpr Resource(s32 handle) noexcept : m_handle(handle) {}

  ~Resource() noexcept {
    if (m_handle >= 0) {
      low::IOS_Close(m_handle);
    }
  }

  constexpr Request::Close Close() noexcept { return Request::Close(m_handle); }

  constexpr s32 GetHandle() const noexcept { return m_handle; }
  constexpr bool IsValid() const noexcept { return m_handle >= 0; }

  constexpr operator s32() const noexcept { return GetHandle(); }

  constexpr Request::Read Read(void *data, s32 size) noexcept {
    return Request::Read(m_handle, data, size);
  }

  constexpr Request::Write Write(void *data, s32 size) noexcept {
    return Request::Write(m_handle, data, size);
  }

  constexpr Request::Seek Seek(s32 where, s32 whence) noexcept {
    return Request::Seek(m_handle, where, whence);
  }

  constexpr Request::Ioctl Ioctl(u32 command, void *in = nullptr,
                                 u32 in_size = 0, void *out = nullptr,
                                 u32 out_size = 0) noexcept {
    return Request::Ioctl(m_handle, command, in, in_size, out, out_size);
  }

  constexpr Request::Ioctlv Ioctlv(u32 command, u32 in_count, u32 out_count,
                                   low::IOVector *vec) noexcept {
    return Request::Ioctlv(m_handle, command, in_count, out_count, vec);
  }
};

template <util::EnumType TIoctlCmdType>
class Resource<Interface<TIoctlCmdType>> : public Resource<void> {
public:
  using InterfaceType = Interface<TIoctlCmdType>;
  using IoctlCmdType = TIoctlCmdType;
  using Resource<void>::Resource;
};

template <class TInterfaceType>
class Resource : public Resource<typename TInterfaceType::IosInterfaceType> {
public:
  using Resource<typename TInterfaceType::IosInterfaceType>::Resource;

  template <class TIoctlDef, class... TArgs>
  constexpr TIoctlDef::Request Ioctl(TArgs &&...args) const {
    return typename TIoctlDef::Request(Resource::GetHandle(), args...);
  }
};

} // namespace peli::ios