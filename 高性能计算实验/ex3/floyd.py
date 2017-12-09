#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Nov 26 21:04:51 2017

@author: zeroquest
"""

import numpy as np
from sys import maxsize, argv
import time
from mpi4py import MPI

INF = maxsize

#print the graph to console
def showGraph(arr):
    if arr.shape[0] != arr.shape[0]:
        raise ValueError("array should have equal length of its two dimensions")
    else:
        size = arr.shape[0]
        for i in range(size):
            for j in range(size):
                if arr[i][j] == INF:
                    print("inf", end='\t')
                else:
                    print(arr[i][j], end='\t')
            print()

#divide an array into slices, store them in a list
def chop(array, num):
    array_slices = []
    N = array.shape[0]
    slices_len = int(np.sqrt(num))
    assert slices_len ** 2 == num
    tab_len = N // slices_len
    for i in range(size):
        row = i // slices_len
        col = i % slices_len
        array_slices.append(array[row * tab_len : (row + 1) * tab_len,
                               col * tab_len : (col + 1) * tab_len])
    return array_slices

#transform a list of array slices into a whole array
def pile(arr_list):
    tab_rows = []
    num = len(arr_list)
    slices_len = int(np.sqrt(num))
    assert slices_len ** 2 == num
    for i in range(slices_len):
        tab_rows.append(np.hstack(arr_list[i * slices_len : (i + 1) * slices_len])) 
    return np.vstack(tab_rows)

#initiate the ajacency matrix with random module
N = int(argv[1])
arr = np.random.randint(1, 11, size=(N, N))
paths = -np.ones((N, N))

for i in range(N):
    for j in range(N):
        if i == j:
            arr[i][j] = 0
        elif arr[i][j] == 10:
            arr[i][j] = INF
            

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size() #size should be an integer's square

if rank == 0:
    print("Initial Graph:")
    showGraph(arr)
    
slices_len = int(np.sqrt(size))
tab_len = N // slices_len
if rank == 0:
    assert size == slices_len ** 2
arr_slices = chop(arr, size)
paths_slices = chop(paths, size)

#send matrix slices to corresponding process
if rank == 0:
    for i in range(1, size):
        comm.send(arr_slices[i], dest=i)
    arr_slice = arr_slices[0]
else:
    arr_slice = comm.recv(source=0)
        
comm.barrier()

if rank == 0:
    for i in range(1, size):
        comm.send(paths_slices[i], dest=i)
    paths_slice = paths_slices[0]
else:
    paths_slice = comm.recv(source=0)

comm.barrier()

assert arr_slice.shape == (tab_len, tab_len)

#record the time parallel Floyd Algorithm begins
if rank == 0:
    start_time = time.clock()
#Floyd Algorithm in parallel form
for k in range(N):
    if rank == 0:
        row_k = arr[k, :]
        comm.bcast(row_k, root=0)
    else:
        row_k = comm.bcast(None, root=0)
    comm.barrier()
    if rank == 0:
        col_k = arr[:, k]
        comm.bcast(col_k, root=0)
    else:
        col_k = comm.bcast(None, root=0)
    comm.barrier()
    
    for i in range(tab_len):
        r = i + tab_len * (rank // slices_len)
        if col_k[r] == INF:
            continue
        for j in range(tab_len):
            c = j + tab_len * (rank % slices_len)
            if row_k[c] == INF:
                continue
            if (arr_slice[i][j] > row_k[c] + col_k[r]):
                arr_slice[i][j] = row_k[c] + col_k[r]
                paths_slice[i][j] = k + 1
                
#record the time parallel Floyd Algorithm ends
end_time = time.clock()

#gather the shortest ajacency matrix in process 0
if rank == 0:
    shortest_ajacency_list = comm.gather(arr_slice, root=0)
else:
    comm.gather(arr_slice, root=0)
comm.barrier()

#gather the shortest paths in process 0
if rank == 0:
    shortest_paths_list = comm.gather(paths_slice, root=0)
else:
    comm.gather(paths_slice, root=0)
comm.barrier()
    

if rank == 0:
    shortest_ajacency = pile(shortest_ajacency_list)
    shortest_paths = pile(shortest_paths_list)
    print("\nShorest Path Graph:")
    showGraph(shortest_ajacency)
    print("\nPaths:")
    showGraph(shortest_paths)
    print("Parallel Floyd Algorithm runs for %f seconds." % (end_time - start_time))