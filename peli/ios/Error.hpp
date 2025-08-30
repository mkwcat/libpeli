// peli/ios/Error.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

namespace peli::ios {

enum IOSError : signed int {
  // IOS errors

  IOS_ERROR_OK = 0,
  IOS_ERROR_ACCESS = -1,
  IOS_ERROR_EXISTS = -2,
  IOS_ERROR_INTR = -3,
  IOS_ERROR_INVALID = -4,
  IOS_ERROR_MAX = -5,
  IOS_ERROR_NOEXISTS = -6,
  IOS_ERROR_QEMPTY = -7,
  IOS_ERROR_QFULL = -8,
  IOS_ERROR_UNKNOWN = -9,
  IOS_ERROR_NOTREADY = -10,
  IOS_ERROR_ECC = -11,
  IOS_ERROR_ECC_CRIT = -12,
  IOS_ERROR_BADBLOCK = -13,
  IOS_ERROR_INVALID_OBJTYPE = -14,
  IOS_ERROR_INVALID_RNG = -15,
  IOS_ERROR_INVALID_FLAG = -16,
  IOS_ERROR_INVALID_FORMAT = -17,
  IOS_ERROR_INVALID_VERSION = -18,
  IOS_ERROR_INVALID_SIGNER = -19,
  IOS_ERROR_FAIL_CHECKVALUE = -20,
  IOS_ERROR_FAIL_INTERNAL = -21,
  IOS_ERROR_FAIL_ALLOC = -22,
  IOS_ERROR_INVALID_SIZE = -23,
  IOS_ERROR_INVALID_ADDR = -24,
  IOS_ERROR_INVALID_ALIGN = -25,

  // ISFS errors

  ISFS_ERROR_OK = 0,
  ISFS_ERROR_INVALID = -101,
  ISFS_ERROR_ACCESS = -102,
  ISFS_ERROR_CORRUPT = -103,
  ISFS_ERROR_NOTREADY = -104,
  ISFS_ERROR_EXISTS = -105,
  ISFS_ERROR_NOEXISTS = -106,
  ISFS_ERROR_MAXFILES = -107,
  ISFS_ERROR_MAXBLOCKS = -108,
  ISFS_ERROR_MAXFD = -109,
  ISFS_ERROR_MAXDEPTH = -110,
  ISFS_ERROR_OPENFD = -111,
  // -112...-113 ???
  ISFS_ERROR_ECC_CRIT = -114,
  ISFS_ERROR_NOTEMPTY = -115,
  ISFS_ERROR_AUTHENTICATION = -116,
  ISFS_ERROR_QEMPTY = -117,
  ISFS_ERROR_QFULL = -118,
  ISFS_ERROR_FATAL_ERROR = -119,

  // ES errors

  /**
   * Operation completed successfully.
   */
  ES_ERROR_OK = 0,

  /**
   * Invalid public key type in certificate.
   */
  ES_ERROR_INVALID_PUB_TYPE = -1005,

  /**
   * Failed to read from the NAND.
   */
  ES_ERROR_READ = -1009,

  /**
   * Failed to write to the NAND.
   */
  ES_ERROR_WRITE = -1010,

  /**
   * Invalid signature type.
   */
  ES_ERROR_INVALID_SIG_TYPE = -1012,

  /**
   * Maximum amount of ES handles open.
   */
  ES_ERROR_MAX_OPEN = -1016,

  /**
   * Invalid arguments.
   */
  ES_ERROR_INVALID = -1017,

  /**
   * Device ID mismatch in a personalized ticket.
   */
  ES_ERROR_DEVICE_ID_MISMATCH = -1020,

  /**
   * Hash of the content does not match the expected hash from the TMD.
   */
  ES_ERROR_VERIFICATION = -1022,

  /**
   * Memory allocation failure.
   */
  ES_ERROR_NO_MEMORY = -1024,

  /**
   * TMD prohibits access to the requested content.
   */
  ES_ERROR_NO_RIGHTS = -1026,

  /**
   * Issuer was not found in the certificate chain.
   */
  ES_ERROR_ISSUER_NOT_FOUND = -1027,

  /**
   * No ticket was found for the given title.
   */
  ES_ERROR_NO_TICKET = -1028,

  /**
   * The supplied ticket is invalid. This is returned if either the key index is
   * invalid or the ticket title ID doesn't match the TMD.
   */
  ES_ERROR_INVALID_TICKET = -1029,

  /**
   * A higher BOOT2 version is already installed.
   */
  ES_ERROR_BOOT_VERSION = -1031,

  /**
   * Returned early in the ES initialization process in case of a fatal error.
   * ES::CheckCommonKey2 (Ioctl 0x45) returns it if Common Key 2 (a.k.a. Korean
   * Key) is incorrect.
   */
  ES_ERROR_FATAL = -1032,

  /**
   * A ticket play limit was exceeded (duration or launch count).
   */
  ES_ERROR_TICKET_LIMIT = -1033,

  /**
   * Returned by ES::CheckCommonKey2 (Ioctl 0x45) if Common Key 2 (a.k.a. Korean
   * Key) is detected to be all zero.
   */
  ES_ERROR_NO_CK2 = -1034,

  /**
   * A title with a higher major version number is already installed, and/or
   * higher minor version if a system title.
   */
  ES_ERROR_TITLE_VERSION = -1035,

  /**
   * IOS version required by the TMD sysversion field is not installed. This
   * only applies when installing the System Menu.
   */
  ES_ERROR_FIRMWARE_NOT_FOUND = -1036,

  /**
   * Installed number of contents doesn't match TMD. Seems to only apply to the
   * System Menu.
   */
  ES_ERROR_INVALID_TMD = -1037,

  /**
   * Returned by DI as an ES error when "TMD not supplied for disc/nand game".
   */
  ES_ERROR_NO_TMD = -1039,

  // IOSC errors

  IOSC_ERROR_OK = 0,
  IOSC_ERROR_ACCESS = -2000,
  IOSC_ERROR_EXISTS = -2001,
  IOSC_ERROR_INVALID = -2002,
  IOSC_ERROR_MAX = -2003,
  IOSC_ERROR_NOEXISTS = -2004,
  IOSC_ERROR_INVALID_OBJTYPE = -2005,
  IOSC_ERROR_INVALID_RNG = -2006,
  IOSC_ERROR_INVALID_FLAG = -2007,
  IOSC_ERROR_INVALID_FORMAT = -2008,
  IOSC_ERROR_INVALID_VERSION = -2009,
  IOSC_ERROR_INVALID_SIGNER = -2010,
  IOSC_ERROR_FAIL_CHECKVALUE = -2011,
  IOSC_ERROR_FAIL_INTERNAL = -2012,
  IOSC_ERROR_FAIL_ALLOC = -2013,
  IOSC_ERROR_INVALID_SIZE = -2014,
  IOSC_ERROR_INVALID_ADDR = -2015,
  IOSC_ERROR_INVALID_ALIGN = -2016,

  // SSL errors

  SSL_ERROR_OK = 0,
  SSL_ERROR_FAILED = -1,
  SSL_ERROR_RAGAIN = -2,
  SSL_ERROR_WAGAIN = -3,
  SSL_ERROR_SYSCALL = -5,
  SSL_ERROR_ZERO = -6,
  SSL_ERROR_CAGAIN = -7,
  SSL_ERROR_ID = -8,
  SSL_ERROR_VERIFY_CNAME = -9,
  SSL_ERROR_VERIFY_ROOTCA = -10,
  SSL_ERROR_VERIFY_CERT_CHAIN = -11,
  SSL_ERROR_VERIFY_CERT_DATE = -12,
  SSL_ERROR_INVALID_SERVER_CERT = -13,

};

}