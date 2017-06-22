#include <stdio.h>
#include "mpi.h"

#define maxn 12

int main(int argc, char **argv){

	int rank, value, size, errcnt, toterr, i, j;
	
	int up_nbr, down_nbr;
	MPI_Status status;
	double x[12][12];
	double xlocal[(12/4)+2][12];
	
	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );	//set up MPI environment
	//abort processes if number of processes not equal 4
	if (size != 4) MPI_Abort( MPI_COMM_WORLD, 1 ); 

 //enter data by for loop
	for (i=1; i<=maxn/size; i++){
		for (j=0; j<maxn; j++){
	 		xlocal[i][j] = rank;
	 	}
	}
	for (j=0; j<maxn; j++){
		xlocal[0][j] = -1;
		xlocal[maxn/size+1][j] = -1;
	 }


	//if current rank at top, set the up_nbr as a dummy null rank
	up_nbr = rank + 1;
	if (up_nbr >= size)
		up_nbr = MPI_PROC_NULL;
	
	//if current rank at bottom, set the down_nbr as a dummy null rank
	down_nbr = rank - 1;
	if (down_nbr < 0)
		down_nbr = MPI_PROC_NULL;
	
	//Send up and receive from below
	MPI_Sendrecv(xlocal[maxn/size], maxn, MPI_DOUBLE, up_nbr, 0, xlocal[0], maxn, MPI_DOUBLE, down_nbr, 0, MPI_COMM_WORLD, &status );
	//Send down and receive from above
	MPI_Sendrecv( xlocal[1], maxn, MPI_DOUBLE, down_nbr, 1, xlocal[maxn/size+1], maxn, MPI_DOUBLE, up_nbr, 1, MPI_COMM_WORLD, &status );

	//Check results 
	errcnt = 0; 
	for (i=1; i<=maxn/size; i++){
		for (j=0; j<maxn; j++){
	 		if (xlocal[i][j] != rank) 
	 			errcnt++;
	 	}
	}

	for (j=0; j<maxn; j++){
		if (xlocal[0][j] != rank - 1) 
			errcnt++;
		
		if (rank < size-1 && xlocal[maxn/size+1][j] != rank + 1) 
			errcnt++;
	}
	//sum up all error count from every processes
	MPI_Reduce( &errcnt, &toterr, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD );
	
	if (rank == 0) {
		if (toterr)
	 		printf( "!found %d errors\n", toterr );
	else
	 	printf( "No errors\n" );
	}
	 
	MPI_Finalize( );
	return 0;
}