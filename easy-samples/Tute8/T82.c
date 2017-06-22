#include <stdio.h>
#include "mpi.h"


#define MSG_EXIT 1
#define MSG_PRINT_ORDERED 2
#define MSG_PRINT_UNORDERED 3

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

/* This is the master */
int master_io( master_comm, MPI_Comm comm){
	
	int i,j, size, nslave, firstmsg;
	char buf[256], buf2[256];
	
	MPI_Status status;
	MPI_Comm_size( master_comm, &size );
	nslave = size - 1;
	
	while (nslave > 0) {
		MPI_Recv( buf, 256, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, master_comm, &status );
		
		//retrive MPI_TAG from received messages from slave ranks
		switch (status.MPI_TAG){
			//when tag is MSG_EXIT, reduce nslave make it less or equal than 0, quit while loop 
			case MSG_EXIT:
					nslave--; 
			break;
			//when tag is MSG_UNORDERED, print received messages 
			case MSG_PRINT_UNORDERED: 
					fputs( buf, stdout );
 			break;
			case MSG_PRINT_ORDERED:
					//get source rank number from MPI_SOURCE
 					firstmsg = status.MPI_SOURCE;
 					//when source rank is i print messages, else receive message to buf2 and print buf2
					for (i=1; i<size; i++) {
						if (i == firstmsg) 
							fputs( buf, stdout );
						else {
							MPI_Recv( buf2, 256, MPI_CHAR, i, MSG_PRINT_ORDERED, master_comm, &status );
							fputs( buf2, stdout );
						}
					}
			break;
		}
	}
	return 0;
}

/* This is the slave */
int slave_io( master_comm, MPI_Comm comm ){
	char buf[256];
	int rank;

	MPI_Comm_rank( comm, &rank );

	//send with tag MSG+PRINT_ORDERED to process rank 0
	sprintf( buf, "Hello from slave %d\n", rank );
	MPI_Send( buf, strlen(buf) + 1, MPI_CHAR, 0, MSG_PRINT_ORDERED, master_comm );
 
	sprintf( buf, "Goodbye from slave %d\n", rank );
	MPI_Send( buf, strlen(buf) + 1, MPI_CHAR, 0, MSG_PRINT_ORDERED, master_comm );

	//send with tag MSG+PRINT_UNORDERED and MSG_EXIT to process rank 0
	sprintf( buf, "I'm exiting (%d)\n", rank );
	MPI_Send( buf, strlen(buf) + 1, MPI_CHAR, 0, MSG_PRINT_UNORDERED, master_comm );
	MPI_Send( buf, 0, MPI_CHAR, 0, MSG_EXIT, master_comm );
	
	return 0;
}