// peli/ios/iosc/Types.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../../cmn/Types.hpp"

namespace peli::ios::iosc {

/**
 * SHA-1 hash.
 */
using Hash = u8[20];

/**
 * Certificate or issuer chain name.
 * Example issuer:
 * - Root-CA00000001-CP00000004
 * Example cert name:
 * - CA00000001
 */
using Name = char[64];

/**
 * Padding after the signature in certificates and blobs.
 */
using SigDummy = u8[60];

/**
 * Padding after the public key in a certificate.
 */
using CertPad = u8[52];

/**
 * Padding after the ECC signature.
 */
using EccCertPad = u8[4];

/**
 * Padding after the ECC public key.
 */
using EccPublicPad = u8[4];

constexpr size_t EccKeySize = 30;

using EccPublicKey = u8[EccKeySize * 2];
using EccPrivateKey = u8[EccKeySize];

/**
 * RSA exponent.
 */
using RsaExponent = u32;

enum class KeyType : u32 {
  Rsa4096 = 0x00010000,
  Rsa2048 = 0x00010001,
  Ecc = 0x00010002,
};

constexpr size_t Rsa4096Size = 512;
constexpr size_t Rsa2048Size = 256;
constexpr size_t EccSize = EccKeySize * 2;

struct SigRsa4096 {
  static constexpr KeyType Type = KeyType::Rsa4096;
  static constexpr size_t Size = Rsa4096Size;

  /* 0x000 */ KeyType sig_type;
  /* 0x004 */ u8 sig[Rsa4096Size];
  /* 0x104 */ SigDummy dummy;
  /* 0x140 */ Name issuer;
};

static_assert(sizeof(SigRsa4096) == 0x280);

struct SigRsa2048 {
  static constexpr KeyType Type = KeyType::Rsa2048;
  static constexpr size_t Size = Rsa2048Size;

  /* 0x000 */ KeyType sig_type;
  /* 0x004 */ u8 sig[Rsa2048Size];
  /* 0x104 */ SigDummy dummy;
  /* 0x140 */ Name issuer;
};

static_assert(sizeof(SigRsa2048) == 0x180);

struct SigEcc {
  static constexpr KeyType Type = KeyType::Ecc;
  static constexpr size_t Size = EccSize;

  /* 0x000 */ KeyType sig_type;
  /* 0x004 */ u8 sig[EccSize];
  /* 0x040 */ EccCertPad ecc_pad;
  /* 0x044 */ SigDummy dummy;
  /* 0x080 */ Name issuer;
};

static_assert(sizeof(SigEcc) == 0x0C0);

template <KeyType Type> struct Sig {};

template <> struct Sig<KeyType::Rsa4096> : SigRsa4096 {};

template <> struct Sig<KeyType::Rsa2048> : SigRsa2048 {};

template <> struct Sig<KeyType::Ecc> : SigEcc {};

struct PublicKeyRsa4096 {
  /* 0x000 */ u8 key[Rsa4096Size];
  /* 0x200 */ RsaExponent exponent;
  /* 0x204 */ CertPad pad;
};

static_assert(sizeof(PublicKeyRsa4096) == 0x238);

struct PublicKeyRsa2048 {
  /* 0x000 */ u8 key[Rsa2048Size];
  /* 0x100 */ RsaExponent exponent;
  /* 0x104 */ CertPad pad;
};

static_assert(sizeof(PublicKeyRsa2048) == 0x138);

struct PublicKeyEcc {
  /* 0x000 */ EccPublicKey key;
  /* 0x03C */ EccPublicPad ecc_pad;
  /* 0x040 */ CertPad pad;
  /* 0x074 */ EccCertPad pad2;
};

static_assert(sizeof(PublicKeyEcc) == 0x078);

template <KeyType Type> struct PublicKey {};

template <> struct PublicKey<KeyType::Rsa4096> : PublicKeyRsa4096 {};

template <> struct PublicKey<KeyType::Rsa2048> : PublicKeyRsa2048 {};

template <> struct PublicKey<KeyType::Ecc> : PublicKeyEcc {};

constexpr size_t AesKeySize = 16;
constexpr size_t AesDataAlign = 64;

using AesKey = u8[AesKeySize];
using AesIv = u8[AesKeySize];

// Only relevant for ES_DiVerify
using SecretKeyHandle = s32;

template <KeyType SigKeyType, KeyType PubKeyType> struct Certificate {
  /* 0x000 */ Sig<SigKeyType> sig;
  /* 0x280 / 0x180 / 0x0C0 */ KeyType public_key_type;
  /* 0x284 / 0x184 / 0x0C4 */ Name name;
  /* 0x2C4 / 0x1C4 / 0x104 */ u32 timestamp;
  /* 0x2C8 / 0x1C8 / 0x108 */ PublicKey<PubKeyType> public_key;
};

static_assert(sizeof(Certificate<KeyType::Rsa4096, KeyType::Rsa2048>) == 0x400);
static_assert(sizeof(Certificate<KeyType::Rsa2048, KeyType::Rsa2048>) == 0x300);
static_assert(sizeof(Certificate<KeyType::Rsa2048, KeyType::Ecc>) == 0x240);
static_assert(sizeof(Certificate<KeyType::Ecc, KeyType::Ecc>) == 0x180);

} // namespace peli::ios::iosc