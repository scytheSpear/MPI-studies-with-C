#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
int main(int argc, char **argv ){
	
	double t1, t2;
	double tmin;
	int size, *in_data, *out_data;
	int j, nloop, k;
	
	MPI_Init( &argc, &argv );
	printf( "Size (bytes) Time (sec)\tRate (MB/sec)\n" );
 
	//loop when size is less than 1000000 and increase by the power of 2
	for (size = 1; size < 1000000; size *= 2 ) {
		//alocate memory pointer
		in_data = (int *)malloc( size * sizeof(int) );
		out_data = (int *)malloc( size * sizeof(int) );
		//throw process when the memory alocation fail
		if ( !in_data || !out_data) {
 			fprintf( stderr, "Failed to allocate space for %d ints\n", size );
 			break;
		}

		tmin = 1000.0;	//set the minimum displayed time as 1000 
		nloop = 100000/size; //get the number of loops needed

		//nloop is a int, so when size is big enough to make nloop=0, set nloop =1
		if (nloop == 0) 
			nloop = 1;
		//loop the test by 10 times
		for (k=0; k < 10; k++) {
			//set t1 as current time stamp
 			t1 = MPI_Wtime();
 			//copy memory for nloop times
 			for (j=0; j<nloop; j++) 
				memcpy( out_data, in_data, size * sizeof(int) );
 			//get the time after copy all the pointers, calculate averange time used in copy memory 
 			t2 = (MPI_Wtime() - t1) / nloop;
 
 			if (t2 < tmin) 
 				tmin = t2;
		}
		//output data
		printf( "%d\t%f\t%f\n", size*sizeof(int), tmin, 1.0e-6*size*sizeof(int)/tmin );
		//release memory
		free( in_data );
		free( out_data );
	}
	
	MPI_Finalize();
	return 0;
}