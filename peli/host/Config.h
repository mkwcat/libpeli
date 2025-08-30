#pragma once

namespace peli::host {

#define PELI_DEBUG
#define PELI_NEWLIB
#define PELI_HOST_PPC
// #define PELI_HOST_IOS 1

/**
 * Enable floating point support.
 */
#define PELI_ENABLE_FLOAT

/**
 * Enable paired single support.
 */
#define PELI_ENABLE_PAIRED_SINGLE

} // namespace peli::host