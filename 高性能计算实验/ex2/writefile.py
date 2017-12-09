#!/usr/bin/env python3
import sys

N = int(sys.argv[1])
with open('input.txt', 'w') as f:
	for i in range(1, N+1):
		f.write(str(i) + ' ')
	f.write('\n')
