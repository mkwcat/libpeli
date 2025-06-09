#pragma once

namespace wsh::host {

#define WSH_DEBUG
#define WSH_NEWLIB
#define WSH_HOST_PPC
// #define WSH_HOST_IOS 1

/**
 * Enable floating point support.
 */
#define WSH_ENABLE_FLOAT

/**
 * Enable paired single support.
 */
#define WSH_ENABLE_PAIRED_SINGLE

} // namespace wsh::host