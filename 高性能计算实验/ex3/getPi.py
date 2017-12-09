#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Nov 26 14:33:45 2017

@author: zeroquest
"""

from scipy import pi
from sys import argv
from numpy import arange
from multiprocessing import Pool, cpu_count

N = 10 ** int(argv[1])
p_num = cpu_count()
chunkSize = N // p_num
N = chunkSize * p_num
numbers = arange(N)
numbers = numbers.reshape(-1, chunkSize)

def getPartSum(numbers):
    part_sum = 0
    for i in numbers:
        part_sum += (1.0 if (i % 2 == 0) else -1) / (2 * i + 1)
    return part_sum


pool = Pool()
part_sum_list = list(pool.imap_unordered(getPartSum, numbers, 1))
pool.close()
res = sum(part_sum_list) * 4
delta = pi - res
print("result of calculation of PI is: %.20f" % pi)
print("error is about: %.20f" % delta)