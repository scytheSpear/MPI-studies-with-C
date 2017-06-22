#include <stdio.h>
#include "mpi.h"
#include <string.h>
#include <stdlib.h>

int main(argc,argv)
int argc;
char **argv;
{
	int rank;	
	int value, valueIn, message;
	char result[]="BROADCAST STRING";
	int reslen=strlen(result);
	int size;	//define rank, size and data use in send and recv
	int i;	

	MPI_Request *requestList, requestNull;
	MPI_Status status;	//define the status agument which will use at MPI_Recv, represents 
						//message source and tag
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);	//set up MPI environment

	if(size!=5)			//abort program if the number of processes is not 5
		MPI_Abort(MPI_COMM_WORLD,1);

	//do{	
		if(rank==0){	//when process rank is 0, get value from user input
			requestList = (MPI_Request*)malloc((size-1)*sizeof(MPI_Request));	
			scanf("%d",&value);
				
			//send value to all other processes using MPI_Isend,
			//because the send request should not block next send operation, 
			//and tag=0 refers the value is from process rank 0
			for(i=1;i<size;i++){
				MPI_Isend(&value,1,MPI_INT,i,0,MPI_COMM_WORLD,&requestNull);
			}
			//recv valueIn from all other processes using MPI_Irecv
			//tag=1 refers the valueIn is from other processes to rank 0
			for(i=1;i<size;i++){
			MPI_Irecv(&valueIn,1,MPI_INT,i,1,MPI_COMM_WORLD,&(requestList[i-1]));
			}
			//for all the valueIn messages from other processes,
			//process rank 0 need to wait all messages receive
			for(i=1;i<size;i++){
				int index;
				MPI_Waitany(size-1,requestList,&index,&status);
				printf("Recv from process rank %d data %d\n", index+1, valueIn);
			}
		}
		else{
			//receive value from rank 0, assign to message
			MPI_Recv(&message,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
			message = message*rank;
			//send modified message back to rank 0
			MPI_Send(&message,1,MPI_INT,0,1,MPI_COMM_WORLD);
			printf("Send message %d to rank 0 from rank %d\n", message, rank);

		}
		
		//broadcast a string result from root process rank 0.
		MPI_Bcast(&result,reslen, MPI_CHAR, 0, MPI_COMM_WORLD);
		printf("process %d got %s from MPI_Bcast\n", rank, result);

	//}while(value >= 0);	//repeat above when user input not less than 0

	MPI_Finalize();
	return 0;
}
