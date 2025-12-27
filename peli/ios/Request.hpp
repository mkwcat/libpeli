#pragma once

#include "../common/Types.hpp"
#include "../util/Halt.hpp"
#include "../util/NoConstruct.hpp"
#include "../util/Transform.hpp"
#include "Error.hpp"
#include "low/Ipc.hpp"

namespace peli::ios {

class Request {
public:
  class Open;
  class Close;
  class Read;
  class Write;
  class Seek;
  class Ioctl;
  class Ioctlv;

  Request() : m_synced(true) {}
  constexpr Request(util::NoConstruct)
      : m_queue(util::NoConstruct{}), m_synced(true) {}

  constexpr ~Request() { _PELI_ASSERT(m_synced); }

  template <class T>
  static constexpr void *operator new(size_t size, T &request) noexcept {
    _PELI_ASSERT(size == sizeof(T));
    return &request;
  }

private:
  void Peek() const noexcept { m_queue.Peek(); }

public:
  constexpr inline auto Sync(this auto &&self) noexcept -> decltype(self) {
    if (!self.m_synced) {
      self.Peek();
      self.m_synced = true;
    }
    return self;
  }

  constexpr inline auto New(this auto &&self, auto &&...args) noexcept
      -> decltype(self) {
    using Self = typename util::Transform<decltype(self)>::RemCVR::T;
    self.~Self();
    new (self) Self(args...);
    return self;
  }

  operator bool() noexcept { return m_synced; }

  constexpr s32 GetResult() const noexcept { return m_cmd_block.result; }
  constexpr IOSError GetError() const noexcept {
    return static_cast<IOSError>(m_cmd_block.result);
  }
  constexpr bool IsError() const noexcept { return m_cmd_block.result < 0; }

  constexpr s32 GetFd() const noexcept { return m_cmd_block.fd; }

  Open *GetOpen() noexcept;
  const Open *GetOpen() const noexcept {
    return const_cast<Request *>(this)->GetOpen();
  }
  Close *GetClose() noexcept;
  const Close *GetClose() const noexcept {
    return const_cast<Request *>(this)->GetClose();
  }
  Read *GetRead() noexcept;
  const Read *GetRead() const noexcept {
    return const_cast<Request *>(this)->GetRead();
  }
  Write *GetWrite() noexcept;
  const Write *GetWrite() const noexcept {
    return const_cast<Request *>(this)->GetWrite();
  }
  Seek *GetSeek() noexcept;
  const Seek *GetSeek() const noexcept {
    return const_cast<Request *>(this)->GetSeek();
  }
  Ioctl *GetIoctl() noexcept;
  const Ioctl *GetIoctl() const noexcept {
    return const_cast<Request *>(this)->GetIoctl();
  }
  Ioctlv *GetIoctlv() noexcept;
  const Ioctlv *GetIoctlv() const noexcept {
    return const_cast<Request *>(this)->GetIoctlv();
  }

protected:
  alignas(low::Alignment) low::IPCCommandBlock m_cmd_block = {};
  host::MessageQueue<low::IPCCommandBlock *, 1> m_queue;
  bool m_synced = true;
};

class Request::Open : public Request {
public:
  Open(const char *path, u32 flags) noexcept {
    low::IOS_OpenAsync(path, flags, m_queue, &m_cmd_block);
    m_synced = false;
  }
};

class Request::Close : public Request {
public:
  Close(s32 fd) noexcept {
    low::IOS_CloseAsync(fd, m_queue, &m_cmd_block);
    m_synced = false;
  }
};

class Request::Read : public Request {
public:
  using Request::Request;
  Read(s32 fd, void *data, s32 size) noexcept {
    low::IOS_ReadAsync(fd, data, size, m_queue, &m_cmd_block);
    m_synced = false;
  }

  constexpr void *GetData() const noexcept { return m_cmd_block.read.data; }
  constexpr s32 GetSize() const noexcept { return m_cmd_block.read.size; }
};

class Request::Write : public Request {
public:
  using Request::Request;
  Write(s32 fd, void *data, s32 size) noexcept {
    low::IOS_WriteAsync(fd, data, size, m_queue, &m_cmd_block);
    m_synced = false;
  }

  constexpr void *GetData() const noexcept { return m_cmd_block.write.data; }
  constexpr s32 GetSize() const noexcept { return m_cmd_block.write.size; }
};

class Request::Seek : public Request {
public:
  using Request::Request;
  Seek(s32 fd, s32 where, s32 whence) noexcept {
    low::IOS_SeekAsync(fd, where, whence, m_queue, &m_cmd_block);
    m_synced = false;
  }

  constexpr s32 GetOffset() const noexcept { return m_cmd_block.seek.where; }
  constexpr s32 GetOrigin() const noexcept { return m_cmd_block.seek.whence; }
};

class Request::Ioctl : public Request {
public:
  using Request::Request;
  Ioctl(s32 fd, u32 command, void *in, u32 in_size, void *out,
        u32 out_size) noexcept {
    low::IOS_IoctlAsync(fd, command, in, in_size, out, out_size, m_queue,
                        &m_cmd_block);
    m_synced = false;
  }

  constexpr void *GetInput() const noexcept { return m_cmd_block.ioctl.in; }
  constexpr u32 GetInputSize() const noexcept {
    return m_cmd_block.ioctl.in_size;
  }
  constexpr void *GetOutput() const noexcept { return m_cmd_block.ioctl.out; }
  constexpr u32 GetOutputSize() const noexcept {
    return m_cmd_block.ioctl.out_size;
  }
  constexpr u32 GetCommand() const noexcept { return m_cmd_block.ioctl.cmd; }
};

class Request::Ioctlv : public Request {
public:
  using Request::Request;
  Ioctlv(s32 fd, u32 command, u32 in_count, u32 out_count,
         low::IOVector *vec) noexcept {
    low::IOS_IoctlvAsync(fd, command, in_count, out_count, vec, m_queue,
                         &m_cmd_block);
    m_synced = false;
  }
};

inline Request::Open *Request::GetOpen() noexcept {
  if (m_cmd_block.cmd == low::IOS_CMD_OPEN) {
    return static_cast<Request::Open *>(this);
  }
  return nullptr;
}

inline Request::Close *Request::GetClose() noexcept {
  if (m_cmd_block.cmd == low::IOS_CMD_CLOSE) {
    return static_cast<Request::Close *>(this);
  }
  return nullptr;
}

inline Request::Read *Request::GetRead() noexcept {
  if (m_cmd_block.cmd == low::IOS_CMD_READ) {
    return static_cast<Request::Read *>(this);
  }
  return nullptr;
}

inline Request::Write *Request::GetWrite() noexcept {
  if (m_cmd_block.cmd == low::IOS_CMD_WRITE) {
    return static_cast<Request::Write *>(this);
  }
  return nullptr;
}

inline Request::Seek *Request::GetSeek() noexcept {
  if (m_cmd_block.cmd == low::IOS_CMD_SEEK) {
    return static_cast<Request::Seek *>(this);
  }
  return nullptr;
}

inline Request::Ioctl *Request::GetIoctl() noexcept {
  if (m_cmd_block.cmd == low::IOS_CMD_IOCTL) {
    return static_cast<Request::Ioctl *>(this);
  }
  return nullptr;
}

inline Request::Ioctlv *Request::GetIoctlv() noexcept {
  if (m_cmd_block.cmd == low::IOS_CMD_IOCTLV) {
    return static_cast<Request::Ioctlv *>(this);
  }
  return nullptr;
}

} // namespace peli::ios