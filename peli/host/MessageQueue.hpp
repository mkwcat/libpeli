// peli/host/MessageQueue.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../rt/MessageQueue.hpp"
#include "Config.h"

namespace peli::host {

#if defined(PELI_HOST_PPC)

template <class MessageType = s32, u32 Count = 0>
using MessageQueue = rt::MessageQueue<MessageType, Count>;

#endif // PELI_HOST_PPC

} // namespace peli::host
