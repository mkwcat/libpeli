// wsh/host/MessageQueue.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../runtime/MessageQueue.hpp"

namespace wsh::host {

#ifdef WSH_HOST_PPC

template <class MessageType = s32, u32 Count = 0>
using MessageQueue = runtime::MessageQueue<MessageType, Count>;

#endif // WSH_HOST_PPC

} // namespace wsh::host