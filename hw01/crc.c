#include <stdint.h>
#include <inttypes.h>

#include "crc.h"

static
uint32_t crc32_reverse(uint32_t x)
{
   x = ((x & 0x55555555) <<  1) | ((x >>  1) & 0x55555555);
   x = ((x & 0x33333333) <<  2) | ((x >>  2) & 0x33333333);
   x = ((x & 0x0F0F0F0F) <<  4) | ((x >>  4) & 0x0F0F0F0F);
   x = (x << 24) | ((x & 0xFF00) << 8) |
       ((x >> 8) & 0xFF00) | (x >> 24);
   return x;
}

void crc32_init(crc32 *context)
{
    context->hash = 0xffffffff;
}

void crc32_update(crc32 *context, size_t size, const void *buffer)
{
    uint32_t byte;
    uint32_t crc = context->hash;

    const uint8_t *mem = buffer;

    for (size_t i = 0; i < size; ++i) {
        byte = crc32_reverse(mem[i]);

        for (int j = 0; j <= 7; ++j) {
            if ((int32_t)(crc ^ byte) < 0)
                crc = (crc << 1) ^ 0x4c11db7;
            else
                crc <<= 1;

            byte <<= 1;
        }
    }

    context->hash = crc;
}

void crc32_finalize(crc32 *context)
{
    context->hash = crc32_reverse(~context->hash);
}
