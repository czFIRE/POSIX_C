#include "sha_utils.h"

#include <stdlib.h>

#include <error.h>
#include <unistd.h>
#include <openssl/err.h>
#include <openssl/sha.h>

// Size of the read buffer.
#define READ_BUFFER_SIZE 512

__attribute__((constructor))
static void initialize(void)
{
    // This function must be called before OpenSSL's ERR_*() functions
    // can be used.
    ERR_load_crypto_strings();
}

__attribute__((destructor))
static void finalize(void)
{
    // Release resources allocated by ERR_load_crypto_strings().
    ERR_free_strings();
}

// The following magic is too dark to be explained.
#define CONCAT1(A, B)   A ## B
#define CONCAT(A, B)    CONCAT1(A, B)

// DSYMBOL() and DCALL() macros will return the correct symbol for
// the chosen hash algorithm, e.g.
//
//      #define PREFIX SHA512
//      DSYMBOL(CTX)        // SHA512_CTX
//      DCALL(Init, &c)     // SHA512_Init(&c)
#define DSYMBOL(SYMBOL)     CONCAT(CONCAT(DIGEST, _), SYMBOL)
#define DCALL(WHAT, ...)    (DSYMBOL(WHAT))(__VA_ARGS__)

// Convenience macros for OpenSSL's reason strings.
#define ERR_reason_string() ERR_reason_error_string(ERR_get_error())

bool get_digest(int fd, size_t *size, void **digest)
{
    static char buffer[READ_BUFFER_SIZE];

    // initialize hash context
    DSYMBOL(CTX) context;
    if (!DCALL(Init, &context))
        goto error_openssl;

    // update context with the block from the file
    ssize_t rd;
    while ((rd = read(fd, buffer, sizeof(buffer))) > 0) {
        // read() may return fewer bytes than we asked it to read
        if (!DCALL(Update, &context, buffer, rd))
            goto error_openssl;
    }

    // read() returns -1 on error
    if (rd == -1)
        goto error_errno;

    // allocate buffer for the result
    *size = DSYMBOL(DIGEST_LENGTH);
    if ((*digest = malloc(*size)) == NULL)
        goto error_errno;

    // *_Final() writes digest bytes to the buffer
    if (!DCALL(Final, *digest, &context))
        goto error_openssl;

    return 1;

error_errno:
    error(0, errno, "failed to compute checksum");
    return 0;

error_openssl:
    error(0, 0, "openssl error: %s", ERR_reason_string());
    return 0;
}
