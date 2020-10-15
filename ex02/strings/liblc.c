#include "libfilter.h"
#include <ctype.h>
#include <string.h>

// LOWER
void filter(char *str)
{
    for (size_t i = 0; i < strlen(str); i++) {
        *(str + i) = tolower(*(str + i));
    }
}