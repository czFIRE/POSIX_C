#ifndef CRC_H
#define CRC_H

/**
 * \file crc.h
 */

#include <stddef.h>
#include <stdint.h>

/**
 * \brief CRC32 context struct
 */
typedef struct {
    uint32_t hash;   ///< CRC32 hash
} crc32;

/**
 * \brief Initializes the CRC32's context.
 *
 * \param context   context to initialize
 */
void crc32_init(crc32 *context);

/**
 * \brief Updates the context with the given data.
 *
 * \param context   context to update
 * \param size      \a buffer size in bytes
 * \param buffer    bytes to update the @a context with
 */
void crc32_update(crc32 *context, size_t size, const void *buffer);

/**
 * \brief Finalizes the context.
 *
 * After this call, the final hash value is available as the \c crc
 * attribute of the context.
 *
 * \param context   context to finalize
 */
void crc32_finalize(crc32 *context);

#endif // CRC_H
