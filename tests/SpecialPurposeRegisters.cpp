// SpecialPurposeRegisters.cpp - Dump values of SPRs

#include <cstdio>
#include <iterator>
#include <peli/common/Types.hpp>
#include <peli/ppc/Spr.hpp>
#include <peli/util/VIConsole.hpp>
#include <peli/util/VIConsoleStdOut.hpp>
#include <utility>

namespace {

template <peli::ppc::Spr SPR> constexpr peli::u32 G() {
  return peli::ppc::GetSpr<SPR>();
}

constexpr std::pair<peli::u32 (*)(), const char *> s_spr_list[] = {
    {G<peli::ppc::Spr::XER>, "XER"},
    {G<peli::ppc::Spr::LR>, "LR"},
    {G<peli::ppc::Spr::CTR>, "CTR"},
    {G<peli::ppc::Spr::DSISR>, "DSISR"},
    {G<peli::ppc::Spr::DAR>, "DAR"},
    {G<peli::ppc::Spr::DEC>, "DEC"},
    {G<peli::ppc::Spr::SDR1>, "SDR1"},
    {G<peli::ppc::Spr::SRR0>, "SRR0"},
    {G<peli::ppc::Spr::SRR1>, "SRR1"},
    {G<peli::ppc::Spr::TBL>, "TBL"},
    {G<peli::ppc::Spr::TBU>, "TBU"},
    {G<peli::ppc::Spr::TBLW>, "TBLW"},
    {G<peli::ppc::Spr::TBUW>, "TBUW"},
    {G<peli::ppc::Spr::PVR>, "PVR"},
    {G<peli::ppc::Spr::IBAT0U>, "IBAT0U"},
    {G<peli::ppc::Spr::IBAT0L>, "IBAT0L"},
    {G<peli::ppc::Spr::IBAT1U>, "IBAT1U"},
    {G<peli::ppc::Spr::IBAT1L>, "IBAT1L"},
    {G<peli::ppc::Spr::IBAT2U>, "IBAT2U"},
    {G<peli::ppc::Spr::IBAT2L>, "IBAT2L"},
    {G<peli::ppc::Spr::IBAT3U>, "IBAT3U"},
    {G<peli::ppc::Spr::IBAT3L>, "IBAT3L"},
    {G<peli::ppc::Spr::DBAT0U>, "DBAT0U"},
    {G<peli::ppc::Spr::DBAT0L>, "DBAT0L"},
    {G<peli::ppc::Spr::DBAT1U>, "DBAT1U"},
    {G<peli::ppc::Spr::DBAT1L>, "DBAT1L"},
    {G<peli::ppc::Spr::DBAT2U>, "DBAT2U"},
    {G<peli::ppc::Spr::DBAT2L>, "DBAT2L"},
    {G<peli::ppc::Spr::DBAT3U>, "DBAT3U"},
    {G<peli::ppc::Spr::DBAT3L>, "DBAT3L"},
    {G<peli::ppc::Spr::IBAT4U>, "IBAT4U"},
    {G<peli::ppc::Spr::IBAT4L>, "IBAT4L"},
    {G<peli::ppc::Spr::IBAT5U>, "IBAT5U"},
    {G<peli::ppc::Spr::IBAT5L>, "IBAT5L"},
    {G<peli::ppc::Spr::IBAT6U>, "IBAT6U"},
    {G<peli::ppc::Spr::IBAT6L>, "IBAT6L"},
    {G<peli::ppc::Spr::IBAT7U>, "IBAT7U"},
    {G<peli::ppc::Spr::IBAT7L>, "IBAT7L"},
    {G<peli::ppc::Spr::DBAT4U>, "DBAT4U"},
    {G<peli::ppc::Spr::DBAT4L>, "DBAT4L"},
    {G<peli::ppc::Spr::DBAT5U>, "DBAT5U"},
    {G<peli::ppc::Spr::DBAT5L>, "DBAT5L"},
    {G<peli::ppc::Spr::DBAT6U>, "DBAT6U"},
    {G<peli::ppc::Spr::DBAT6L>, "DBAT6L"},
    {G<peli::ppc::Spr::DBAT7U>, "DBAT7U"},
    {G<peli::ppc::Spr::DBAT7L>, "DBAT7L"},
    {G<peli::ppc::Spr::GQR0>, "GQR0"},
    {G<peli::ppc::Spr::GQR1>, "GQR1"},
    {G<peli::ppc::Spr::GQR2>, "GQR2"},
    {G<peli::ppc::Spr::GQR3>, "GQR3"},
    {G<peli::ppc::Spr::GQR4>, "GQR4"},
    {G<peli::ppc::Spr::GQR5>, "GQR5"},
    {G<peli::ppc::Spr::GQR6>, "GQR6"},
    {G<peli::ppc::Spr::GQR7>, "GQR7"},
    {G<peli::ppc::Spr::HID2>, "HID2"},
    {G<peli::ppc::Spr::WPAR>, "WPAR"},
    {G<peli::ppc::Spr::DMAU>, "DMAU"},
    {G<peli::ppc::Spr::DMAL>, "DMAL"},
    {G<peli::ppc::Spr::UMMCR0>, "UMMCR0"},
    {G<peli::ppc::Spr::UPMC1>, "UPMC1"},
    {G<peli::ppc::Spr::UPMC2>, "UPMC2"},
    {G<peli::ppc::Spr::USIA>, "USIA"},
    {G<peli::ppc::Spr::UMMCR1>, "UMMCR1"},
    {G<peli::ppc::Spr::UPMC3>, "UPMC3"},
    {G<peli::ppc::Spr::UPMC4>, "UPMC4"},
    {G<peli::ppc::Spr::MMCR0>, "MMCR0"},
    {G<peli::ppc::Spr::PMC1>, "PMC1"},
    {G<peli::ppc::Spr::PMC2>, "PMC2"},
    {G<peli::ppc::Spr::SIA>, "SIA"},
    {G<peli::ppc::Spr::MMCR1>, "MMCR1"},
    {G<peli::ppc::Spr::PMC3>, "PMC3"},
    {G<peli::ppc::Spr::PMC4>, "PMC4"},
    {G<peli::ppc::Spr::HID0>, "HID0"},
    {G<peli::ppc::Spr::HID1>, "HID1"},
    {G<peli::ppc::Spr::IABR>, "IABR"},
    {G<peli::ppc::Spr::HID4>, "HID4"},
    {G<peli::ppc::Spr::TDCL>, "TDCL"},
    {G<peli::ppc::Spr::DABR>, "DABR"},
    {G<peli::ppc::Spr::L2CR>, "L2CR"},
    {G<peli::ppc::Spr::TDCU>, "TDCU"},
    {G<peli::ppc::Spr::ICTC>, "ICTC"},
    {G<peli::ppc::Spr::THRM1>, "THRM1"},
    {G<peli::ppc::Spr::THRM2>, "THRM2"},
    {G<peli::ppc::Spr::THRM3>, "THRM3"},
};

} // namespace

int main(int argc, char **argv) noexcept {
  peli::util::VIConsole console(false);

  console.Print("\nPowerPC SPR listing:\n");

  // Register the console as stdout
  peli::util::VIConsoleStdOut::Register(console);

  for (std::size_t i = 0; i < std::size(s_spr_list); i++) {
    const auto &info = s_spr_list[i];
    peli::u32 value = info.first();
    if (i % 4 == 0) {
      std::printf("\n");
    }
    std::printf("%6s: %08x ", info.second, value);
  }

  std::fflush(stdout);

  while (true) {
  }

  return 0;
}