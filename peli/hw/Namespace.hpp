// peli/hw/Namespace.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../host/Config.h"

namespace hw {

#if defined(PELI_HOST_PPC)
#define PELI_HW_HOST_NAME ppc
#elif defined(PELI_HOST_IOS) || defined(PELI_HOST_ARM)
#define PELI_HW_HOST_NAME iop
#endif

} // namespace hw