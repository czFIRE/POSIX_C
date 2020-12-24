#include "queue.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <pthread.h>
#include <unistd.h>

#define UNUSED(VAR) ((void)(VAR))

int main(void)
{
    int res, test_size = 2;

    struct queue *queue;

    res = queue_create(&queue, sizeof(size_t), test_size);

    printf("%d\n", queue_is_empty(queue));

    size_t nums[] = {1024, 1025, 1026, 1027};

    queue_state_debug(queue);

    res = queue_push(queue, nums);
    res = queue_try_push(queue, nums + 1);

    queue_state_debug(queue);

    printf("%ld %d\n", queue_size(queue), queue_is_full(queue));

    size_t ret_nums[4] = {0};

    res = queue_pop(queue, ret_nums);

    queue_state_debug(queue);

    res = queue_push(queue, nums + 2);
    puts("pushed");
    res = queue_try_pop(queue, ret_nums + 1);
    puts("popped");

    printf("Got these numbers: ");
    for (size_t i = 0; i < 4; i++) {
        printf("%ld ", ret_nums[i]);
    }
    putchar('\n');

    // res = queue_push(queue, nums + 2);
    res = queue_try_push(queue, nums + 3);

    queue_state_debug(queue);

    queue_destroy(queue);
    puts("Finished!");

    UNUSED(res);

    return EXIT_SUCCESS;
}
