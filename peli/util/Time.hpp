#pragma once

#include "../cmn/Types.hpp"
#include "../ppc/Spr.hpp"

namespace peli::util {

constexpr u64 BusClock = 243000000ull;  // 243 MHz
constexpr u64 CoreClock = 729000000ull; // 729 MHz

inline u64 GetTime() {
  u32 time_low, time_high;
  do {
    time_high = ppc::MoveFrom<ppc::Spr::TBU>();
    time_low = ppc::MoveFrom<ppc::Spr::TBL>();
  } while (time_high != ppc::MoveFrom<ppc::Spr::TBU>());
  return (static_cast<u64>(time_high) << 32) | time_low;
}

inline void TimeBaseDelay(u64 tb_ticks) noexcept {
  u64 start = GetTime();
  while ((GetTime() - start) < tb_ticks) {
  }
}

inline void BusDelay(u64 bus_ticks) noexcept { TimeBaseDelay(bus_ticks / 4); }

inline void DelayMilliseconds(u64 ms) noexcept {
  TimeBaseDelay(ms * (BusClock / 4000));
}

} // namespace peli::util