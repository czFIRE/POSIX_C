#!/usr/bin/env python3

import getopt
import sys

opts, args = getopt.gnu_getopt(sys.argv[1:], "abg:", ["alpha", "beta", "gamma="])

print("OPTIONS")
for opt in opts:
    print(" *", opt[0], opt[1])

print("ARGUMENTS")
for arg in args:
    print(" *", arg)
