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
			A[i] = 100 -i;
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
	else
	{	
		R_BS(A,0,COUNT -1, COUNT);
	}

MPI_Finalize();
}


void P_BS(int *A, int ll,int lh,int rl,int rh,int n)
{
int temp, lm,rm,i,j,min;
int sendparams[5];
int recvparams[5];
int *B;
if(my_id == 0)
{    
        if((lh - ll + 1) <= 25)
        {
		for(i = ll; i <= lh; i++)
                {
                        min = i;
			for(j = rl; j <=rh; j++)
                        {
                                if(A[j] < A[min])
                                {
                                        min = j;
                                }
                        }
                        if(min != i)
                        {
                                temp = A[i];
                                A[i] = A[min];
                                A[min] = temp;
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
	
                //P_BS(A,ll,lm,rl,rm,n);
                MPI_Send(&sendparams,5,MPI_INT,2,0,MPI_COMM_WORLD);
		MPI_Send(A,n,MPI_INT,2,0,MPI_COMM_WORLD);
                P_BS(A,lm+1,lh,rm+1,rh,n);
                B = malloc(sizeof(int)* n);
		MPI_Recv(B,n,MPI_INT,2,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		/*for(i = 0; i< n; i++)
		{
			printf("%d ", A[i]);
		}
		printf("\n");
		for(i = 0; i< n; i++)
                {
                        printf("%d ", B[i]);
                }*/       

		for(i = ll; i < lm; i++)
                {
                        A[i] = B[i];
                }
                for(i = rl; i < rm; i++)
                {
                        A[i] = B[i];
                }
                free (B);
		sendparams[0] = ll; 
		sendparams[1] = lm;
		sendparams[2] = rm+1;
		sendparams[3] = rh;
		MPI_Send(&sendparams,5,MPI_INT,3,0,MPI_COMM_WORLD);
		MPI_Send(A,n,MPI_INT,3,0,MPI_COMM_WORLD);
		P_BS(A,lm+1,lh,rl,rm,n);
		// Recieve
                //P_BS(A,ll,lm,rm+1,rh,n);
                B = malloc(sizeof(int) * n);
		MPI_Recv(B,n,MPI_INT,3,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		for(i = ll; i<lm; i++)
		{
			A[i]  = B[i];
		}
		for(i = rm+1; i< rh; i++ )
		{
			A[i] = B[i];
		}
		free(B);
        }
}

else if(my_id == 2 || my_id ==3)
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
                for(i = ll; i <= lh; i++)
                {
                        min = i;
			for(j = rl; j <=rh; j++)
                        {
                                if(B[j] < B[min])
                                {
                                        min = j;
                                }
                        }
                        if(min != i)
                        {
                                temp = B[i];
                                B[i] = B[min];
                                B[min] = temp;
                        }
                }
        }
	MPI_Send(B,recvparams[4],MPI_INT,0,0,MPI_COMM_WORLD);
}
}
void R_BS(int *A,int l ,int h,int n)
{
        int temp, m, i,j, recvcount, sendcount,min;
	int *B;
	if(my_id == 0) 
	{
		if((h-l + 1) <= 50)
        	{
                	for(i = l; i <=h - 1; i++)
                	{
                        	min = i;
				for(j = i+1; j <= h; j++)
                        	{
                                	if(A[j] < A[min])
                                	{
                                        	min = j;
                                	}
                        	}
				if(min != i)
				{
					temp = A[i];
					A[i] = A[min];
					A[min] = temp;
				}
                	}	
        	}
        	else
        	{
                	m = (l+h) / 2;
			P_BS(A,l,m,m+1,h,n);
			sendcount = m+1;
                        B = malloc(sizeof(int) * (m+1));
			MPI_Send(&sendcount,1,MPI_INT,1,0,MPI_COMM_WORLD);
   			MPI_Send(A, m+1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                	R_BS(A, m+1, h, n);
			MPI_Recv(B, m+1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for(i = 0; i <= m; i++)	
			{
				A[i] = B[i];
			}
			free(B);
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
		free (B);
	}
	else if(my_id == 2 || my_id == 3)
	{
	                P_BS(A,l,m,m+1,h,n);
	}
}	
