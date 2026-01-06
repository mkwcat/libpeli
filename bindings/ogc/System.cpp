// bindings/ogc/System.cpp
//   Written by mkwcat
//
// Copyright (c) 2026 mkwcat
// SPDX-License-Identifier: MIT

#include "ogc/system.h"
#include <cstdlib>

void *ogc::SYS_AllocateFramebuffer(GXRModeObj *rmode) {
  size_t size = (rmode->xfbHeight & 0x3FF) * (rmode->fbWidth & 0x7F0u);
  return std::aligned_alloc(0x4000, size);
}
