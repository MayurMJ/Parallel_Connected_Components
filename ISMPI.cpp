#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
# include <mpi.h>
#define COUNT  100 
void R_IS(int *A,int l ,int h,int n);
void M_IS(int *A, int ll,int lh,int rl,int rh,int n);
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
			A[i] = rand() %100;	
			//A[i] = 100 -i;
		}
		for (i = 0; i < COUNT; i++) {	
			printf("%d ", A[i]);
		}
		printf("\n");
		R_IS(A,0,COUNT -1, COUNT);
		for (i = 0; i < COUNT; i++) {	
			printf("%d ", A[i]);
		}
		free(A);
	}
	else
	{	
		R_IS(A,0,COUNT -1, COUNT);
	}

MPI_Finalize();
}


void M_IS(int *A, int ll,int lh,int rl,int rh,int n)
{
int temp, lm,rm,i,j, key;
int sendparams[5];
int recvparams[5];
int *B;
if(my_id == 0)
{    
        if((lh - ll + 1) <= 25)
        {
		if (A[lh] > A[rl])
                {
                        for (i = rl; i <= rh; i++)
                        {
                                key = A[i];
                                j = i - 1;
                                while (j >= rl && A[j] > key)
                                {
                                        A[j + 1] = A[j];
                                        j = j - 1;
                                }
                                if (A[lh] > key)
                                {
                                        A[rl] = A[lh];
                                        j = lh - 1;

                                        while (j >= ll && A[j] > key)
                                        {
                                                A[j + 1] = A[j];
                                                j = j - 1;
                                        }
                                }
                                A[j + 1] = key;
                        }
                }
        }
        else
        {
                lm = (ll + lh) / 2;
                rm = (rl + rh) / 2;
	        sendparams[0] = ll;
                sendparams[1] = lm;
                sendparams[2] = rl;
                sendparams[3] = rm;
                sendparams[4] = n;
	
                //M_IS(A,ll,lm,rl,rm,n);
                MPI_Send(&sendparams,5,MPI_INT,2,0,MPI_COMM_WORLD);
		MPI_Send(A,n,MPI_INT,2,0,MPI_COMM_WORLD);
                M_IS(A,lm+1,lh,rm+1,rh,n);
                B = malloc(sizeof(int)* n);
		MPI_Recv(B,n,MPI_INT,2,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);       

		for(i = ll; i < lm; i++)
                {
                        A[i] = B[i];
                }
                for(i = rl; i < rm; i++)
                {
                        A[i] = B[i];
                }
                free (B);
		M_IS(A,lm+1,lh,rl,rm,n);
        }
}

else if(my_id == 2)
{
        MPI_Recv(&recvparams,5,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	ll = recvparams[0];
	lh = recvparams[1];
	rl = recvparams[2];
	rh = recvparams[3];
	B = malloc(sizeof(int) * n);
	MPI_Recv(B,recvparams[4],MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	if((lh - ll + 1) <= 25)
        {
                if (B[lh] > B[rl])
                {
                        for (i = rl; i <= rh; i++)
                        {
                                key = B[i];
                                j = i - 1;
                                while (j >= rl && B[j] > key)
                                {
                                        B[j + 1] = B[j];
                                        j = j - 1;
                                }
                                if (B[lh] > key)
                                {
                                        B[rl] = B[lh];
                                        j = lh - 1;

                                        while (j >= ll && B[j] > key)
                                        {
                                                B[j + 1] = B[j];
                                                j = j - 1;
                                        }
                                }
                                B[j + 1] = key;
                        }
                }
        }
	MPI_Send(B,recvparams[4],MPI_INT,0,0,MPI_COMM_WORLD);
}
}
void R_IS(int *A,int l ,int h,int n)
{
        int temp, m, i,j, recvcount, sendcount,key;
	int *B;
	if(my_id == 0) 
	{
		if((h-l + 1) <= 50)
        	{
                	for (i = l + 1; i <= h; i++)
                	{
                        	key = A[i];
                        	j = i - 1;
                        	while (j >= l && A[j] > key)
                        	{
                                	A[j + 1] = A[j];
                                	j = j - 1; 
                                	A[j + 1] = key;
                        	}
                	}
	
        	}
        	else
        	{
                	m = (l+h) / 2;
			sendcount = m+1;
                        B = malloc(sizeof(int) * (m+1));
			MPI_Send(&sendcount,1,MPI_INT,1,0,MPI_COMM_WORLD);
   			MPI_Send(A, m+1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                	R_IS(A, m+1, h, n);
			MPI_Recv(B, m+1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for(i = 0; i <= m; i++)	
			{
				A[i] = B[i];
			}
			free(B);	
			M_IS(A,l,m,m+1,h,n);
        	}
	}
	else if(my_id == 1)
	{
		
		MPI_Recv(&recvcount,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		B = malloc(sizeof(int) * recvcount);
		MPI_Recv(B,recvcount, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		l = 0;
		h = 50 -1; 
		if((h-l + 1) <= 50)
                {
			for (i = l + 1; i <= h; i++)
                	{
                        	key = B[i];
                        	j = i - 1;
                        	while (j >= l && B[j] > key)
                        	{
                                	B[j + 1] = B[j];
                                	j = j - 1; 
                                	B[j + 1] = key;
                        	}
              	  	}	
                }
		MPI_Send(B, recvcount, MPI_INT, 0, 0, MPI_COMM_WORLD);
		free (B);
	}
	else if(my_id == 2)
	{
	                M_IS(A,l,m,m+1,h,n);
	}
}	
