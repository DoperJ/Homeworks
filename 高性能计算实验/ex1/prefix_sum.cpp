#include <iostream>
#include <omp.h>
using namespace std;

int main(int argc, char *argv[])
{
	int i, j, chunk, tid;
	int num_threads=0;
	int seq_len = 0;
	chunk = 4;
	int N = atoi(argv[1]);
	int arr[N];
	int ass[N];
    cout << "initial array:\t\t";
    //initialize array
	for (i=0; i<=N; i++) {
		arr[i] = i;
		ass[i] = i;
        cout << arr[i] << "\t";
	}
    cout << endl << endl;

	//assistant array is used to get the prefix sum
	//without parallel algorithms
	double start_ass = omp_get_wtime();
	for (i=0; i<N; i++) {
		ass[i+1] += ass[i];
	}

	//print serialized algorithm running time
	double end_ass = omp_get_wtime();
	cout << "serialized algorithm has run for: " << end_ass - start_ass << "s\n";
	cout << endl;

	//print the prefix sum stored in the assistant array
    cout << "result should be:\t";
    for (i=0; i<=N; i++) {
        cout << ass[i] << "\t";
    }
	cout << endl << endl;

    //step 1 : get the prefix sum seperately in each sequence
	double start_step1 = omp_get_wtime();
	#pragma omp parallel shared(arr) private(tid, j)
	{
		// get thread id
		tid = omp_get_thread_num();
		// get thread number
		num_threads = omp_get_num_threads();
		//get sequence length
		seq_len = (N + 1) / num_threads;
        
		for(j=0; j<seq_len-1; j++) {
			arr[tid*seq_len+j+1] += arr[tid*seq_len+j];
		}
	}

    cout << "after step 1:\t\t";
    for (i=0; i<=N; i++) {
        cout << arr[i] << "\t";
    }
    cout << endl;
	
	//print step 1 running time
	double start_step2 = omp_get_wtime();
	cout << "step 1 has run for: " << start_step2 - start_step1 << "s\n";
	cout << endl;

    /*
     step 2:
     get the prefix sum of considering all the last value
        in each sequence
     */
	for (i=2; i<num_threads+1; i++) {
		arr[i*seq_len-1] += arr[(i-1)*seq_len-1];
	}

    cout << "after step 2:\t\t";
    for (i=0; i<=N; i++) {
        cout << arr[i] << "\t";
    }
    cout << endl;
    
	//print step 2 running time
	double start_step3 = omp_get_wtime();
	cout << "step 2 has run for: " << start_step3 - start_step2 << "s\n";
	cout << endl;

    /*
     step 3:
     add every element with the last value
        in its previous sequence
     */
	#pragma omp parallel private(tid, j) shared(arr)
	{
		tid = omp_get_thread_num();
		if (tid!=0) {
			for (j=0; j<seq_len-1; j++) {
				arr[tid*seq_len+j] += arr[tid*seq_len-1];
			}
		}
	}
    
    cout << "after step 3:\t\t";
    for (i=0; i<=N; i++) {
        cout << arr[i] << "\t";
    }
    cout << endl;

	//print step 3 running time
	double start_step4 = omp_get_wtime();
	cout << "step 3 has run for: " << start_step4 - start_step3 << "s\n";
	cout << endl;

    /*
     step 4:
     do the same operation like step 3
        but for the last few value
     */
	if (N % num_threads != 0) {
		for (i=num_threads*seq_len; i<=N; i++)
		{
			arr[i] += arr[i-1];
		}
	}
    
    cout << "final result:\t\t";
	for(i=0; i<=N; i++) {
		cout << arr[i] << "\t";
	}
    cout << endl;

	//print step 4 running time
	double end = omp_get_wtime();
	cout << "step 4 has run for: " << end - start_step4 << "s\n";
	return 0;
}
