// peli/ios/sdio/HcReg.hpp - SD Host Controller register I/O
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../../hw/SDHostController.hpp"
#include "../../util/Bit.hpp"
#include "Interface.hpp"
#include "Types.hpp"

namespace peli::ios::sdio {

struct HcReg {
public:
  template <class T>
  [[nodiscard]]
  static inline Interface::WriteHcReg::Request
  Write(s32 fd, volatile T hw::SDHostController::*reg, const T &value) {
    return Interface::WriteHcReg::Request(fd, HcRegOp{
                                                  .reg = regOffset(reg),
                                                  .size = sizeof(T),
                                                  .value = valueCast(value),
                                              });
  }

  template <class T>
  [[nodiscard]]
  static inline Interface::WriteHcReg::Request
  Write(s32 fd, volatile hw::Register<T> hw::SDHostController::*reg,
        const T &value) {
    return Interface::WriteHcReg::Request(fd, HcRegOp{
                                                  .reg = regOffset(reg),
                                                  .size = sizeof(T),
                                                  .value = valueCast(value),
                                              });
  }

  template <class T>
  [[nodiscard]]
  static inline Interface::ReadHcReg::Request
  Read(s32 fd, volatile T hw::SDHostController::*reg) {
    return Interface::ReadHcReg::Request(fd, HcRegOp{
                                                 .reg = regOffset(reg),
                                                 .size = sizeof(T),
                                                 .value = 0,
                                             });
  }

  template <class T>
  [[nodiscard]]
  static inline IOSError Mask(s32 fd, volatile T hw::SDHostController::*reg,
                              const T &clear, const T &set) {
    u32 value;
    if (IOSError error = Read(fd, reg) //
                             .Sync()
                             .CopyOutput(value)
                             .GetError()) {
      return error;
    }

    value &= ~valueCast(clear);
    value |= valueCast(set);

    return Write(fd, reg, reverseValueCast<T>(value)) //
        .Sync()
        .GetError();
  }

  template <class T>
  [[nodiscard]]
  static inline IOSError
  Mask(s32 fd, volatile hw::Register<T> hw::SDHostController::*reg,
       const T &clear, const T &set) {
    u32 value;
    if (IOSError error = Read(fd, reg) //
                             .Sync()
                             .CopyOutput(value)
                             .GetError()) {
      return error;
    }

    value &= ~valueCast(clear);
    value |= valueCast(set);

    return Write(fd, reg, reverseValueCast<T>(value)) //
        .Sync()
        .GetError();
  }

private:
  template <class T>
  static inline u32 regOffset(volatile T hw::SDHostController::*reg) {
    return (&(static_cast<hw::SDHostController *>(nullptr)->*reg) -
            static_cast<T *>(nullptr)) *
           sizeof(T);
  }

  template <class T> static consteval auto integralForSize() {
    if constexpr (sizeof(T) == 1) {
      return u8();
    } else if constexpr (sizeof(T) == 2) {
      return u16();
    } else {
      return u32();
    }
  }

  template <class T> static constexpr inline u32 valueCast(const T &value) {
    return util::BitCast<decltype(integralForSize<T>())>(value);
  }

  template <class T> static constexpr inline T reverseValueCast(u32 value) {
    return util::BitCast<T>(static_cast<decltype(integralForSize<T>())>(value));
  }
};

} // namespace peli::ios::sdio