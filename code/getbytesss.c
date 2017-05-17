#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
# include <mpi.h>
#include <sys/time.h>
int COUNT;
void R_SS(int *A,int l ,int h,int n);
void P_SS(int *A, int ll,int lh,int rl,int rh,int n);
int my_id;
struct timeval start,end;
int numprocs, bytes_recv;
int b;
MPI_Status status;
int main(int argc , char * argv []) {
	COUNT = 1 << atoi(argv[1]);
	b = 1 << atoi(argv[2]);
 	MPI_Status status;
 	MPI_Init ( &argc, &argv );
  	MPI_Comm_size ( MPI_COMM_WORLD, &numprocs );
  	MPI_Comm_rank ( MPI_COMM_WORLD, &my_id );
	int *A,i;
	if(my_id == 0)
	{
		A = (int *) malloc(sizeof(int) * COUNT);
		srand(time(NULL));	
		for (i = 0; i < COUNT; i++) {
			A[i] = rand() %100;	
		}
		//for (i = 0; i < COUNT; i++) {	
		//	printf("%d ", A[i]);
		//}
		//printf("\nnumprocs %d", numprocs);
                gettimeofday(&start,NULL);
		R_SS(A,0,COUNT -1, COUNT);
                gettimeofday(&end,NULL); //Stop timing the computation
                double myTime = (end.tv_sec+(double)end.tv_usec/1000000) - (start.tv_sec+(double)start.tv_usec/1000000);
                 printf("\n%f",myTime);

		/*for (i = 0; i < COUNT; i++) {	
			printf("%d ", A[i]);
		}*/
		free(A);
	}
	else
	{	
		R_SS(A,0,COUNT -1, COUNT);
	}

	int total_bytes;
	MPI_Reduce(&bytes_recv, &total_bytes, 1, MPI_INT, MPI_SUM, 0,
           MPI_COMM_WORLD);
	if(my_id == 0)
	{
		printf("\nBytes Recieved, %d",total_bytes );
	}
MPI_Finalize();
}


void P_SS(int *A, int ll,int lh,int rl,int rh,int n)
{
int temp, lm,rm,i,j,source,dest,min,msg_size;
int sendparams[5];
int recvparams[5];
int *B, *C;
if(my_id == 0)
{    
        /*if((lh - ll + 1) <= 25)
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
        }*/
        //else
        {
                lm = (ll + lh) / 2;
                rm = (rl + rh) / 2;
	        sendparams[0] = ll;
                sendparams[1] = lm;
                sendparams[2] = rl;
                sendparams[3] = rm;
                sendparams[4] = n;
	
                //P_SS(A,ll,lm,rl,rm,n);
                MPI_Send(&sendparams,5,MPI_INT,2,0,MPI_COMM_WORLD);
		MPI_Send(A,n,MPI_INT,2,0,MPI_COMM_WORLD);
                //P_SS(A,lm+1,lh,rm+1,rh,n);
                sendparams[0] = lm+1;
		sendparams[1] = lh;
		sendparams[2] = rm+1;
		sendparams[3] = rh;
		sendparams[4] = n;
		MPI_Send(&sendparams,5,MPI_INT,3,0,MPI_COMM_WORLD);
		MPI_Send(A,n,MPI_INT,3,0,MPI_COMM_WORLD);
                B = malloc(sizeof(int)* n);
		MPI_Recv(B,n,MPI_INT,2,0,MPI_COMM_WORLD,&status);
 		MPI_Get_count(&status, MPI_BYTE, &msg_size);
  		bytes_recv += msg_size;

		for(i = ll; i <= lm; i++)
                {
                        A[i] = B[i];
                }
                for(i = rl; i <= rm; i++)
                {
                        A[i] = B[i];
                }
                printf("\n");
		/*for(i = 0; i< n; i++)
                {
                        printf("%d ", A[i]);
                }*/
                free (B);
		B = malloc(sizeof(int)* n);
		MPI_Recv(B,n,MPI_INT,3,0,MPI_COMM_WORLD,&status);
 		MPI_Get_count(&status, MPI_BYTE, &msg_size);
  		bytes_recv += msg_size;
               /* printf("\nReturned B\n");
                for(i = 0; i< n; i++)
                {
                        printf("%d ", B[i]);
                }*/	
		for(i = lm+1; i <=lh; i++)
                {
                        A[i] = B[i];
                }
                for(i = rm+1; i <=rh; i++)
                {
                        A[i] = B[i];
                }
                free (B);	
		sendparams[0] = ll; 
		sendparams[1] = lm;
		sendparams[2] = rm+1;
		sendparams[3] = rh;
		sendparams[4] = n;
		MPI_Send(&sendparams,5,MPI_INT,2,0,MPI_COMM_WORLD);
		MPI_Send(A,n,MPI_INT,2,0,MPI_COMM_WORLD);
		//P_SS(A,lm+1,lh,rl,rm,n);
		sendparams[0] = lm+1; 
		sendparams[1] = lh;
		sendparams[2] = rl;
		sendparams[3] = rm;
		sendparams[4] = n;
		MPI_Send(&sendparams,5,MPI_INT,3,0,MPI_COMM_WORLD);
		MPI_Send(A,n,MPI_INT,3,0,MPI_COMM_WORLD);
		// Recieve
                //P_SS(A,ll,lm,rm+1,rh,n);
                B = malloc(sizeof(int) * n);
		MPI_Recv(B,n,MPI_INT,2,0,MPI_COMM_WORLD,&status);
 		MPI_Get_count(&status, MPI_BYTE, &msg_size);
  		bytes_recv += msg_size;
		for(i = ll; i<=lm; i++)
		{
			A[i]  = B[i];
		}
		for(i = rm+1; i<= rh; i++ )
		{
			A[i] = B[i];
		}
		free(B);
                B = malloc(sizeof(int) * n);
		MPI_Recv(B,n,MPI_INT,3,0,MPI_COMM_WORLD,&status);
 		MPI_Get_count(&status, MPI_BYTE, &msg_size);
  		bytes_recv += msg_size;
		for(i = lm+1; i<=lh; i++)
		{
			A[i]  = B[i];
		}
		for(i = rl; i<=rm; i++ )
		{
			A[i] = B[i];
		}
		free(B);
		sendparams[0] = -1;
                sendparams[1] = lh;
                sendparams[2] = rl;
                sendparams[3] = rm;
                sendparams[4] = n;
                MPI_Send(&sendparams,5,MPI_INT,3,0,MPI_COMM_WORLD);
                MPI_Send(&sendparams,5,MPI_INT,2,0,MPI_COMM_WORLD);
        }
}

else if(my_id > 1)
{
        while (1)
	{
		source = my_id/2;
		dest = my_id /2;
		if(source == 1)
			source = 0;
		if(dest == 1)
			dest = 0;
		MPI_Recv(&recvparams,5,MPI_INT,source,0,MPI_COMM_WORLD,&status);
 		MPI_Get_count(&status, MPI_BYTE, &msg_size);
  		bytes_recv += msg_size;
		ll = recvparams[0];
		lh = recvparams[1];
		rl = recvparams[2];
		rh = recvparams[3];
		n  = recvparams[4];
		if(ll == -1)
		{
			sendparams[0] = -1;
                        sendparams[1] = lh;
                        sendparams[2] = rl;
                        sendparams[3] = rm;
                        sendparams[4] = n;
			if(my_id < numprocs /2)
			{
                        	MPI_Send(&sendparams,5,MPI_INT,(2*my_id),0,MPI_COMM_WORLD);
                       	 	MPI_Send(&sendparams,5,MPI_INT,(2*my_id + 1),0,MPI_COMM_WORLD); 
			}
			break;
		}	
		B = malloc(sizeof(int) * n);
		MPI_Recv(B,recvparams[4],MPI_INT,source,0,MPI_COMM_WORLD,&status);
 		MPI_Get_count(&status, MPI_BYTE, &msg_size);
  		bytes_recv += msg_size;
		if(my_id >= numprocs/2 || (lh - ll + 1) <= b)
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
		else
		{	
		  //copy start
                	lm = (ll + lh) / 2;
               		rm = (rl + rh) / 2;
	        	sendparams[0] = ll;
                	sendparams[1] = lm;
                	sendparams[2] = rl;
                	sendparams[3] = rm;
                	sendparams[4] = n;
	
                //P_SS(A,ll,lm,rl,rm,n);
                	MPI_Send(&sendparams,5,MPI_INT,(2*my_id),0,MPI_COMM_WORLD);
			MPI_Send(B,n,MPI_INT,(2*my_id),0,MPI_COMM_WORLD);
                //P_SS(A,lm+1,lh,rm+1,rh,n);
                	sendparams[0] = lm+1;
			sendparams[1] = lh;
			sendparams[2] = rm+1;
			sendparams[3] = rh;
			sendparams[4] = n;
			MPI_Send(&sendparams,5,MPI_INT,(2*my_id + 1),0,MPI_COMM_WORLD);
			MPI_Send(B,n,MPI_INT,(2*my_id +1),0,MPI_COMM_WORLD);
                	C = malloc(sizeof(int)* n);
			MPI_Recv(C,n,MPI_INT,(2* my_id),0,MPI_COMM_WORLD,&status);
 		MPI_Get_count(&status, MPI_BYTE, &msg_size);
  		bytes_recv += msg_size;
			for(i = ll; i <= lm; i++)
                	{
                        	B[i] = C[i];
                	}
                	for(i = rl; i <= rm; i++)
                	{
                        	B[i] = C[i];
                	}
                	free (C);
			C = malloc(sizeof(int)* n);
			MPI_Recv(C,n,MPI_INT,(2*my_id +1),0,MPI_COMM_WORLD,&status);
 		MPI_Get_count(&status, MPI_BYTE, &msg_size);
  		bytes_recv += msg_size;
			for(i = lm+1; i <= lh; i++)
                	{
                        	B[i] = C[i];
                	}
                	for(i = rm+1; i <= rh; i++)
                	{
                        	B[i] = C[i];
                	}
                	free (C);	
			sendparams[0] = ll; 
			sendparams[1] = lm;
			sendparams[2] = rm+1;
			sendparams[3] = rh;
			sendparams[4] = n;
			MPI_Send(&sendparams,5,MPI_INT,(2*my_id),0,MPI_COMM_WORLD);
			MPI_Send(B,n,MPI_INT,(2*my_id),0,MPI_COMM_WORLD);
			//P_SS(A,lm+1,lh,rl,rm,n);
			sendparams[0] = lm+1; 
			sendparams[1] = lh;
			sendparams[2] = rl;
			sendparams[3] = rm;
			sendparams[4] = n;
			MPI_Send(&sendparams,5,MPI_INT,(2* my_id + 1),0,MPI_COMM_WORLD);
			MPI_Send(B,n,MPI_INT,(2 * my_id +1),0,MPI_COMM_WORLD);
			// Recieve
                	//P_SS(A,ll,lm,rm+1,rh,n);
               	 	C = malloc(sizeof(int) * n);
			MPI_Recv(C,n,MPI_INT,(2 * my_id),0,MPI_COMM_WORLD,&status);
 		MPI_Get_count(&status, MPI_BYTE, &msg_size);
  		bytes_recv += msg_size;
			for(i = ll; i<=lm; i++)
			{
				B[i]  = C[i];
			}
			for(i = rm+1; i<= rh; i++ )
			{
				B[i] = C[i];
			}
			free(C);
                	C = malloc(sizeof(int) * n);
			MPI_Recv(C,n,MPI_INT,(2 * my_id + 1),0,MPI_COMM_WORLD,&status);
 		MPI_Get_count(&status, MPI_BYTE, &msg_size);
  		bytes_recv += msg_size;
			for(i = lm+1; i<=lh; i++)
			{
				B[i]  = C[i];
			}
			for(i = rl; i<= rm; i++ )
			{	
				B[i] = C[i];
			}
			free(C);
			//sendparams[0] = -1;
                	//sendparams[1] = lh;
                	//sendparams[2] = rl;
                //	sendparams[3] = rm;
                	//sendparams[4] = n;
                	//MPI_Send(&sendparams,5,MPI_INT,(2*my_id),0,MPI_COMM_WORLD);
                	//MPI_Send(&sendparams,5,MPI_INT,(2*my_id + 1),0,MPI_COMM_WORLD);	
		}
		MPI_Send(B,recvparams[4],MPI_INT,dest,0,MPI_COMM_WORLD);
	}
}
}
void R_SS(int *A,int l ,int h,int n)
{
        int temp, m, i,j, recvcount, sendcount,min, msg_size;
	int *B;
	if(my_id == 0) 
	{
		if((h-l + 1) <= COUNT/2)
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
			P_SS(A,l,m,m+1,h,n);
			sendcount = m+1;
                        B = malloc(sizeof(int) * (m+1));
			MPI_Send(&sendcount,1,MPI_INT,1,0,MPI_COMM_WORLD);
   			MPI_Send(A, m+1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                	R_SS(A, m+1, h, n);
			MPI_Recv(B, m+1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
 		MPI_Get_count(&status, MPI_BYTE, &msg_size);
  		bytes_recv += msg_size;
			for(i = 0; i <= m; i++)	
			{
				A[i] = B[i];
			}
			free(B);
        	}
	}
	else if(my_id == 1)
	{
		
		MPI_Recv(&recvcount,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
 		MPI_Get_count(&status, MPI_BYTE, &msg_size);
  		bytes_recv += msg_size;
		B = malloc(sizeof(int) * recvcount);
		MPI_Recv(B,recvcount, MPI_INT, 0, 0, MPI_COMM_WORLD,&status);
 		MPI_Get_count(&status, MPI_BYTE, &msg_size);
  		bytes_recv += msg_size;
		l = 0;
		h = recvcount -1; 
		if((h-l + 1) <= COUNT/2)
                {
			for(i = l; i <=h - 1; i++)
                        {
                                min = i;
                                for(j = i+1; j <= h; j++)
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
		MPI_Send(B, recvcount, MPI_INT, 0, 0, MPI_COMM_WORLD);
		free (B);
	}
	else if(my_id > 1)
	{
	                P_SS(A,l,m,m+1,h,n);
	}
}	
