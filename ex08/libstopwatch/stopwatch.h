#ifndef STOPWATCH_H
#define STOPWATCH_H

#ifndef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE 199309L
#endif

#include <time.h>

/**
 * Initialize the stopwatch.
 *
 * \return 0 on success, pthread error code on failure.
 */
int stopwatch_init(void);

/**
 * Destroy stopwatch for all threads.
 *
 * \return 0 on success, pthread error code on failure.
 */
int stopwatch_destroy(void);

/**
 * Reset the stopwatch in the calling thread.
 *
 * This function must be called before the thread attempts to
 * measure elapsed time with \c stopwatch_elapsed().
 *
 * \return 0 on success, pthread error code on failure.
 */
int stopwatch_reset(void);

/**
 * Store the elapsed duration since the last call to \c stopwatch_reset()
 * in the calling thread into the out-parameter \c result.
 *
 * The behaviour is undefined if there was no prior call
 * to \c stopwatch_reset() in the calling thread.
 *
 * \param result        elapsed time
 * \return 0 on success, pthread error code on failure
 */
int stopwatch_elapsed(struct timespec *result);

#endif // STOPWATCH_H
