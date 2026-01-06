// bindings/ogc/include/ogcsys.h
//   Written by mkwcat
//
// Copyright (c) 2026 mkwcat
// SPDX-License-Identifier: MIT

#if !defined(__OGCSYS_H__)
#define __OGCSYS_H__

// This header sucks
#include "gccore.h"

enum {
  TB_BUS_CLOCK = 243000000u,
  TB_CORE_CLOCK = 729000000u,

  TB_TIMER_CLOCK = (TB_BUS_CLOCK / 4000),

  TB_MSPERSEC = 1000,
  TB_USPERSEC = 1000000,
  TB_NSPERSEC = 1000000000,
  TB_NSPERMS = 1000000,
  TB_NSPERUS = 1000,
  TB_USPERTICK = 10000,

  TB_SECSPERMIN = 60,
  TB_MINSPERHR = 60,
  TB_MONSPERYR = 12,
  TB_DAYSPERYR = 365,
  TB_HRSPERDAY = 24,
  TB_SECSPERDAY = (TB_SECSPERMIN * TB_MINSPERHR * TB_HRSPERDAY),
  TB_SECSPERNYR = (365 * TB_SECSPERDAY),
};

#endif // __OGCSYS_H__