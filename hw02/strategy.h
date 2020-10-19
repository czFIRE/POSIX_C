#ifndef STRATEGY_H
#define STRATEGY_H

/**
 * Guess Strategy API
 *
 * \file strategy.h
 * \author Roman Lacko <xlacko1@fi.muni.cz>
 */

/**
 * Initialize the strategy with range from which the secret number is chosen.
 * Returns a pointer to internal data or NULL on failure.
 *
 * \param  min      lower bound (inclusive)
 * \param  max      upper bound (inclusive)
 */
void *init(long min, long max);

/**
 * Deallocates internal state of the strategy.
 */
void destroy(void *self);

/**
 * Asks the library for next guess.
 *
 * \return self     strategy instance
 */
long guess(void *self);

/**
 * Informs the library about the result of its guess.
 *
 * The guess does not need to be a result of this strategy's \c guess call,
 * the program may inform all strategies about each other's guesses.
 *
 * \param  self     strategy instance
 * \param  sign     +1 or -1
 */
void notify(void *self, int sign);

#endif // STRATEGY_H
