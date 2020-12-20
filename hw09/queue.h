/**
 * Thread-safe queue library.
 *
 * \author      Roman Lacko <xlacko1@fi.muni.cz>
 * \file        queue.h
 */

#ifndef QUEUE_API_H
#define QUEUE_API_H

#include <stdbool.h>
#include <stddef.h>

/* we need ssize_t for queue_forecast() */
#include <unistd.h>

/**
 * Thread-safe fixed-size queue.
 *
 * This is an opaque struct.
 */
struct queue;

/**
 * Status codes that can be returned by operations on the queue.
 **/
enum queue_error_codes {
    QUEUE_SUCCESS = 0,
    ALLOC_FAILURE,
    WRONG_QUEUE_ARG,
    MUTEX_INIT_FAIL,
    COND_INIT_FAIL,
    MUTEX_DESTROY_FAIL,
    COND_DESTROY_FAIL,
};

//--[  Constructor and Destructor  ]-------------------------------------------

/**
 * Allocates a new queue and stores its pointer to the location pointed
 * to by \c qptr.
 *
 * Behaviour of this function when called on the same qptr from two or
 * more threads is NOT defined and may not be thread-safe.
 *
 * \note    The error code returned by this function is not retrievable
 *          by queue_errno() as no instance would be created.
 *
 * \param[out]  qptr            where the pointer to a new queue will be stored
 * \param[in]   elem_size       size of stored items
 * \param[in]   queue_capacity  queue capacity
 * \returns     0 on success; error code otherwise
 */
int queue_create(struct queue **qptr, size_t elem_size, size_t queue_capacity);

/**
 * Destroys the queue.
 *
 * This operation may not be thread-safe, each queue must be destroyed
 * by exactly one thread.
 *
 * \note    The calling thread should make sure there are no other threads
 *          waiting on this queue, e.g. by changing some signaling variable
 *          and calling the queue_abort().
 *
 * \note    When this function returns (successfully or not), the behaviour
 *          of other functions called with the same \c queue pointer
 *          (except queue_create()) is undefined.
 *
 * \note    The error code returned by this function is not retrievable
 *          by queue_errno() as the instance gets destroyed by this call.
 *
 * \param[in]   queue       queue to destroy
 * \returns     0 on success; error code otherwise
 */
int queue_destroy(struct queue *queue);

//--[  Queries  ]--------------------------------------------------------------

/**
 * Returns the size of struct queue type in bytes.
 */
size_t queue_type_size(void);

/**
 * Returns the upper capacity for the given queue.
 */
size_t queue_capacity(const struct queue *queue);

/**
 * Returns the element size of the given queue.
 */
size_t queue_element(const struct queue *queue);

/**
 * Returns the number of elements stored in the queue.
 */
size_t queue_size(const struct queue *queue);

/**
 * Returns \c true if the queue has no elements stored, \c false otherwise.
 */
bool queue_is_empty(const struct queue *queue);

/**
 * Returns \c true if the queue has no free space available, \c false otherwise.
 */
bool queue_is_full(const struct queue *queue);

/**
 * Returns the number of elements stored with waiting pushes and pops counted
 * in.
 *
 * \note The result may be negative if there are more pops waiting
 *       for respective pushes.
 *
 * \note The result may be greater than the queue capacity if there are
 *       more pushes waiting for respective pops.
 */
ssize_t queue_forecast(const struct queue *queue);

//--[  Queue manipulation  ]---------------------------------------------------

/**
 * Adds a new element into the queue.
 *
 * If the queue is full, the function blocks until there is enough space
 * to store the element.
 *
 * This function makes a copy of the data pointed to by \c elem,
 * so modifying it afterwards will not alter the state of the queue.
 *
 * \param[in]   elem        pointer to data that will be copied to the queue
 * \returns     0 on success; error code otherwise
 */
int queue_push(struct queue *queue, const void *elem);

/**
 * Removes and returns an element from the queue.
 *
 * If the queue is empty, the function blocks until there is an element
 * that can be returned.
 *
 * If \c elem is not \c NULL, the function copies the memory contents of
 * the removed element from its internal storage to the memory pointed to by
 * \c elem. The behaviour is undefined if memory pointed to by \c elem
 * is not at least \c elem_size bytes long.
 *
 * If \c elem is \c NULL, the function discards the element that would have
 * been returned. No copying is performed.
 *
 * \param[out]  elem        pointer to memory where the element will be copied
 *                          or NULL if the data should be discarded
 * \returns     0 on success; error code otherwise
 */
int queue_pop(struct queue *queue, void *elem);

/**
 * Adds a new element into the queue.
 *
 * Similar to \c queue_push(), except it does not block and fails immediately
 * if there is no storage available for the new element.
 *
 * \param[in]   elem        pointer to data that will be copied to the queue
 * \returns     0 on success; error code otherwise
 */
int queue_try_push(struct queue *queue, const void *elem);

/**
 * Removes and returns an element from the queue.
 *
 * Similar to \c queue_pop(), except it does not block and fails immediately
 * if there are no elements in the queue.
 *
 * \param[out]  elem        pointer to memory where the element will be stored
 *                          or NULL if the data should be discarded
 * \returns     0 on success; error code otherwise
 */
int queue_try_pop(struct queue *queue, void *elem);

//--[  Utilities  ]------------------------------------------------------------

/**
 * Wakes up all threads waiting on queue_push() or queue_pop() and causes
 * the blocked operations to fail with a respective error code.
 *
 * \note    It is implementation-defined whether future calls
 *          to push and pop will be able to block or fail immediately.
 *
 * \returns     0 on success; error code otherwise
 */
int queue_abort(struct queue *queue);

/**
 * Returns the error code of the last failed operation on the queue that
 * was performed in the calling thread. Error codes returned by
 * queue_create() and queue_destroy() may not be retrieved by this operation.
 *
 * \note    The function may return 0 if no such error has occured.
 */
int queue_errno(const struct queue *queue);

/**
 * Fills a description of the given \c error_code to the provided \c buffer.
 * The function will write at most \c (maxlen - 1) bytes followed by
 * a null byte.
 *
 * The function returns the number of bytes written to the \c buffer
 * if the entire message including the null byte fits. Otherwise
 * the function returns the minimum number of bytes required to store
 * the entire message. Therefore a call like this:
 *
 *      size_t bufsz = queue_strerror(error_number, NULL, 0ul);
 *
 * can be used to determine the initial buffer size before the message
 * is actually written.
 *
 * \note    The behaviour is (for whatever reason) strikingly similar to
 *          the standard snprintf() function.
 *
 * \param[in]   queue       the queue
 * \returns     number of bytes required for the entire message
 */
size_t queue_strerror(int error_code, char *buffer, size_t maxlen);

#define DEBUG
#ifdef DEBUG
/**
 * This is a debugging function used to read the inner state of the structure 
 * 
 * \param queue             the queue
 */
void queue_state_debug(const struct queue *queue);
#endif

#endif /* QUEUE_API_H */
