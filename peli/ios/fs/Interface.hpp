// peli/ios/fs/Interface.hpp
//   Written by mkwcat
//
// Copyright (c) 2025 mkwcat
// SPDX-License-Identifier: MIT

#pragma once

#include "../../common/Types.h"
#include "../Interface.hpp"
#include "Types.hpp"

namespace peli::ios::fs {

/**
 * I/O Control commands for FS (File System).
 */
enum class Ioctl : u32 {
  FORMAT = 0x01,
  GET_STATS = 0x02,
  CREATE_DIR = 0x03,
  READ_DIR = 0x04,
  SET_ATTR = 0x05,
  GET_ATTR = 0x06,
  DELETE = 0x07,
  RENAME = 0x08,
  CREATE_FILE = 0x09,
  SET_FILE_VER_CTRL = 0x0A,
  GET_FILE_STATS = 0x0B,
  GET_USAGE = 0x0C,
  SHUTDOWN = 0x0D,
  CREATE_MULTIPLE_FILES = 0x0E,
};

/**
 * FS (File System) provides high-level interfaces for managing files and
 * directories on the NAND file system.
 *
 * Its module exists in every IOS, including BOOT2, and is always embedded
 * inside the main boot content, alongside IOS, IOSC, and usually ES. It runs
 * under PID 2. The module name sometimes varies across the names FS, FSP, FFS,
 * FFSP. The module sections are located in DOMAIN 2.
 */
struct Interface : public ios::Interface<Ioctl> {
  /**
   * Formats the entire NAND file system. This is not like the Wii Menu button,
   * it will erase _all data_, including data necessary for the system to boot,
   * causing a brick only recoverable via boot2-level hacks (like BootMii). It's
   * likely used during the factory process to setup the filesystem, as it's
   * called by ES if GET_STATS (0x02) returns ISFS_ERROR_NOTREADY (-104), which
   * will occur if the NAND filesystem doesn't exist or is corrupted.
   *
   * It can only be called by UID 0 on a /dev/fs handle. Attempting to do
   * otherwise will return ISFS_ERROR_ACCESS (-102) [check].
   */
  using Format = //
      IoctlDef<fs::Ioctl::FORMAT,
               In<>, //
               Out<>>;

  /**
   * Fetches the filesystem statistics. Includes the following information (in
   * the output Stats structure):
   * - Cluster size
   * - Free clusters
   * - Used clusters
   * - Bad clusters
   * - Reserved clusters
   * - Free inodes
   * - Used inodes
   *
   * It can be called by any UID on a /dev/fs handle.
   */
  using GetStats = //
      IoctlDef<fs::Ioctl::GET_STATS,
               In<>,        //
               Out<Stats>>; // Output stats

  /**
   * Creates a directory. Path and attributes are passed in the input Attr
   * structure.
   *
   * It can be called by any UID on a /dev/fs handle.
   */
  using CreateDir = //
      IoctlDef<fs::Ioctl::CREATE_DIR,
               In<Attr>, // Directory attributes
               Out<>>;

  /**
   * Get the total number of entries in a directory. The command is the same as
   * ReadDir, but differs in vector count. The input is the directory path, and
   * the output is the number of entries in the directory.
   */
  using ReadDirCount = //
      IoctlVecDef<fs::Ioctl::READ_DIR,
                  In<Path>,  // Directory path
                  Out<u32>>; // Total entry count

  /**
   * Lists the contents of a directory. Inputs are the directory path and the
   * maximum number of entries that can be written to the output buffer. The
   * outputs are a list of entries, and the total number of entries in the
   * directory irrespective of the supplied maximum number of entries.
   *
   * It can be called by any UID on a /dev/fs handle.
   */
  using ReadDir = IoctlVecDef<fs::Ioctl::READ_DIR,
                              In<Path,        // Directory path
                                 u32>,        // Max entries
                              Out<NodeName *, // Output entries
                                  u32>>;      // Total entry count

  /**
   * Sets the attributes of a file or directory. The path and attributes are
   * passed in the input Attr structure.
   *
   * It can be called by any UID on a /dev/fs handle. The caller must have write
   * access to the containing path.
   */
  using SetAttr = //
      IoctlDef<fs::Ioctl::SET_ATTR,
               In<Attr>, // File attributes
               Out<>>;

  /**
   * Gets the attributes of a file or directory. The path is passed in the input
   * string and the output is of the Attr structure.
   *
   * It can be called by any UID on a /dev/fs handle. TODO: Check what
   * permissions are needed.
   */
  using GetAttr = //
      IoctlDef<fs::Ioctl::GET_ATTR,
               In<Path>, // File path
               Out<Attr>>;

  /**
   * Deletes a file or directory. The path is passed in the input string.
   * TODO: Check if a directory needs to be empty before deletion.
   *
   * It can be called by any UID on a /dev/fs handle. The caller must have write
   * access to the containing path.
   */
  using Delete = IoctlDef<fs::Ioctl::DELETE, In<Path>, Out<>>;

  /**
   * Renames or moves a file or directory. The paths are passed in the input
   * RenamePaths structure. When moving a file from one directory to
   * another, the destination file name must be identical to the source file
   * name.
   *
   * It can be called by any UID on a /dev/fs handle. The caller must have write
   * access to both paths.
   */
  using Rename = //
      IoctlDef<fs::Ioctl::RENAME,
               In<RenamePaths>, // Rename paths
               Out<>>;

  /**
   * Creates a file. The path and attributes are passed in the input Attr
   * structure. Owner and group ID are overriden by the UID and GID of the
   * caller.
   *
   * It can be called by any UID on a /dev/fs handle. The caller must have write
   * access to the containing path.
   */
  using CreateFile = //
      IoctlDef<fs::Ioctl::CREATE_FILE,
               In<Attr>, // File attributes
               Out<>>;

  /**
   * Unknown ioctl, must be investigated further. Details such as arguments are
   * unknown.
   */
  using SetFileVerCtrl = //
      IoctlDef<fs::Ioctl::SET_FILE_VER_CTRL, In<>, Out<>>;

  /**
   * Gets the stats of the current open file. The output is of the FileStats
   * structure, and contains the file size and current stream position in the
   * file.
   *
   * It can be called on any open ISFS file handle, but not a /dev/fs handle.
   */
  using GetFileStats = //
      IoctlDef<fs::Ioctl::GET_FILE_STATS,
               In<>,            //
               Out<FileStats>>; // File stats

  /**
   * Gets the used number of clusters and inodes of a specific directory. The
   * input is the path and the outputs are the number of used clusters and
   * inodes.
   *
   * It can be called by any UID on a /dev/fs handle. TODO: Check needed
   * permissions.
   */
  using GetUsage = //
      IoctlVecDef<fs::Ioctl::GET_USAGE,
                  In<Path>,       // Directory path
                  Out<u32, u32>>; // Used clusters, used inodes

  /**
   * Waits for any pending I/O operations to finish and shuts down the file
   * system. Any requests after this will return ISFS_ERROR_SHUTDOWN (-119).
   *
   * It can be called by any UID on a /dev/fs handle.
   */
  using Shutdown = //
      IoctlDef<fs::Ioctl::SHUTDOWN,
               In<>, //
               Out<>>;

  /**
   * Creates and allocates space for many files in one go. The actual name and
   * purpose of the command is unknown. It was added to IOS at an unknown point
   * and no code has ever been found to use it. Features a variable number of
   * vectors, all inputs, each containing a path for a file to be created. The
   * last vector is an array of u32 values, which are the amount of space to
   * allocate in each file.
   *
   * It can be called by any UID on a /dev/fs handle. TODO: Check needed
   * permissions.
   */
  using CreateMultipleFiles = //
      IoctlVecDef<fs::Ioctl::CREATE_MULTIPLE_FILES,
                  In<>, //
                  Out<>>;
};

} // namespace peli::ios::fs