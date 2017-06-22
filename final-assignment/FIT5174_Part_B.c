#include <stdio.h>
#include <time.h>
#include "mpi.h"
#include <string.h>

// Your naval fleet patrols an area comprising 1500 distinct locations. Each vessel can occupy any one of these locations at random. 
// For a vessel to be able to launch a strike, other vessels must accompany it. The rules for launching a strike are summarised as follows: 
// 1. At least four vessels must share the same location, at a given point in time, for a strike to be counted. 
// 2. The fleet may generate more than one strike at an instant of time. It will however depend on the number of locations with four or more vessels present at that instant of time. 
// 3. There is no limit on the number of vessels in the fleet. The objective is to achieve the highest possible strike rate. 
// 	It may however be pointed out that increases in fleet size will increase the probability of satisfying Rule no. 1 (above), but doing so will also slow the program owing to higher inter-­‐process communication overheads. 
// 4. Correct submissions that achieve the highest strike rates over a period of 60 seconds, wall clock time, may be awarded certificates of recognition. Assume that a set of MPI processes represents the fleet and each MPI process represents a naval vessel. 


int main(argc,argv)
int argc;
char **argv;
{
	int rank;	
	int value, locvalue, result,count;
	int i,j;
	int a;
	//double t,seed;
	int chksum[1501]={};
	double rate;
	int size;	//define rank, size and the int data use in send and recv
	
	
	MPI_Status status; //define the status agument which will use at MPI_Recv, represents message source and tag
	MPI_Init(&argc,&argv);

	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);	//set up MPI environment
	
	result = 0;
	count=0;
	time_t tInit;
	time(&tInit);
	int timeIntrv = 60;
	//Loop progress for 60 seconds
	while(((int)difftime(time(NULL),tInit))<timeIntrv){	
	
		count++;

	//t=MPI_Wtime();
	//printf("current time %f\n",t);
	//do{
		if(rank==0){
			memset(chksum, 0, sizeof(chksum));
			for(i=1;i<size;i++){ 
			MPI_Recv(&locvalue,1,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&status);
			//printf("Recv location %d\n", locvalue);
			chksum[locvalue] = chksum[locvalue] + 1;
			//printf("chksum %d have value %d\n", locvalue, chksum[locvalue]);
			}
			
			a = 0;
			for(i=0; i<1501;i++){
				a = a + chksum[i];
			}
			if(a != size -1){
				printf("%d Did not pass the overall size check.\n", a);
				MPI_Abort(MPI_COMM_WORLD,1);
			}
					
			for(i=0; i<1501;i++){
				if(chksum[i] >= 4){
					result = result + 1;
				}
			}
			printf("In all %d time in 60sec, result is %d",count, result);
			rate=(double)result/(double)count; 
			printf(",rate is %f\n",rate);
		}
		else{	

/*
			seed=MPI_Wtime()*100000;
			srand((int)seed+rank+j);
*/
		unsigned int se;
		FILE* urandom = fopen("/dev/urandom","r");
		fread(&se,sizeof(int),1,urandom);
		fclose(urandom);
		srand(se);	
		
			value = rand()%1500 + 1;
			MPI_Send(&value,1,MPI_INT,0,0,MPI_COMM_WORLD);	
			//printf("send location %d to rank 0 from rank %d\n", value, rank);	
		}
	//	printf("process %d got %d\n", rank, value);
	//}while(MPI_Wtime()<=t+60);
}
	MPI_Finalize();
	

	return 0;
}


