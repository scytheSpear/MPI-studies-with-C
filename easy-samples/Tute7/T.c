#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

// Define the number of elements to be processed per thread
#define CHUNKSIZE 2
// Define the size of the array which stores random numbers
#define N 10
// Define the number of threads
#define NUM_THREADS 4
// define the random number range
#define MAXRND 25

int main (int argc, char *argv[]) {
	
	int nthreads, tid, i, chunk;
	int min, max, count, j;
	int a[N];
	int repeated[N+MAXRND] = { 0 };


	chunk = CHUNKSIZE;
	omp_set_num_threads(NUM_THREADS);	//set the number of threads to constant value NUM_THREADS
	#pragma omp parallel shared(a,nthreads,chunk) private(i,tid,min,max)
 	{
 		//get current thread number, when thread number is 0 print the number of threads
 		tid = omp_get_thread_num();
 		if (tid == 0){
 			nthreads = omp_get_num_threads();
 			printf("Number of threads = %d\n", nthreads);
		}
 		
 		printf("Thread %d starting...\n",tid);
 		//generate a pseudo random integer by using current timestamp as a seed 
 		srandom(time(NULL) | tid);
		//start parallel section, generate 10 random values assign in to array a[]
		min=1; max=MAXRND;
		#pragma omp for schedule(dynamic,chunk)
			for (i=0; i<N; i++)
		 	a[i] = (random()%(max-min+1))+min; 
	}

	//print the computed random valued matrix sequentially
	for (i=0; i < N; i++){
 		printf(" a[%d]:%d\n",i, a[i]);
 	}

	printf("------------------------------------\n");
	printf("a[]:No_of_Copies_Found\n");
	//count the repeat occurrences of a value in a[]
	for ( i = 0 ; i < N ; i++ ){
		printf("%d -> ", repeated[a[i]]);
		repeated[a[i]]++;
		printf("a[%d]:%d -> %d\n",i,a[i],repeated[a[i]]);
 	}
 
 	count=0;
	//repeated[] = 1 represents unique occurrences of random numbers ignored
	for ( i = 0 ; i < N+MAXRND; i++ ){
 		if (repeated[i] > 1)
 			count++;
	}
	printf("Total:(num of two or more matching vals found in the a[]) -> %d\n", count);

	return 0;
}