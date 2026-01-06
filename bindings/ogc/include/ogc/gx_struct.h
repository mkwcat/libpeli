// bindings/ogc/include/ogc/gx_struct.h
//   Written by mkwcat
//
// Copyright (c) 2026 mkwcat
// SPDX-License-Identifier: MIT

#if !defined(__GX_STRUCT_H__)
#define __GX_STRUCT_H__

#include "gctypes.h"

struct _gx_rmodeobj {
  u32 viTVMode;
  u16 fbWidth;
  u16 efbHeight;
  u16 xfbHeight;
  u16 viXOrigin;
  u16 viYOrigin;
  u16 viWidth;
  u16 viHeight;
  u32 xfbMode;
  u8 field_rendering;
  u8 aa;
  u8 sample_pattern[12][2];
  u8 vfilter[7];
};

#if defined(__cplusplus)
extern "C" {
namespace ogc {
#endif

typedef struct _gx_rmodeobj GXRModeObj;

#if defined(__cplusplus)
} // namespace ogc
using namespace ogc;
} // extern "C"
#endif

#endif // __GX_STRUCT_H__