#include <stdlib.h>

#define BUFFER_LENGTH 80

struct strategy_impl {
    void *(*init)(long, long);
    void (*destroy)(void *);
    long (*guess)(void *);
    void (*notify)(void *, int);
};

void cleanup(struct strategy_impl *functions[], void *handles[],
             void *strategies[], size_t length);
int init_libraries(struct strategy_impl *functions[], void *handles[],
                   size_t argc, char *argv[]);

int gameloop(int min, int max, size_t length, struct strategy_impl *functions[], void *strategies[]);