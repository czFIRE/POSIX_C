#include "strategy.h"

void *init(long min, long max) { return 0; }

void destroy(void *self) {}

long guess(void *self) { return 5; }

void notify(void *self, int sign) {}