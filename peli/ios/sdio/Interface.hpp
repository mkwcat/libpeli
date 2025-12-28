// peli/ios/sdio/Interface.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../../cmn/Types.hpp"
#include "../Interface.hpp"
#include "Ioctl.hpp"
#include "Types.hpp"

namespace peli::ios::sdio {

struct Interface : public ios::Interface<Ioctl> {
  using WriteHcReg =                   //
      IoctlDef<Ioctl::SD_IOWHCREG,     //
               In<HcRegOp>,            //
               Out<>>;                 //
  using ReadHcReg =                    //
      IoctlDef<Ioctl::SD_IORHCREG,     //
               In<HcRegOp>,            //
               Out<u32>>;              //
  using Reset =                        //
      IoctlDef<Ioctl::SD_RESET,        //
               In<>,                   //
               Out<u32>>;              //
  using SetClock =                     //
      IoctlDef<Ioctl::SD_SETCLK,       //
               In<u32>,                //
               Out<>>;                 //
  using ImmCommand =                   //
      IoctlDef<Ioctl::SD_CMD,          //
               In<Command>,            //
               Out<u32[4]>>;           //
  using BufCommand =                   //
      IoctlVecDef<Ioctl::SD_CMD,       //
                  In<Command, void *>, //
                  Out<u32[4]>>;        //
  using GetStatus =                    //
      IoctlDef<Ioctl::SD_GET_STATUS,   //
               In<>,                   //
               Out<Status>>;           //
  using GetOcr =                       //
      IoctlDef<Ioctl::SD_GET_OCR,      //
               In<>,                   //
               Out<u32>>;              //
};

} // namespace peli::ios::sdio
