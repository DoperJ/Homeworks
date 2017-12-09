from mpi4py import MPI
import time

def writeList(f, l):
	for i in range(len(l)):
		f.write(str(l[i]) + '\t')

#read input file 
with open('input.txt', 'r') as f:
	line = f.readline()

a = line.split()
a = list(map(int, a))

comm = MPI.COMM_WORLD
rank = comm.Get_rank();
size = comm.Get_size();
N = len(a)
seq_len = N // size
result = []

if rank == 0:
    start_time = time.clock()
#step1: get prefix sum separately
for i in range(rank * seq_len + 1, (rank + 1) * seq_len):
	a[i] += a[i-1]

if N % size != 0:
	for i in range(size * seq_len + 1, N):
		a[i] += a[i-1]

comm.barrier()

#step2: send last value in each seqence to later sequences
data_to_send = a[(rank + 1) * seq_len - 1]
if rank != size-1:
	for i in range(rank+1, size):
		comm.send(data_to_send, dest=i)
#each sequence receive all the last values of previous sequences and add them to its last value
if rank != 0:
	for i in range(rank):
		a[(rank + 1) * seq_len - 1] += comm.recv(source=i)

comm.barrier()

#step3: each sequence send its last value to its next sequence
if rank != size-1:
	comm.send(a[(rank + 1) * seq_len - 1], dest=rank+1)
#add previous sequence's value received to each value except the last one in any sequence, excluding the first one
if rank != 0:
	data_recv = comm.recv(source=rank-1)
	for i in range(rank * seq_len, (rank + 1) * seq_len - 1):
		a[i] += data_recv

comm.barrier()

#step4: gather all result
rank_seq = {}
if rank == 0:
	result = comm.gather(a[rank * seq_len : (rank + 1) * seq_len], root=0)
	for i in range(size * seq_len, N):
		a[i] += result[-1][-1]

	end_time = time.clock()
	print('the parallel calculation cost %f seconds' % (end_time - start_time))
	with open('output.txt', 'w') as f:
		for i in range(len(result)):
			writeList(f, result[i])
		if N % size != 0:
			for i in range(size*seq_len, N):
				f.write(str(a[i]) + '\t')
		f.write('\n')
else:
	comm.gather(a[rank * seq_len : (rank + 1) * seq_len], root=0)
