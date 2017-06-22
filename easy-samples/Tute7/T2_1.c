#include <stdio.h>
#include "mpi.h"

#define maxn 12

int main(int argc, char **argv){

	int rank, value, size, errcnt, toterr, i, j;

	MPI_Request r[4];
	int nreq;
	MPI_Status statuses[4];
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

	
	nreq = 0; 
	//send to process current rank+1, if not at maximum rank
	if (rank < size - 1) 
		MPI_Isend(xlocal[maxn/size], maxn, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &r[nreq++]);
	//receive from process current  rank -1, if not at the first rank
	if (rank > 0)
		MPI_Irecv(xlocal[0], maxn, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &r[nreq++]);

	//send to rank -1 if not at the first rank
	if (rank > 0) 
		MPI_Isend( xlocal[1], maxn, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, &r[nreq++] );
	//receive from process current rank+1, if not at maximum rank 
	if (rank < size - 1) 
		MPI_Irecv( xlocal[maxn/size+1], maxn, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &r[nreq++] );
	//wait all messages receive
	MPI_Waitall( nreq, r, statuses );

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