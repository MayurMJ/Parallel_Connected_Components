#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <cilk/cilk.h>
#include <sys/time.h>
int COUNT, b;
struct timeval start,end; 
void R_IS(int *A, int l, int h, int n);
void M_IS(int *A, int ll, int lh, int rl, int rh, int n);
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
	R_IS(A, 0, COUNT - 1, COUNT);
        gettimeofday(&end,NULL); //Stop timing the computation
       	double myTime = (end.tv_sec+(double)end.tv_usec/1000000) - (start.tv_sec+(double)start.tv_usec/1000000);
        printf("\n%f",myTime);
	/*for (i = 0; i < n; i++) {
		printf("%d ", A[i]);
	}*/
	free(A);
	}

void R_IS(int *A, int l, int h, int n)
{
	int i, j,m, key;
	if ((h - l + 1) <= COUNT / 2)
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
		m = (l + h) / 2;
		cilk_spawn R_IS(A, l, m, n);
		R_IS(A, m + 1, h, n);
		cilk_sync;
		M_IS(A, l, m, m + 1, h, n);
	}
}
void M_IS(int *A, int ll, int lh, int rl, int rh, int n)
{
	int i, j, lm, rm, key;
	if ((lh - ll + 1) <= b)
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
		cilk_spawn M_IS(A, ll, lm, rl, rm, n);
		M_IS(A, lm + 1, lh, rm + 1, rh, n);
		cilk_sync;
		M_IS(A, lm + 1, lh, rl, rm, n);
	}
}
