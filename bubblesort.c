#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
# include <mpi.h>
#define COUNT  100 
void R_BS(int *A,int l ,int h,int n);
void P_BS(int *A, int ll,int lh,int rl,int rh,int n);
int my_id;
int numprocs;
int main(int argc , char * argv []) {
	int numprocs;		
 	MPI_Status status;
 	MPI_Init ( &argc, &argv );
  	MPI_Comm_size ( MPI_COMM_WORLD, &numprocs );
  	MPI_Comm_rank ( MPI_COMM_WORLD, &my_id );
	int *A,i;
	if(my_id == 0)
	{
		A = (int *) malloc(sizeof(int) * COUNT);
		srand(time(NULL));	
		for (i = COUNT - 1; i >= 0; i--) {
			//A[i] = rand() %100;	
			A[i] = i;
		}
		for (i = 0; i < COUNT; i++) {	
			printf("%d ", A[i]);
		}
		printf("\n");
		R_BS(A,0,COUNT -1, COUNT);
		for (i = 0; i < COUNT; i++) {	
			printf("%d ", A[i]);
		}
		free(A);
	}
}


void P_BS(int *A, int ll,int lh,int rl,int rh,int n)
{
int temp, lm,rm,i,j ;    
        if((lh - ll + 1) <= 2)
        {
                for(i = rl; i <= rh; i++)
                {
                        for(j = ll; j <lh; j++)
                        {
                                if(A[j] > A[j+1])
                                {
                                        temp = A[j];
                                        A[j] = A[j+1];
                                        A[j+1] = temp;
                                }
                        }
                        if(A[lh] > A[i])
                        {
                                temp = A[lh];
                                A[lh] = A[i];
                                A[i] = temp;
                        }
                }
        }
        else
        {
                lm = (ll + lh) / 2;
                rm = (rl + rh) / 2;
		// send
                P_BS(A,ll,lm,rl,rm,n);
                P_BS(A,lm+1,lh,rm+1,rh,n);
		// Recieve
                P_BS(A,ll,lm,rm+1,rh,n);
                P_BS(A,lm+1,lh,rl,rm,n);
        }
}





void R_BS(int *A,int l ,int h,int n)
{
        int temp, m, i,j, recvcount;
	int *B;
	if(my_id == 0) 
	{
        	if((h-l + 1) <= 50)
        	{
                	for(i = l; i <=h; i++)
                	{
                        	for(j = l; j <= l + h - i - 1; j++)
                        	{
                                	if(A[j] > A[j+1])
                                	{
                                        	temp = A[j];
                                       		 A[j] = A[j+1];
                                        	A[j+1] = temp;
                                	}
                        	}
                	}	
        	}
        	else
        	{
                	m = (l+h) / 2;
                	B = malloc(sizeof(int) * (m+1));
   			MPI_Send(A, m+1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                	R_BS(A, m+1, h, n);
			MPI_Recv(B, m+1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for(i = 0; i <= m; i++)	
			{
				A[i + l] = B[i];
			}
			free(B);
        	}
	}
	else if(my_id == 1)
	{
		B = malloc(sizeof(int) * recvcount);
		MPI_Recv(B, recvcount, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		l = 0;
		h = recvcount -1; 
		if((h-l + 1) <= 50)
                {
                        for(i = l; i <=h; i++)
                        {
                                for(j = l; j <= l + h - i - 1; j++)
                                {
                                        if(B[j] > B[j+1])
                                        {
                                                temp = B[j];
                                                 B[j] = B[j+1];
                                                B[j+1] = temp;
                                        }
                                }
                        }
                }
		MPI_Send(B, recvcount, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
}	
