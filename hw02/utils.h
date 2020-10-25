#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

#define BUFFER_LENGTH 80
#define FAILURE 1
#define SUCCESS 0

struct strategy_impl {
    void *(*init)(long, long);
    void (*destroy)(void *);
    long (*guess)(void *);
    void (*notify)(void *, int);
};

struct guess_game {
    size_t length;
    struct strategy_impl **functions;
    void **handles;
    void **strategies;
};

void cleanup(struct guess_game *resources);
int init_libraries(size_t argc, char *argv[], struct guess_game *resources);
int gameloop(int min, int max, struct guess_game *resources);

#endif // UTILS_H