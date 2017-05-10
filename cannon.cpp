#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
# include <mpi.h>
#include <sys/time.h>
#include <math.h>
using namespace std;
int getprocessrankfrom2d(int i, int j);
void get2dprocessrank(int my_id, int &x, int &y);
int my_id, COUNT, rootp;
struct timeval start,end;
int p, n;
int main(int argc , char * argv []) {
        int i,j,k, sign, random, x, y;
	n = atoi(argv[1]);
        MPI_Status stat;
        MPI_Init ( &argc, &argv );
        MPI_Comm_size ( MPI_COMM_WORLD, &p);
	MPI_Comm_rank ( MPI_COMM_WORLD, &my_id );
	MPI_Request sendreq[2], recvreq[2];
	rootp = sqrt(p);
	COUNT = n / rootp;
	int* C = new int[COUNT * COUNT];
  	int* A = new int[COUNT * COUNT];
  	int* B = new int[COUNT * COUNT];
	int* Brecv = new int[COUNT * COUNT];
	int* Arecv = new int[COUNT * COUNT];	
	int* tempA = new int[COUNT * COUNT];
	int* tempB = new int[COUNT * COUNT];
	for(int i = 0; i< COUNT; i++)
	{
		for(j = 0; j < COUNT; j++)	
		{
			C[i *COUNT + j ] = 0;
		}
	}
	for(int i = 0; i< COUNT; i++)
	{
		for(int j = 0; j < COUNT; j++)
		{
			sign = rand() % 2;
			random = rand() % 100;
			if (sign == 1)
			{
				A[COUNT * i + j] = random;
			}
			else
			{
				A[COUNT * i + j]  = -1 * (random);
			}
			sign = rand() % 2;
			random = rand() % 100;
                        if (sign == 1)
                        {
                                B[COUNT * i + j]  = random;
                        } 
                        else
                        {
                                B[COUNT * i + j]  = -1 * (random);
                        }
		}
	}
	get2dprocessrank(my_id, x, y);
	// rotate x of A[x,y] by x positions to the left
	// rotate y of B[x,y] by y positions to the top
	int Ax,By,sendA, sendB, recvA, recvB;
	Ax = y - x;
	if(Ax < 0)
	{
		Ax = rootp + Ax;
	}
	By = x - y;
	if(By < 0 )
	{
		By = rootp + By;
	}
	sendA = getprocessrankfrom2d(x,Ax);
	sendB = getprocessrankfrom2d(By,y);

	
	
	//printf("\nProcessor with rank %d P[%d,%d] will send A to P[%d,%d] with rank %d",my_id,x,y,x,Ax,sendA);
	//printf("\nProcessor with rank %d P[%d,%d] will send B to P[%d,%d] with rank %d",my_id,x,y,By,y,sendB);
	
	Ax = x + y;
	Ax = Ax % rootp;
	
	By = y + x;
	By = By % rootp;
	
	recvA = getprocessrankfrom2d(x,Ax);
	recvB = getprocessrankfrom2d(By,y);

	//printf("\nProcessor with rank %d P[%d,%d] will recieve A to P[%d,%d] with rank %d",my_id,x,y,x,Ax,recvA);
        //printf("\nProcessor with rank %d P[%d,%d] will recieve B to P[%d,%d] with rank %d",my_id,x,y,By,y,recvB);
	
	// recv A
	MPI_Isend(A, COUNT * COUNT, MPI_INT, sendA, 0, MPI_COMM_WORLD,&sendreq[0]);
        MPI_Isend(B, COUNT * COUNT, MPI_INT, sendB, 0, MPI_COMM_WORLD,&sendreq[1]);


	MPI_Irecv(Arecv, COUNT * COUNT, MPI_INT, recvA, 0, MPI_COMM_WORLD,&recvreq[0]);
	MPI_Irecv(Brecv, COUNT * COUNT, MPI_INT, recvB, 0, MPI_COMM_WORLD,&recvreq[1]);
	
	
	MPI_Wait(&recvreq[0],&stat);
	MPI_Wait(&recvreq[1],&stat);

	MPI_Wait(&sendreq[0],&stat);
        MPI_Wait(&sendreq[1],&stat);


/*	if(my_id == 0)
	{
		for(i = 0; i< COUNT; i++)
		{
			printf("\n");
			for(j = 0; j < COUNT; j++)
			{
				printf("%d ",Arecv[COUNT * i + j]);
			}
		}
		printf("\n");
	}
      if(my_id == 0)
        {
                for(i = 0; i< COUNT; i++)
                {
                        printf("\n");
                        for(j = 0; j < COUNT; j++)
                        {
                                printf("%d ",Brecv[COUNT * i + j]);
                        }
                }

		
        } */ 
       for(int i = 0; i< COUNT; i++)
        {
                for(int j =0; j< COUNT; j++)
                {
                        for(int k = 0; k < COUNT; k++)
                        {
                                C[i* COUNT + j]  += Arecv[COUNT *i + k ] * Brecv[COUNT * k + j ];
                        }
                }
        }

	for(i = 0; i< rootp -1; i++)
	{
		
		Ax = y - 1;
        	if(Ax < 0)
        	{
                	Ax = rootp + Ax;
        	}
        	By = x - 1;
        	if(By < 0 )
        	{
                	By = rootp + By;
        	}
        	sendA = getprocessrankfrom2d(x,Ax);
        	sendB = getprocessrankfrom2d(By,y);
       		//printf("\nProcessor with rank %d P[%d,%d] will send A to P[%d,%d] with rank %d",my_id,x,y,x,Ax,sendA);
        	//printf("\nProcessor with rank %d P[%d,%d] will send B to P[%d,%d] with rank %d",my_id,x,y,By,y,sendB);

        	Ax = y + 1;
        	Ax = Ax % rootp;
        	By = x + 1;
        	By = By % rootp;

	        recvA = getprocessrankfrom2d(x,Ax);
        	recvB = getprocessrankfrom2d(By,y);
		
		
        	//printf("\nProcessor with rank %d P[%d,%d] will recieve A from P[%d,%d] with rank %d",my_id,x,y,x,Ax,recvA);
                //printf("\nProcessor with rank %d P[%d,%d] will recieve B from P[%d,%d] with rank %d",my_id,x,y,By,y,recvB);
              
			
		//tempA = Arecv;
		//tempB = Brecv;

		MPI_Isend(tempA, COUNT * COUNT, MPI_INT, sendA, 0, MPI_COMM_WORLD,&sendreq[0]);
                MPI_Isend(tempB, COUNT * COUNT, MPI_INT, sendB, 0, MPI_COMM_WORLD,&sendreq[1]);


		MPI_Irecv(Arecv, COUNT * COUNT, MPI_INT, recvA, 0, MPI_COMM_WORLD,&recvreq[0]);
        	MPI_Irecv(Brecv, COUNT * COUNT, MPI_INT, recvB, 0, MPI_COMM_WORLD,&recvreq[1]);

		MPI_Wait(&recvreq[0],&stat);
       		MPI_Wait(&recvreq[1],&stat);

        	MPI_Wait(&sendreq[0],&stat);
       		MPI_Wait(&sendreq[1],&stat);

		for(int i = 0; i< COUNT; i++)
        	{
                	for(int j =0; j< COUNT; j++)
                	{
                        	for(int k = 0; k < COUNT; k++)
                        	{
                                	C[i* COUNT + j]  += Arecv[COUNT *i + k ] * Brecv[COUNT * k + j ];
                        	}
                	}
        	}
	}	
	

	

	delete [] C;
	delete [] A;
	delete [] B;
	delete [] Arecv;
	delete [] Brecv;
	delete [] tempA;
	MPI_Finalize();
}

void get2dprocessrank(int my_id, int &x, int &y)
{
	y = my_id % rootp;
	x = my_id / rootp;	
}
int getprocessrankfrom2d(int i, int j)
{
	return((rootp * i) + j);
}

