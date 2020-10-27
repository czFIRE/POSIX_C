// something something mblen -> something with that iterator
#define _GNU_SOURCE

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include <assert.h>

void close_files(FILE **files, size_t length);

int main(int argc, char *argv[], char *env[])
{
    // if I use files, then the program se
    printf("%s\n", setlocale(LC_CTYPE, NULL));

    FILE **files = calloc(argc - 1, sizeof(FILE *));

    if (!files) {
        return EXIT_FAILURE;
    }

    for (size_t i = 1; i < argc; i++) {
        FILE *file = fopen(argv[i], "r");
        files[i - 1] = file;

        if (file == NULL) {
            close_files(files, argc - 1);
            free(files);
            return EXIT_FAILURE;
        }
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    for (size_t i = 0; i < argc - 1; i++) {
        size_t chars = 0, bytes = 0;

        while ((read = getline(&line, &len, files[i])) != EOF) {
            puts(line);
            bytes += read;

            mbstate_t mbs = {0}; // mbr
            size_t line_len = strlen(line);
            size_t byte_size;

            mbrlen(NULL, 0, &mbs); // mbr
            // mblen(NULL, 0);       //mb

            char *line_copy = line;

            while (line_len > 0) {
                byte_size = mbrlen(line, line_len, &mbs); // mbr
                // byte_size = mblen(line_copy, line_len);       //mb

                printf("byte_size = %d\n", byte_size);
                if (byte_size == 0 || byte_size > line_len) {
                    break;
                }
                line_copy += byte_size; // mb
                line_len -= byte_size;
                chars++;
            }

            // printf("%s", line);
        }

        printf("%s: bytes=%lu chars=%lu\n", argv[i + 1], bytes, chars);
    }

    free(line);
    free(files);

    return EXIT_SUCCESS;
}

void close_files(FILE **files, size_t length)
{
    for (size_t i = 0; i < length; i++) {
        if (files[i] == NULL) {
            return;
        }

        fclose(files[i]);
    }
}