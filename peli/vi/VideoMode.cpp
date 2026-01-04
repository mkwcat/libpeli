// peli/vi/VideoMode.cpp - Video mode definitions
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "Video.hpp"
#include "peli/cmn/Macro.h"

namespace peli::vi {

constinit const decltype(Video::TimingSets) Video::TimingSets = {
    .Interlaced{
        .NtscEurgb60{
            .HTR0 = {.HCS = 71, .HCE = 105, .HLW = 429},
            .HTR1 = {.HBS = 373, .HBE = 162, .HSY = 64},
            .VTR = {.ACV = 240, .EQU = 6},
            .VTO = {.PSB = 3, .PRB = 24},
            .VTE = {.PSB = 2, .PRB = 25},
            .BBEI = {.BE3 = 520, .BS3 = 12, .BE1 = 520, .BS1 = 12},
            .BBOI = {.BE4 = 519, .BS4 = 13, .BE2 = 519, .BS2 = 13},
            .HBE = {.HBE656 = 122},
            .HBS = {.HBS656 = 412},
            .DI = {.ENB = 1, .VCT = 525 / 2 + 1, .HCT = 429 + 1},
        },
        .Pal{
            .HTR0 = {.HCS = 75, .HCE = 106, .HLW = 432},
            .HTR1 = {.HBS = 380, .HBE = 172, .HSY = 64},
            .VTR = {.ACV = 287, .EQU = 5},
            .VTO = {.PSB = 1, .PRB = 35},
            .VTE = {.PSB = 0, .PRB = 36},
            .BBEI = {.BE3 = 617, .BS3 = 11, .BE1 = 619, .BS1 = 13},
            .BBOI = {.BE4 = 620, .BS4 = 10, .BE2 = 618, .BS2 = 12},
            .HBE = {.HBE656 = 133},
            .HBS = {.HBS656 = 420},
            .DI = {.ENB = 1, .VCT = 625 / 2 + 1, .HCT = 432 + 1},
        },
        .PalM{
            .HTR0 = {.HCS = 78, .HCE = 112, .HLW = 429},
            .HTR1 = {.HBS = 373, .HBE = 162, .HSY = 64},
            .VTR = {.ACV = 240, .EQU = 6},
            .VTO = {.PSB = 3, .PRB = 24},
            .VTE = {.PSB = 2, .PRB = 25},
            .BBEI = {.BE3 = 516, .BS3 = 14, .BE1 = 518, .BS1 = 16},
            .BBOI = {.BE4 = 519, .BS4 = 13, .BE2 = 517, .BS2 = 15},
            .HBE = {.HBE656 = 122},
            .HBS = {.HBS656 = 412},
            .DI = {.ENB = 1, .VCT = 525 / 2 + 1, .HCT = 429 + 1},
        },
        .Gca{
            .HTR0 = {.HCS = 71, .HCE = 105, .HLW = 429},
            .HTR1 = {.HBS = 370, .HBE = 159, .HSY = 64},
            .VTR = {.ACV = 241, .EQU = 6},
            .VTO = {.PSB = 1, .PRB = 24},
            .VTE = {.PSB = 0, .PRB = 25},
            .BBEI = {.BE3 = 520, .BS3 = 12, .BE1 = 520, .BS1 = 12},
            .BBOI = {.BE4 = 519, .BS4 = 13, .BE2 = 519, .BS2 = 13},
            .HBE = {.HBE656 = 122},
            .HBS = {.HBS656 = 412},
            .DI = {.ENB = 1, .VCT = 525 / 2 + 1, .HCT = 429 + 1},
        },
    },
    .NonInterlaced{
        .NtscEurgb60{
            .HTR0 = {.HCS = 71, .HCE = 105, .HLW = 429},
            .HTR1 = {.HBS = 373, .HBE = 162, .HSY = 64},
            .VTR = {.ACV = 240, .EQU = 6},
            .VTO = {.PSB = 4, .PRB = 24},
            .VTE = {.PSB = 4, .PRB = 24},
            .BBEI = {.BE3 = 520, .BS3 = 12, .BE1 = 520, .BS1 = 12},
            .BBOI = {.BE4 = 519, .BS4 = 13, .BE2 = 519, .BS2 = 13},
            .HBE = {.HBE656 = 122},
            .HBS = {.HBS656 = 412},
            .DI = {.ENB = 1, .VCT = 526 / 2 + 1, .HCT = 429 + 1},
        },
        .Pal{
            .HTR0 = {.HCS = 75, .HCE = 106, .HLW = 432},
            .HTR1 = {.HBS = 380, .HBE = 172, .HSY = 64},
            .VTR = {.ACV = 287, .EQU = 5},
            .VTO = {.PSB = 2, .PRB = 33},
            .VTE = {.PSB = 2, .PRB = 33},
            .BBEI = {.BE3 = 619, .BS3 = 13, .BE1 = 619, .BS1 = 13},
            .BBOI = {.BE4 = 621, .BS4 = 11, .BE2 = 621, .BS2 = 11},
            .HBE = {.HBE656 = 133},
            .HBS = {.HBS656 = 420},
            .DI = {.ENB = 1, .VCT = 624 / 2 + 1, .HCT = 432 + 1},
        },
        .PalM{
            .HTR0 = {.HCS = 78, .HCE = 112, .HLW = 429},
            .HTR1 = {.HBS = 373, .HBE = 162, .HSY = 64},
            .VTR = {.ACV = 240, .EQU = 6},
            .VTO = {.PSB = 4, .PRB = 24},
            .VTE = {.PSB = 4, .PRB = 24},
            .BBEI = {.BE3 = 518, .BS3 = 16, .BE1 = 518, .BS1 = 16},
            .BBOI = {.BE4 = 520, .BS4 = 14, .BE2 = 520, .BS2 = 14},
            .HBE = {.HBE656 = 122},
            .HBS = {.HBS656 = 412},
            .DI = {.ENB = 1, .VCT = 526 / 2 + 1, .HCT = 429 + 1},
        },
    },
    .Progressive{
        .NtscPalMEurgb60{
            .HTR0 = {.HCS = 71, .HCE = 105, .HLW = 429},
            .HTR1 = {.HBS = 373, .HBE = 162, .HSY = 64},
            .VTR = {.ACV = 480, .EQU = 12},
            .VTO = {.PSB = 6, .PRB = 48},
            .VTE = {.PSB = 6, .PRB = 48},
            .BBEI = {.BE3 = 1038, .BS3 = 24, .BE1 = 1038, .BS1 = 24},
            .BBOI = {.BE4 = 1038, .BS4 = 24, .BE2 = 1038, .BS2 = 24},
            .HBE = {.HBE656 = 122},
            .HBS = {.HBS656 = 412},
            .DI = {.ENB = 1, .VCT = 1050 / 2 + 1, .HCT = 429 + 1},
        },
        .Gca{
            .HTR0 = {.HCS = 71, .HCE = 105, .HLW = 429},
            .HTR1 = {.HBS = 391, .HBE = 180, .HSY = 64},
            .VTR = {.ACV = 480, .EQU = 12},
            .VTO = {.PSB = 6, .PRB = 48},
            .VTE = {.PSB = 6, .PRB = 48},
            .BBEI = {.BE3 = 1038, .BS3 = 24, .BE1 = 1038, .BS1 = 24},
            .BBOI = {.BE4 = 1038, .BS4 = 24, .BE2 = 1038, .BS2 = 24},
            .HBE = {.HBE656 = 122},
            .HBS = {.HBS656 = 412},
            .DI = {.ENB = 1, .VCT = 1050 / 2 + 1, .HCT = 429 + 1},
        },
        .Pal{
            .HTR0 = {.HCS = 75, .HCE = 106, .HLW = 432},
            .HTR1 = {.HBS = 380, .HBE = 172, .HSY = 64},
            .VTR = {.ACV = 576, .EQU = 10},
            .VTO = {.PSB = 6, .PRB = 62},
            .VTE = {.PSB = 6, .PRB = 62},
            .BBEI = {.BE3 = 1240, .BS3 = 20, .BE1 = 1240, .BS1 = 20},
            .BBOI = {.BE4 = 1240, .BS4 = 20, .BE2 = 1240, .BS2 = 20},
            .HBE = {.HBE656 = 122},
            .HBS = {.HBS656 = 420},
            .DI = {.ENB = 1, .VCT = 1250 / 2 + 1, .HCT = 432 + 1},
        },
    },
    .Ntsc3D{
        .HTR0 = {.HCS = 71, .HCE = 105, .HLW = 429},
        .HTR1 = {.HBS = 379, .HBE = 168, .HSY = 64},
        .VTR = {.ACV = 480, .EQU = 12},
        .VTO = {.PSB = 10, .PRB = 44},
        .VTE = {.PSB = 10, .PRB = 44},
        .BBEI = {.BE3 = 1038, .BS3 = 24, .BE1 = 1038, .BS1 = 24},
        .BBOI = {.BE4 = 1038, .BS4 = 24, .BE2 = 1038, .BS2 = 24},
        .HBE = {.HBE656 = 122},
        .HBS = {.HBS656 = 412},
        .DI = {.ENB = 1, .VCT = 1050 / 2 + 1, .HCT = 429 + 1},
    },
};

_PELI_DIAGNOSTIC_CLANG(push)
_PELI_DIAGNOSTIC_CLANG(ignored "-Wmissing-braces")

constinit const decltype(Video::RenderModes) Video::RenderModes = {
    .Ntsc{
        .Interlaced{
            .mode = {.standard = Standard::Ntsc,
                     .interlace_mode = InterlaceMode::Interlace},
            .fb_width = 608,
            .efb_height = 456,
            .xfb_height = 456,
            .vi_x_origin = 17,
            .vi_y_origin = 12,
            .vi_width = 686,
            .vi_height = 456,
            .field_rendering = 0,
            .antialiasing = 0,
            .sample_pattern = {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
                               6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
            .vertical_filter = {7, 7, 12, 12, 12, 7, 7},
        },
        .NonInterlaced{},
        .Progressive{
            .mode = {.standard = Standard::Ntsc,
                     .interlace_mode = InterlaceMode::Progressive},
            .fb_width = 608,
            .efb_height = 456,
            .xfb_height = 456,
            .vi_x_origin = 17,
            .vi_y_origin = 12,
            .vi_width = 686,
            .vi_height = 456,
            .field_rendering = 0,
            .antialiasing = 0,
            .sample_pattern = {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
                               6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
            .vertical_filter = {0, 0, 21, 22, 21, 0, 0},
        },
    },
    .Pal{
        .Interlaced{},
        .NonInterlaced{},
        .Progressive{},
    },
    .PalM{
        .Interlaced{},
        .NonInterlaced{},
        .Progressive{},
    },
    .Eurgb60{
        .Interlaced{},
        .NonInterlaced{},
        .Progressive{},
    },
};

_PELI_DIAGNOSTIC_CLANG(pop)

const Video::Timings &Video::TVMode::GetTimings() const noexcept {
  if (interlace_mode == InterlaceMode::Interlace) {
    if (standard == Standard::Pal) {
      return TimingSets.Interlaced.Pal;
    } else if (standard == Standard::PalM) {
      return TimingSets.Interlaced.PalM;
    } else if (standard == Standard::Gca) {
      return TimingSets.Interlaced.Gca;
    }
    return TimingSets.Interlaced.NtscEurgb60;
  } else if (interlace_mode == InterlaceMode::NonInterlace) {
    if (standard == Standard::Pal) {
      return TimingSets.NonInterlaced.Pal;
    } else if (standard == Standard::PalM) {
      return TimingSets.NonInterlaced.PalM;
    }
    return TimingSets.NonInterlaced.NtscEurgb60;
  } else if (interlace_mode == InterlaceMode::Progressive) {
    if (standard == Standard::Gca) {
      return TimingSets.Progressive.Gca;
    } else if (standard == Standard::Pal) {
      return TimingSets.Progressive.Pal;
    }
    return TimingSets.Progressive.NtscPalMEurgb60;
  }
  // Default to interlaced NTSC/EURGB60
  return TimingSets.Interlaced.NtscEurgb60;
}

const Video::RenderMode &Video::TVMode::GetRenderMode() const noexcept {
  const auto *standard_struct = &RenderModes.Ntsc;
  if (standard != Standard::Ntsc) {
    if (standard == Standard::Pal) {
      standard_struct = &RenderModes.Pal;
    } else if (standard == Standard::Eurgb60) {
      standard_struct = &RenderModes.Eurgb60;
    } else if (standard == Standard::PalM) {
      standard_struct = &RenderModes.PalM;
    }
  }

  switch (interlace_mode) {
  default:
  case InterlaceMode::Interlace:
    return standard_struct->Interlaced;
  case InterlaceMode::NonInterlace:
    return standard_struct->NonInterlaced;
  case InterlaceMode::Progressive:
    return standard_struct->Progressive;
  }
}

Video::TVMode Video::TVMode::Detect() noexcept {
  // TODO: Check SYSCONF for actual configuration (rather than just checking the
  // previous video configuration)
  Video::TVMode mode;

  switch (hw::VI->DCR.FMT) {
  default:
    mode.standard = Standard::Ntsc;
    break;
  case VI::Dcr::Fmt::PAL:
    mode.standard = Standard::Pal;
    break;
  case VI::Dcr::Fmt::MPAL:
    mode.standard = Standard::PalM;
    break;
  case VI::Dcr::Fmt::DEBUG:
    mode.standard = Standard::Debug;
    break;
  }

  if (hw::VI->VISEL.VISEL & 1) {
    mode.interlace_mode = InterlaceMode::Progressive;
  } else if (hw::VI->DCR.NIN) {
    mode.interlace_mode = InterlaceMode::NonInterlace;
  } else {
    mode.interlace_mode = InterlaceMode::Interlace;
  }

  return mode;
}

} // namespace peli::vi
