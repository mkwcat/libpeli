// wsh/ios/low/Ipc.hpp - Low-level IOS IPC functions
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../../common/Types.h"
#include "../../host/MessageQueue.hpp"

namespace wsh::ios::low {

/**
 * The byte alignment necessary for buffers passed through IPC. This is derived
 * from the processor cache line size of both the PPC and ARM CPUs.
 */
constexpr size_t Alignment = 32;

enum {
  IOS_CMD_OPEN = 1,
  IOS_CMD_CLOSE = 2,
  IOS_CMD_READ = 3,
  IOS_CMD_WRITE = 4,
  IOS_CMD_SEEK = 5,
  IOS_CMD_IOCTL = 6,
  IOS_CMD_IOCTLV = 7,
};

struct IOVector {
  void *data;
  u32 size;
};

struct IOSRequest {
  u32 cmd;
  s32 result;
  s32 fd;

  struct Open {
    char *path;
    u32 flags;
    u32 uid;
    u16 gid;
  };

  struct ReadWrite {
    void *data;
    u32 size;
  };

  struct Seek {
    s32 where;
    s32 whence;
  };

  struct Ioctl {
    u32 cmd;
    void *in;
    u32 in_size;
    void *out;
    u32 out_size;
  };

  struct Ioctlv {
    u32 cmd;
    u32 in_count;
    u32 out_count;
    IOVector *vec;
  };

  union {
    u32 args[5];
    Open open;
    ReadWrite read, write;
    Seek seek;
    Ioctl ioctl;
    Ioctlv ioctlv;
  };
};

struct alignas(32) IPCCommandBlock : IOSRequest {
  host::MessageQueue<IPCCommandBlock *> *queue;
};

s32 IOS_Open(const char *path, u32 flags) noexcept;
s32 IOS_Close(s32 fd) noexcept;
s32 IOS_Read(s32 fd, void *data, s32 size) noexcept;
s32 IOS_Write(s32 fd, void *data, s32 size) noexcept;
s32 IOS_Seek(s32 fd, s64 where, u32 whence) noexcept;
s32 IOS_Ioctl(s32 fd, u32 cmd, void *in, u32 in_size, void *out,
              u32 out_size) noexcept;
s32 IOS_Ioctlv(s32 fd, u32 cmd, u32 in_count, u32 out_count,
               IOVector *vectors) noexcept;

s32 IOS_OpenAsync(const char *path, u32 flags,
                  host::MessageQueue<IPCCommandBlock *> &queue,
                  IPCCommandBlock *block) noexcept;
s32 IOS_CloseAsync(s32 fd, host::MessageQueue<IPCCommandBlock *> &queue,
                   IPCCommandBlock *block) noexcept;
s32 IOS_ReadAsync(s32 fd, void *data, s32 size,
                  host::MessageQueue<IPCCommandBlock *> &queue,
                  IPCCommandBlock *block) noexcept;
s32 IOS_WriteAsync(s32 fd, void *data, s32 size,
                   host::MessageQueue<IPCCommandBlock *> &queue,
                   IPCCommandBlock *block) noexcept;
s32 IOS_SeekAsync(s32 fd, s64 where, u32 whence,
                  host::MessageQueue<IPCCommandBlock *> &queue,
                  IPCCommandBlock *block) noexcept;
s32 IOS_IoctlAsync(s32 fd, u32 cmd, void *in, u32 in_size, void *out,
                   u32 out_size, host::MessageQueue<IPCCommandBlock *> &queue,
                   IPCCommandBlock *block) noexcept;
s32 IOS_IoctlvAsync(s32 fd, u32 cmd, u32 in_count, u32 out_count,
                    IOVector *vectors,
                    host::MessageQueue<IPCCommandBlock *> &queue,
                    IPCCommandBlock *block) noexcept;

void Init() noexcept;

} // namespace wsh::ios::low