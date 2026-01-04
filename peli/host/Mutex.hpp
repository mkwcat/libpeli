// peli/host/Mutex.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../rt/Mutex.hpp"
#include "Config.h"

namespace peli::host {

#if defined(PELI_HOST_PPC)

using Mutex = rt::Mutex;
using RecursiveMutex = rt::RecursiveMutex;

#endif // PELI_HOST_PPC

} // namespace peli::host