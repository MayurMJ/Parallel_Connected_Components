#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm> 
#include <cilk/cilk.h>
#include <sys/time.h>
using namespace std;
struct Edge {
	long u, v;
};
struct timeval start,end;
void Parallel_Sum(vector<long> &A, long n);
bool compare_func(long i, long j);
void find_roots(long n, vector<long> &P, vector<long> &S);
void Par_Prefix_Sum(vector<long> &arr, vector<long> &out);
int par_deterministic_cc(long n, vector<Edge> &E, vector<long> &L);

int main(int argc, const char * argv[]) {
	ifstream graph_input(argv[1]);
	if (graph_input.is_open())
	{
		string line;
		long in[2], value;
		getline(graph_input, line);
		std::istringstream ss(line);
		long i = 0;
		while (ss >> value)
		{
			in[i] = value;
			i++;
		}
		long n, m;
		n = in[0] + 1;
		m = (2 * in[1]) + 1;
		vector<Edge> E(m);
		vector<long> L(in[0] + 1);
		i = 1;
		long j = 0;
		while (getline(graph_input, line))
		{
			std::istringstream ss(line);
			j = 0;
			while (ss >> value)
			{
				in[j] = value;;
				if (ss.peek() == ',' || ss.peek() == ' ')
					ss.ignore();
				j++;
			}
			E[i].u = in[0];
			E[i].v = in[1];
			E[i+1].u = in[1];
			E[i+1].v = in[0];
			i= i+2;
		}

		cilk_for (long i = 1; i < n; i++)
		{
			L[i] = i;
		}
		gettimeofday(&start,NULL); 
		par_deterministic_cc(n, E, L);
		gettimeofday(&end,NULL); //Stop timing the computation
    		double myTime = (end.tv_sec+(double)end.tv_usec/1000000) - (start.tv_sec+(double)start.tv_usec/1000000);
    		cout << argv[1] << ": Implemented in: " << myTime << " seconds.\n";
	
		
		map<long, long> CC;
		vector<long> result;
		for(long k=1;k<L.size();k++)
		CC[L[k]]++;

		for(map<long,long>::iterator m_iter = CC.begin();m_iter != CC.end();++m_iter){
		result.push_back(m_iter->second);
		}

		sort(result.begin(), result.end(), compare_func);
		cout<<result.size()<<endl;

		for(long i=0;i<result.size();i++)
		cout<<result[i]<<endl;
	}
}
bool compare_func(long i, long j) {
	return i>j;
}

void printarr(vector <long> &arr)
{
	printf("\n");
	for (long i = 0; i< arr.size(); i++)
		printf("%d\t", arr[i]);
}
int par_deterministic_cc(long n, vector<Edge> &E, vector<long> &L)
{
	long m = E.size();
	vector<long> l2h(n);
	vector<long> h2l(n);
	vector<long> S(m);
	vector<long> T(m);
	vector<long> L1(L.size());
	if (m == 1)
	{
		return 0;
	}
	cilk_for(long i = 1; i < n; i++)
	{
		l2h[i] = 0;
		h2l[i] = 0;
	}
	cilk_for(long i = 1; i < m; i++)
	{
		if (E[i].u < E[i].v)
		{
			l2h[E[i].u] = 1;
		}
		else
		{
			h2l[E[i].u] = 1;
		}
	}
	long n1, n2;
	Parallel_Sum(l2h,1);
	Parallel_Sum(h2l,1);
	n1 = l2h[1];
	n2 = h2l[1];
/*	for (int i = 1; i < l2h.size(); i++)
	{
		n1 += l2h[i];
	}
	int n2 = 0;
	for (int i = 1; i < h2l.size(); i++)
	{
		n2 += h2l[i];
	}
*/
	cilk_for(long i = 1; i < m; i++)
	{
		if (n1 >= n2 && E[i].u < E[i].v)
			L[E[i].u] = E[i].v;
		else if (n1 < n2 && E[i].u > E[i].v)
			L[E[i].u] = E[i].v;
	}
	find_roots(n,L,L1);
	L = L1;
	cilk_for(long i = 1; i < m; i++)
	{
		if (L1[E[i].u] != L1[E[i].v])
		S[i] = 1;
		else
		S[i] = 0;
	}
	Par_Prefix_Sum(S, T);
	vector<Edge> F(T[m-1] + 1);
	cilk_for(long i = 1; i < m; i++ )
	{
		if (L1[E[i].u] != L1[E[i].v])
		{
			F[T[i]].u = L1[E[i].u];
			F[T[i]].v = L1[E[i].v];
		}
	}
	par_deterministic_cc(n, F, L);
}

void find_roots(long n, vector<long> &P, vector<long> &S)
{
	bool flag = true;
	cilk_for (long v= 1; v < n; v++)
	{
		S[v] = P[v];
	}
	while (flag == true)
	{
		flag = false;
		cilk_for (long v = 1; v < n; v++)
		{
			S[v] = S[S[v]];
			if (S[v] != S[S[v]])
				flag = true;
		}
	}

}

void Par_Prefix_Sum(vector<long> &arr, vector<long> &out)
{
	long len = arr.size();

	if (len == 1) {
		out[0] = arr[0];
		return;
	}

	vector<long> y(len / 2);
	vector<long> z(len / 2);

	cilk_for (long i = 0; i<len / 2; i++)
	{
		y[i] = arr[2 * i] + arr[2 * i + 1];
	}

	Par_Prefix_Sum(y, z);
	cilk_for (long i = 0; i<len; i++)
	{
		if (i == 0)
			out[i] = arr[i];
		else if (i % 2 != 0)
			out[i] = z[i / 2];
		else
			out[i] = z[(i - 1) / 2] + arr[i];
	}
}
void Parallel_Sum(vector<long> &A, long n)
{
	cilk_for (long i = 1; i < A.size(); i = i + (2 * n))
	{
		if ((i + n) < A.size())
			A[i] = A[i] + A[i + n];
	}
	if ((1 + n) >= A.size() - 1)
		return;

	Parallel_Sum(A, 2 * n);
}

