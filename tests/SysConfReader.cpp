// SysConfReader.cpp - Print all SYSCONF values

#include <cstdio>
#include <peli/cmn/Types.hpp>
#include <peli/ios/low/Ipc.hpp>
#include <peli/nand/conf/Ipl.hpp>
#include <peli/util/VIConsole.hpp>
#include <peli/util/VIConsoleStdOut.hpp>

namespace {

template <class T> T error() {
  std::fputs("Error: ", stdout);
  return T();
}
} // namespace

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  peli::util::VIConsole console(false);
  console.SetTabWidth(8);

  console.Print("\nMeow! Testing the SYSCONF reader!\n");

  // Register the console as stdout
  peli::util::VIConsoleStdOut::Register(console);

  peli::u32 value_u32;
  std::printf("IPL.CB: 0x%08x\t", peli::nand::conf::GetCounterBias(value_u32)
                                      ? value_u32
                                      : error<peli::u32>());

  peli::nand::conf::AspectRatio value_ar;
  std::printf("IPL.AR: %d\t", peli::nand::conf::GetAspectRatio(value_ar)
                                  ? (peli::s8)value_ar
                                  : error<peli::s8>());

  peli::nand::conf::AutoRunMode value_arm;
  std::printf("IPL.ARN: %d\t", peli::nand::conf::GetAutoRunMode(value_arm)
                                   ? (peli::s8)value_arm
                                   : error<peli::s8>());

  bool value_bool;
  std::printf("IPL.CD: %d\t", peli::nand::conf::GetConfigDone(value_bool)
                                  ? value_bool
                                  : error<bool>());
  std::printf("IPL.CD2: %d\t", peli::nand::conf::GetConfigDone2(value_bool)
                                   ? value_bool
                                   : error<bool>());

  peli::s8 value_s8;
  std::printf("IPL.DH: %d\t",
              peli::nand::conf::GetDisplayOffsetHorizontal(value_s8)
                  ? (peli::s8)value_s8
                  : error<peli::s8>());

  auto &conf = peli::nand::conf::GetSysConf();

  std::printf("\n=== SYSCONF dump ===\n");
  for (size_t i = 0; i < conf.GetCount(); i++) {
    size_t size, key_length;
    const char *key;
    const peli::u8 *data;

    auto type = conf.Get(i, data, size, key, key_length);
    if (type == decltype(type)::None) {
      std::printf("#ERROR: %u\n", i);
      continue;
    }

    std::printf("%.*s: ", key_length, key);

    constexpr auto be = peli::host::Endian::Big;

    switch (type) {
    default:
      std::fputs("(unknown)\t", stdout);
      break;

    case decltype(type)::SmallArray:
    case decltype(type)::BigArray:
      std::fputs("(array)\t", stdout);
      break;

    case decltype(type)::Byte:
      std::printf("c(%d)\t", peli::util::ImmRead<peli::s8>(data));
      break;

    case decltype(type)::Short:
      std::printf("s(%d)\t",
                  peli::util::ImmReadMisaligned<peli::s16, be>(data));
      break;

    case decltype(type)::Long:
      std::printf("l(%d)\t",
                  peli::util::ImmReadMisaligned<peli::s32, be>(data));
      break;

    case decltype(type)::LongLong:
      std::printf("ll(%lld)\t",
                  peli::util::ImmReadMisaligned<peli::s64, be>(data));
      break;

    case decltype(type)::Bool:
      std::printf("%s\t",
                  peli::util::ImmRead<peli::u8>(data) ? "true" : "false");
      break;
    }
  }

  peli::util::VIConsoleStdOut::Deregister();
  return 0;
}