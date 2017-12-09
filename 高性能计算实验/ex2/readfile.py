#!/usr/bin/env python3

import sys

with open(sys.argv[1], 'r') as f:
	line = f.readline()

a = line.split()
a = list(map(int, a))
print(a)
