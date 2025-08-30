// peli/hw/VideoMode.cpp - Video mode definitions
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "Video.hpp"

namespace peli::hw {

constinit const Video::Timings //
    Video::TimingsInterlacedNtscEurgb60 = {
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
};

constinit const Video::Timings //
    Video::TimingsNonInterlacedNtscEurgb60 = {
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
};

constinit const Video::Timings //
    Video::TimingsInterlacedPal = {
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
};

constinit const Video::Timings //
    Video::TimingsNonInterlacedPal = {
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
};

constinit const Video::Timings //
    Video::TimingsInterlacedPalM = {
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
};

constinit const Video::Timings //
    Video::TimingsNonInterlacedPalM = {
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
};

constinit const Video::Timings //
    Video::TimingsProgressiveNtscPalMEurgb60 = {
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
};

constinit const Video::Timings //
    Video::TimingsNtsc3D = {
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
};

constinit const Video::Timings //
    Video::TimingsInterlacedGca = {
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
};

constinit const Video::Timings //
    Video::TimingsProgressiveGca = {
        .HTR0 = {.HCS = 71, .HCE = 105, .HLW = 429},
        .HTR1 = {.HBS = 391, .HBE = 180, .HSY = 64},
        .VTR = {.ACV = 480, .EQU = 12},
        .VTO = {.PSB = 6, .PRB = 48},
        .VTE = {.PSB = 6, .PRB = 48},
        .BBEI = {.BE3 = 1038, .BS3 = 24, .BE1 = 1038, .BS1 = 24},
        .BBOI = {.BE4 = 1038, .BS4 = 24, .BE2 = 1038, .BS2 = 24},
        .HBE = {.HBE656 = 122},
        .HBS = {.HBS656 = 412},
        .DI = {.ENB = 1, .VCT = 1050 / 2 + 1, .HCT = 429 + 1}};

constinit const Video::Timings //
    Video::TimingsProgressivePal = {
        .HTR0 = {.HCS = 75, .HCE = 106, .HLW = 432},
        .HTR1 = {.HBS = 380, .HBE = 172, .HSY = 64},
        .VTR = {.ACV = 576, .EQU = 10},
        .VTO = {.PSB = 6, .PRB = 62},
        .VTE = {.PSB = 6, .PRB = 62},
        .BBEI = {.BE3 = 1240, .BS3 = 20, .BE1 = 1240, .BS1 = 20},
        .BBOI = {.BE4 = 1240, .BS4 = 20, .BE2 = 1240, .BS2 = 20},
        .HBE = {.HBE656 = 122},
        .HBS = {.HBS656 = 420},
        .DI = {.ENB = 1, .VCT = 1250 / 2 + 1, .HCT = 432 + 1}};

constinit const Video::RenderMode Video::RenderModeNtscInterlaced = {
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
};

constinit const Video::RenderMode Video::RenderModeNtscNonInterlaced = {};

constinit const Video::RenderMode Video::RenderModeNtscProgressive = {
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
};

constinit const Video::RenderMode Video::RenderModePalInterlaced = {};
constinit const Video::RenderMode Video::RenderModePalNonInterlaced = {};
constinit const Video::RenderMode Video::RenderModePalProgressive = {};
constinit const Video::RenderMode Video::RenderModePalMInterlaced = {};
constinit const Video::RenderMode Video::RenderModePalMNonInterlaced = {};
constinit const Video::RenderMode Video::RenderModePalMProgressive = {};
constinit const Video::RenderMode Video::RenderModeEurgb60Interlaced = {};
constinit const Video::RenderMode Video::RenderModeEurgb60NonInterlaced = {};
constinit const Video::RenderMode Video::RenderModeEurgb60Progressive = {};

const Video::Timings &Video::TVMode::GetTimings() const noexcept {
  if (interlace_mode == InterlaceMode::Interlace) {
    if (standard == Standard::Pal) {
      return TimingsInterlacedPal;
    } else if (standard == Standard::PalM) {
      return TimingsInterlacedPalM;
    } else if (standard == Standard::Gca) {
      return TimingsInterlacedGca;
    }
    return TimingsInterlacedNtscEurgb60;
  } else if (interlace_mode == InterlaceMode::NonInterlace) {
    if (standard == Standard::Pal) {
      return TimingsNonInterlacedPal;
    } else if (standard == Standard::PalM) {
      return TimingsNonInterlacedPalM;
    }
    return TimingsNonInterlacedNtscEurgb60;
  } else if (interlace_mode == InterlaceMode::Progressive) {
    if (standard == Standard::Gca) {
      return TimingsProgressiveGca;
    } else if (standard == Standard::Pal) {
      return TimingsProgressivePal;
    }
    return TimingsProgressiveNtscPalMEurgb60;
  }
  // Default to interlaced NTSC/EURGB60
  return TimingsInterlacedNtscEurgb60;
}

const Video::RenderMode &Video::TVMode::GetRenderMode() const noexcept {
  if (interlace_mode == InterlaceMode::Interlace) {
    if (standard == Standard::Pal) {
      return RenderModePalInterlaced;
    } else if (standard == Standard::PalM) {
      return RenderModePalMInterlaced;
    } else if (standard == Standard::Eurgb60) {
      return RenderModeEurgb60Interlaced;
    }
    return RenderModeNtscInterlaced;
  } else if (interlace_mode == InterlaceMode::NonInterlace) {
    if (standard == Standard::Pal) {
      return RenderModePalNonInterlaced;
    } else if (standard == Standard::PalM) {
      return RenderModePalMNonInterlaced;
    } else if (standard == Standard::Eurgb60) {
      return RenderModeEurgb60NonInterlaced;
    }
    return RenderModeNtscNonInterlaced;
  } else if (interlace_mode == InterlaceMode::Progressive) {
    if (standard == Standard::Pal) {
      return RenderModePalProgressive;
    } else if (standard == Standard::PalM) {
      return RenderModePalMProgressive;
    } else if (standard == Standard::Eurgb60) {
      return RenderModeEurgb60Progressive;
    }
    return RenderModeNtscProgressive;
  }
  // Default to interlaced NTSC
  return RenderModeNtscInterlaced;
}

Video::TVMode Video::TVMode::Detect() noexcept {
  // TODO: Check SYSCONF for actual configuration (rather than just checking the
  // previous video configuration)
  Video::TVMode mode;

  switch (VI->DCR.FMT) {
  default:
    mode.standard = Standard::Ntsc;
    break;
  case Dcr::Fmt::PAL:
    mode.standard = Standard::Pal;
    break;
  case Dcr::Fmt::MPAL:
    mode.standard = Standard::PalM;
    break;
  case Dcr::Fmt::DEBUG:
    mode.standard = Standard::Debug;
    break;
  }

  if (VI->VISEL.VISEL & 1) {
    mode.interlace_mode = InterlaceMode::Progressive;
  } else if (VI->DCR.NIN) {
    mode.interlace_mode = InterlaceMode::NonInterlace;
  } else {
    mode.interlace_mode = InterlaceMode::Interlace;
  }

  return mode;
}

} // namespace peli::hw