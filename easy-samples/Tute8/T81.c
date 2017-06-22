#include <stdio.h>
#include "mpi.h"

int main (int argc, char *argv[]) {
	
	int rank, size;
	MPI_Comm new_comm;	//define a new communicator new_comm
 
	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	//form new_comm from MPI_COMM_WORLD
	MPI_Comm_split( MPI_COMM_WORLD, rank == 0, 0, &new_comm ); 

	//when rank is 0 call method master_io, and in other case call method slave_io
	if (rank == 0)
		master_io( MPI_COMM_WORLD, new_comm );
	else
		slave_io( MPI_COMM_WORLD, new_comm );
	
	MPI_Finalize( );
	return 0;
}

//master_io receive message from other ranks
int master_io( master_comm, MPI_Comm comm){
	
	int i,j, size;
	char buf[256];

	MPI_Status status;
	MPI_Comm_size( master_comm, &size );
 
	for (j=1; j<=2; j++) {
		for (i=1; i<size; i++) { //master_io which execute when rank is 0, receive from all the other processes 
 			MPI_Recv( buf, 256, MPI_CHAR, i, 0, master_comm, &status );
 			fputs(buf, stdout );	//print out string in buffer 
		}
 	}
}

//slave_io send message to process rank 0, which is the master_io occured
int slave_io( master_comm, MPI_Comm comm ){
	
	char buf[256];
	int rank;

	MPI_Comm_rank( comm, &rank );
	sprintf( buf, "Hello from slave %d\n", rank );
	MPI_Send( buf, strlen(buf) + 1, MPI_CHAR, 0, 0, master_comm ); //send buffer to rank 0
 
	sprintf( buf, "Goodbye from slave %d\n", rank );
	MPI_Send( buf, strlen(buf) + 1, MPI_CHAR, 0, 0, master_comm );
	return 0;
}