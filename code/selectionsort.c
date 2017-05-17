#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <cilk/cilk.h>
#include <sys/time.h>
int COUNT, b;
struct timeval start,end; 
void R_SS(int *A,int l ,int h,int n);
void P_SS(int *A, int ll,int lh,int rl,int rh,int n);
int main(int argc, char * argv[]) {
	int *A, i, tempn, tempb;
	tempn = atoi(argv[1]);
	tempb = atoi(argv[2]);
	COUNT = (1 << tempn);
    	b = (1 << tempb);

	A = (int *)malloc(sizeof(int) * COUNT);
	srand(time(NULL));
	for (i = 1; i < COUNT; i++) {
		A[i] = rand() %100;
	}
	/*for (i = 0; i < n; i++) {
		printf("%d ", A[i]);
	}*/
//	printf("\n");
        gettimeofday(&start,NULL);
	R_SS(A, 0, COUNT - 1, COUNT);
        gettimeofday(&end,NULL); //Stop timing the computation
       	double myTime = (end.tv_sec+(double)end.tv_usec/1000000) - (start.tv_sec+(double)start.tv_usec/1000000);
        printf("\n%f",myTime);
	/*for (i = 0; i < n; i++) {
		printf("%d ", A[i]);
	}*/
	free(A);
	}

	

void R_SS(int *A,int l ,int h,int n)
{
	int temp, m, i,j,min;
	if((h-l + 1) <= COUNT/2)
	{
		for(i = l; i <=h-1; i++)
		{
			min = i;
			for(j = i+1; j <= h; j++)
			{
				if(A[j] < A[min])
				{
					min = j;
				}
			}
			if (min != i)
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
		R_SS(A, l, m, n);
		R_SS(A, m+1, h, n);
	}
}
	
void P_SS(int *A, int ll,int lh,int rl,int rh,int n)
{
int temp, lm,rm,i,j,min ;	
	if((lh - ll + 1) <= b)
	{
		for(i = ll; i <= lh; i++)
		{
			min = i;
			for(j = rl; j <= rh; j++)
			{
				if(A[j] < A[min])
				{
					min = j;
				}
			}
			if (min != i)
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
		P_SS(A,ll,lm,rl,rm,n);
		P_SS(A,lm+1,lh,rm+1,rh,n);
		P_SS(A,ll,lm,rm+1,rh,n);
		P_SS(A,lm+1,lh,rl,rm,n);
	}
}
