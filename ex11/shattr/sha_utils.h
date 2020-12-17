#ifndef SHA_UTILS_H
#define SHA_UTILS_H

#include <stddef.h>
#include <stdbool.h>

/**
 * Digest to use.
 */
#ifndef DIGEST
#  define DIGEST SHA256
#endif

/**
 * Computes the SHA checksum of the data from the given descriptor.
 *
 * The function uses OpenSSL's implementation of the digest to compute
 * the digest of the provided file descriptor. The result is returned
 * via the output parameters \c size and \c digest, where the digest
 * is automatically allocated.
 *
 * \note    It is responsibility of the caller to free the memory allocated
 *          for the \c digest variable after usage.
 *
 * Example:
 *
 * \code{.c}
 *      size_t  digest_size;
 *      void   *digest;
 *
 *      if (get_digest(descriptor, &digest_size, &digest) == 0) {
 *          // handle error
 *      }
 * \endcode
 *
 * The provided file descriptor must represent a regular file without
 * \c O_NONBLOCK flag, otherwise the effect is unspecified.
 *
 * \param[in]   fd      File descriptor to be read.
 * \param[out]  size    Number of bytes written to \c digest.
 * \param[out]  digest  Automatically allocated memory that holds the result.
 * \returns     true on success, otherwise prints an error message
 *              and returns false.
 */
bool get_digest(int fd, size_t *size, void **digest);

#endif // SHA_UTILS_H
