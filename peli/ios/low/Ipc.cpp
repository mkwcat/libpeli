// peli/ios/low/Ipc.cpp - Low-level IOS IPC functions
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "Ipc.hpp"
#include "../../hw/Bit.hpp"
#include "../../hw/Interrupt.hpp"
#include "../../hw/Wood.hpp"
#include "../../ppc/Msr.hpp"
#include "../../ppc/Sync.hpp"
#include "../../runtime/Exception.hpp"
#include "../../util/Address.hpp"
#include "../../util/CpuCache.hpp"
#include "../../util/String.hpp"
#include "../Error.hpp"

namespace peli::ios::low {

#ifndef PELI_HOST_IOS

namespace {

constinit host::MessageQueue<IPCCommandBlock *> *s_queue_send;
constinit bool s_waiting_ack = false;

enum IpcBit {
  X1 = 1 << 0,
  Y2 = 1 << 1,
  Y1 = 1 << 2,
  X2 = 1 << 3,
  IY1 = 1 << 4,
  IY2 = 1 << 5,
};

[[maybe_unused]]
void syncNoInterrupt(IOSRequest &request) noexcept {
  util::CpuCache::DcFlush(request);

  hw::WOOD->IPCPPCMSG.PTR = util::Physical(&request);
  ppc::Eieio();
  hw::WOOD->IPCPPCCTRL = X1;
  while (!hw::WOOD->IPCPPCCTRL.Y2) {
    if (hw::WOOD->IPCPPCCTRL.Y1) {
      // Expected an ack but got a reply!
      hw::WOOD->IPCPPCCTRL = Y1;
      hw::WOOD->IPCPPCCTRL = X2;
    }
  }

  hw::WOOD->IPCPPCCTRL = Y2;

  void *reply;
  do {
    while (!hw::WOOD->IPCPPCCTRL.Y1) {
      if (hw::WOOD->IPCPPCCTRL.Y2) {
        // Expected a reply but got an ack!
        hw::WOOD->IPCPPCCTRL.Y2 = 1;
      }
    }
    reply = const_cast<void *>(hw::WOOD->IPCIOPMSG.PTR);
    hw::WOOD->IPCPPCCTRL = Y1;
    hw::WOOD->IPCPPCCTRL = X2;
  } while (reply != util::Physical(&request));

  util::CpuCache::DcInvalidate(request);
}

// Expects interrupts to be disabled
void ipcAcrSend(IPCCommandBlock *request) {
  util::CpuCache::DcFlush(request, sizeof(IOSRequest));
  ppc::Sync();

  hw::WOOD->IPCPPCMSG.PTR = util::Physical(request);
  ppc::Eieio();
  hw::WOOD->IPCPPCCTRL = IY1 | IY2 | X1;

  s_waiting_ack = true;
}

void handleAck() {
  hw::WOOD->IPCPPCCTRL = IY1 | Y2;
  ppc::Eieio();
  hw::WOOD->PPCINTSTS = hw::BitMask(hw::Irq::IpcPpc);

  s_waiting_ack = false;

  // Send the next request if available
  IPCCommandBlock *request;
  if (s_queue_send->TryReceive(request)) {
    ipcAcrSend(request);
  }
}

void handleReply() {
  void *reply_ptr = const_cast<void *>(hw::WOOD->IPCIOPMSG.PTR);

  hw::WOOD->IPCPPCCTRL = (hw::WOOD->IPCPPCCTRL & IY2) | Y1;
  ppc::Eieio();
  hw::WOOD->PPCINTSTS = hw::BitMask(hw::Irq::IpcPpc);
  ppc::Eieio();
  hw::WOOD->IPCPPCCTRL = IY1 | IY2 | X2;

  IPCCommandBlock *reply =
      static_cast<IPCCommandBlock *>(util::Effective(reply_ptr));
  util::CpuCache::DcInvalidate(reply, sizeof(IOSRequest));

  // Fix the reply before sending it to the user. Must invalidate input buffers
  // as some devices (cough SSL) may write to them.
  switch (reply->cmd) {
  case IOS_CMD_READ:
    reply->read.data = util::Effective(reply->read.data);
    util::CpuCache::DcInvalidate(reply->read.data, reply->read.size);
    break;
  case IOS_CMD_WRITE:
    reply->write.data = util::Effective(reply->write.data);
    break;
  case IOS_CMD_IOCTL:
    if (reply->ioctl.in_size != 0) {
      reply->ioctl.in = util::Effective(reply->ioctl.in);
      util::CpuCache::DcInvalidate(reply->ioctl.in, reply->ioctl.in_size);
    }
    if (reply->ioctl.out_size != 0) {
      reply->ioctl.out = util::Effective(reply->ioctl.out);
      if (reply->ioctl.out != reply->ioctl.in) {
        util::CpuCache::DcInvalidate(reply->ioctl.out, reply->ioctl.out_size);
      }
    }
    break;
  case IOS_CMD_IOCTLV:
    reply->ioctlv.vec = util::Effective(reply->ioctlv.vec);
    for (u32 i = 0; i < reply->ioctlv.in_count + reply->ioctlv.out_count; i++) {
      if (reply->ioctlv.vec[i].size != 0) {
        reply->ioctlv.vec[i].data = util::Effective(reply->ioctlv.vec[i].data);
        util::CpuCache::DcInvalidate(reply->ioctlv.vec[i].data,
                                     reply->ioctlv.vec[i].size);
      }
    }
    break;
  }

  if (reply->queue) {
    reply->queue->Send(reply);
  }
}

void ipcHandleInterrupt(hw::Irq, ppc::Context *) {
  u32 ctrl = hw::WOOD->IPCPPCCTRL;

  if (ctrl & Y2) {
    handleAck();
  }
  if (ctrl & Y1) {
    handleReply();
  }
}

void ipcAsync(IPCCommandBlock *request) {
  ppc::Msr::NoInterruptsScope guard;

  if (!s_waiting_ack) {
    // Send the request on this thread
    ipcAcrSend(request);
  } else {
    // Enqueue the request to send once the ack is received
    s_queue_send->Send(request);
  }
}

} // namespace

s32 IOS_Open(const char *path, u32 flags) noexcept {
  alignas(Alignment) char path_fixed[PathSize] = {};
  if (util::StrCopy<PathSize>(path_fixed, path) == PathSize) {
    return IOSError::IOS_ERROR_INVALID;
  }

  alignas(Alignment) IPCCommandBlock request = {};
  host::MessageQueue<IPCCommandBlock *, 1> queue;

  s32 result = IOS_OpenAsync(path_fixed, flags, queue, &request);
  return result != IOSError::IOS_ERROR_OK ? result : queue.Receive()->result;
}

s32 IOS_Close(s32 fd) noexcept {
  alignas(Alignment) IPCCommandBlock request = {};
  host::MessageQueue<IPCCommandBlock *, 1> queue;

  s32 result = IOS_CloseAsync(fd, queue, &request);
  return result != IOSError::IOS_ERROR_OK ? result : queue.Receive()->result;
}

s32 IOS_Read(s32 fd, void *data, s32 size) noexcept {
  alignas(Alignment) IPCCommandBlock request = {};
  host::MessageQueue<IPCCommandBlock *, 1> queue;

  s32 result = IOS_ReadAsync(fd, data, size, queue, &request);
  return result != IOSError::IOS_ERROR_OK ? result : queue.Receive()->result;
}

s32 IOS_Write(s32 fd, void *data, s32 size) noexcept {
  alignas(Alignment) IPCCommandBlock request = {};
  host::MessageQueue<IPCCommandBlock *, 1> queue;

  s32 result = IOS_WriteAsync(fd, data, size, queue, &request);
  return result != IOSError::IOS_ERROR_OK ? result : queue.Receive()->result;
}

s32 IOS_Seek(s32 fd, s32 where, s32 whence) noexcept {
  alignas(Alignment) IPCCommandBlock request = {};
  host::MessageQueue<IPCCommandBlock *, 1> queue;

  s32 result = IOS_SeekAsync(fd, where, whence, queue, &request);
  return result != IOSError::IOS_ERROR_OK ? result : queue.Receive()->result;
}

s32 IOS_Ioctl(s32 fd, u32 command, void *in, u32 in_size, void *out,
              u32 out_size) noexcept {
  alignas(Alignment) IPCCommandBlock request = {};
  host::MessageQueue<IPCCommandBlock *, 1> queue;

  s32 result =
      IOS_IoctlAsync(fd, command, in, in_size, out, out_size, queue, &request);
  return result != IOSError::IOS_ERROR_OK ? result : queue.Receive()->result;
}

s32 IOS_Ioctlv(s32 fd, u32 command, u32 in_count, u32 out_count,
               IOVector *vec) noexcept {
  alignas(Alignment) IPCCommandBlock request = {};
  host::MessageQueue<IPCCommandBlock *, 1> queue;

  s32 result =
      IOS_IoctlvAsync(fd, command, in_count, out_count, vec, queue, &request);
  return result != IOSError::IOS_ERROR_OK ? result : queue.Receive()->result;
}

s32 IOS_OpenAsync(const char *path, u32 flags,
                  host::MessageQueue<IPCCommandBlock *> &queue,
                  IPCCommandBlock *block) noexcept {
  if (!util::IsAligned(Alignment, block) || !util::IsAligned(Alignment, path)) {
    block->result = IOSError::IOS_ERROR_INVALID;
    return IOSError::IOS_ERROR_INVALID;
  }

  util::CpuCache::DcFlush(path, 64);

  *block = {};
  block->cmd = IOS_CMD_OPEN;
  block->open.path = util::Physical(const_cast<char *>(path));
  block->open.flags = flags;
  block->queue = &queue;

  ipcAsync(block);

  return IOSError::IOS_ERROR_OK;
}

s32 IOS_CloseAsync(s32 fd, host::MessageQueue<IPCCommandBlock *> &queue,
                   IPCCommandBlock *block) noexcept {
  if (!util::IsAligned(Alignment, block)) {
    block->result = IOSError::IOS_ERROR_INVALID;
    return IOSError::IOS_ERROR_INVALID;
  }

  *block = {};
  block->cmd = IOS_CMD_CLOSE;
  block->fd = fd;
  block->queue = &queue;

  ipcAsync(block);

  return IOSError::IOS_ERROR_OK;
}

s32 IOS_SeekAsync(s32 fd, s32 where, s32 whence,
                  host::MessageQueue<IPCCommandBlock *> &queue,
                  IPCCommandBlock *block) noexcept {
  if (!util::IsAligned(Alignment, block)) {
    block->result = IOSError::IOS_ERROR_INVALID;
    return IOSError::IOS_ERROR_INVALID;
  }

  *block = {};
  block->cmd = IOS_CMD_SEEK;
  block->fd = fd;
  block->seek.where = where;
  block->seek.whence = whence;
  block->queue = &queue;

  ipcAsync(block);

  return IOSError::IOS_ERROR_OK;
}

s32 IOS_ReadAsync(s32 fd, void *data, s32 size,
                  host::MessageQueue<IPCCommandBlock *> &queue,
                  IPCCommandBlock *block) noexcept {
  if (!util::IsAligned(Alignment, block)) {
    block->result = IOSError::IOS_ERROR_INVALID;
    return IOSError::IOS_ERROR_INVALID;
  }

  *block = {};
  block->cmd = IOS_CMD_READ;
  block->fd = fd;
  block->read.data = util::Physical(data);
  block->read.size = size;
  block->queue = &queue;

  ipcAsync(block);

  return IOSError::IOS_ERROR_OK;
}

s32 IOS_WriteAsync(s32 fd, void *data, s32 size,
                   host::MessageQueue<IPCCommandBlock *> &queue,
                   IPCCommandBlock *block) noexcept {
  if (!util::IsAligned(Alignment, block)) {
    block->result = IOSError::IOS_ERROR_INVALID;
    return IOSError::IOS_ERROR_INVALID;
  }

  *block = {};
  block->cmd = IOS_CMD_WRITE;
  block->fd = fd;
  block->write.data = util::Physical(data);
  block->write.size = size;
  block->queue = &queue;

  util::CpuCache::DcFlush(data, size);

  ipcAsync(block);

  return IOSError::IOS_ERROR_OK;
}

s32 IOS_IoctlAsync(s32 fd, u32 command, void *in, u32 in_size, void *out,
                   u32 out_size, host::MessageQueue<IPCCommandBlock *> &queue,
                   IPCCommandBlock *block) noexcept {
  if (!util::IsAligned(Alignment, block)) {
    block->result = IOSError::IOS_ERROR_INVALID;
    return IOSError::IOS_ERROR_INVALID;
  }

  if (in_size != 0) {
    util::CpuCache::DcFlush(in, in_size);
  }
  if (out_size != 0) {
    util::CpuCache::DcFlush(out, out_size);
  }

  *block = {};
  block->cmd = IOS_CMD_IOCTL;
  block->fd = fd;
  block->ioctl.cmd = command;
  block->ioctl.in = util::Physical(in);
  block->ioctl.in_size = in_size;
  block->ioctl.out = util::Physical(out);
  block->ioctl.out_size = out_size;
  block->queue = &queue;

  ipcAsync(block);

  return IOSError::IOS_ERROR_OK;
}

s32 IOS_IoctlvAsync(s32 fd, u32 command, u32 in_count, u32 out_count,
                    IOVector *vec, host::MessageQueue<IPCCommandBlock *> &queue,
                    IPCCommandBlock *block) noexcept {
  if (!util::IsAligned(Alignment, block)) {
    block->result = IOSError::IOS_ERROR_INVALID;
    return IOSError::IOS_ERROR_INVALID;
  }

  for (u32 i = 0; i < in_count + out_count; i++) {
    if (vec[i].size != 0) {
      util::CpuCache::DcFlush(vec[i].data, vec[i].size);
      vec[i].data = util::Physical(vec[i].data);
    }
  }
  util::CpuCache::DcFlush(vec, (in_count + out_count) * sizeof(IOVector));

  *block = {};
  block->cmd = IOS_CMD_IOCTLV;
  block->fd = fd;
  block->ioctlv.cmd = command;
  block->ioctlv.in_count = in_count;
  block->ioctlv.out_count = out_count;
  block->ioctlv.vec = util::Physical(vec);
  block->queue = &queue;

  ipcAsync(block);

  return IOSError::IOS_ERROR_OK;
}

void Init() noexcept {
  // TODO: It would be nice if MessageQueue had a (stub) constexpr constructor,
  // so we could have a constinit object and then reconstruct it here. Note
  // host::MessageQueue constructor cannot be constexpr due to incompatibilities
  // with other hosts (such as IOS) which need a syscall to create a message
  // queue. On the other paw, this code is only really for PPC isn't it?
  static host::MessageQueue<IPCCommandBlock *, 1> s_local_queue_send;
  s_queue_send = &s_local_queue_send;

  runtime::SetIrqHandler(hw::Irq::IpcPpc, ipcHandleInterrupt);

  hw::WOOD->IPCPPCCTRL = Y1 | Y2;
}

#endif // !PELI_HOST_IOS

} // namespace peli::ios::low