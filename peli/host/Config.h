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

/**
 * Minimum stack size for threads.
 */
#define PELI_THREAD_MIN_STACK_SIZE 0x1000

/**
 * Stack size for the initial startup thread. This is allocated within the .bss
 * section.
 */
#define PELI_CRT0_STACK_SIZE 0x8000

/**
 * Override for the default memory allocation function.
 */
// #define PELI_FUNC_ALLOC(X_ALIGN, X_SIZE) ::aligned_alloc(X_ALIGN, X_SIZE)

/**
 * Override for the default memory free function.
 */
// #define PELI_FUNC_FREE(X_PTR, X_SIZE) ::free(X_PTR)

} // namespace peli::host
