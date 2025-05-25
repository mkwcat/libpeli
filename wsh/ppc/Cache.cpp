// wsh/ppc/Cache.cpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#include "Cache.hpp"

namespace wsh::ppc::Cache {

template <bool DoSync> void DcStore(const void *start, u32 size) noexcept {
  OpRange<Op::DcStore, DoSync>(start, size);
}

template void DcStore<true>(const void *start, u32 size) noexcept;
template void DcStore<false>(const void *start, u32 size) noexcept;

template <bool DoSync> void DcFlush(const void *start, u32 size) noexcept {
  OpRange<Op::DcFlush, DoSync>(start, size);
}

template void DcFlush<true>(const void *start, u32 size) noexcept;
template void DcFlush<false>(const void *start, u32 size) noexcept;

template <bool DoSync> void DcInvalidate(const void *start, u32 size) noexcept {
  OpRange<Op::DcInvalidate, DoSync>(start, size);
}

template void DcInvalidate<true>(const void *start, u32 size) noexcept;
template void DcInvalidate<false>(const void *start, u32 size) noexcept;

template <bool DoSync> void DcZero(const void *start, u32 size) noexcept {
  OpRange<Op::DcZero, DoSync>(start, size);
}

template void DcZero<true>(const void *start, u32 size) noexcept;
template void DcZero<false>(const void *start, u32 size) noexcept;

template <bool DoSync> void DcTouch(const void *start, u32 size) noexcept {
  OpRange<Op::DcTouch, DoSync>(start, size);
}

template void DcTouch<true>(const void *start, u32 size) noexcept;
template void DcTouch<false>(const void *start, u32 size) noexcept;

template <bool DoSync> void DcTouchStore(const void *start, u32 size) noexcept {
  OpRange<Op::DcTouchStore, DoSync>(start, size);
}

template void DcTouchStore<true>(const void *start, u32 size) noexcept;
template void DcTouchStore<false>(const void *start, u32 size) noexcept;

template <bool DoSync> void IcInvalidate(const void *start, u32 size) noexcept {
  OpRange<Op::IcInvalidate, DoSync>(start, size);
}

template void IcInvalidate<true>(const void *start, u32 size) noexcept;
template void IcInvalidate<false>(const void *start, u32 size) noexcept;

} // namespace wsh::ppc::Cache