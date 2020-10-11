#ifndef GET_CRC
#define GET_CRC

/**
 * \file get_crc.h
 */

#include <stdbool.h>
#include "crc.h"

/**
 * Computes the CRC32 context for the given file.
 *
 * @param   fd              file descriptor
 * @param   context         context to fill with the CRC32
 * @returns @c true if the function succeeds, @c false otherwise
 */
bool get_crc32(int fd, crc32 *context);

#endif // GET_CRC
