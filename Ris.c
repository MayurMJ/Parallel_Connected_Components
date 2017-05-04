#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <cilk/cilk.h>
#define COUNT  100 
void R_IS(int *A, int l, int h, int n);
void M_IS(int *A, int ll, int lh, int rl, int rh, int n);
int my_id;
int numprocs;
int main(int argc, char * argv[]) {
	int numprocs;
	int *A, i;
	if (my_id == 0)
	{
		A = (int *)malloc(sizeof(int) * COUNT);
		srand(time(NULL));
		for (i = COUNT - 1; i >= 0; i--) {
			A[i] = rand() %100;
		}
		for (i = 0; i < COUNT; i++) {
			printf("%d ", A[i]);
		}
		printf("\n");
		R_IS(A, 0, COUNT - 1, COUNT);
		for (i = 0; i < COUNT; i++) {
			printf("%d ", A[i]);
		}
		free(A);
	}
}
void R_IS(int *A, int l, int h, int n)
{
	int i, j,m, key;
	if ((h - l + 1) <= 2)
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
	if ((lh - ll + 1) <= 2)
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
