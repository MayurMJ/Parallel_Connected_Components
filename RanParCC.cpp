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
struct timeval start,end;


#define HEAD true
#define TAIL false

struct Edge {
	long u, v;
};

bool compare_func(long i, long j);
void Par_Prefix_Sum(vector<long> &arr, vector<long> &out);
int par_random_cc(long n, vector<Edge> &E, vector<long> &L, vector<long> &M);

int main(long argc, const char * argv[]) {

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
		m = (2*in[1]) + 1;
		vector<long> V(n);
		vector<Edge> E(m);
		vector<long> L(n);
		vector<long> M(n);
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
		cilk_for (long i = 1; i < n; i++)
		{
			M[i] = i;	
		}
		gettimeofday(&start,NULL); 
                par_random_cc(n, E, L, M);
                gettimeofday(&end,NULL); //Stop timing the computation
                double myTime = (end.tv_sec+(double)end.tv_usec/1000000) - (start.tv_sec+(double)start.tv_usec/1000000);
                cout << argv[1] << ": Implemented in: " << myTime << " seconds.\n";

		map<long, long> CC;
		vector<long> result;
		for (long k = 1; k<M.size(); k++)
			CC[M[k]]++;

		for (map<long, long>::iterator m_iter = CC.begin(); m_iter != CC.end(); ++m_iter) {
			result.push_back(m_iter->second);
		}

		sort(result.begin(), result.end(), compare_func);
		cout << result.size() << endl;

		for (long i = 0; i<result.size(); i++)
			cout << result[i] << endl;
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
int par_random_cc(long n, vector<Edge> &E, vector<long> &L, vector<long> &M)
{
	long m = E.size();
	vector<long> C(n);
	vector<long> S(m);
	vector<long> T(m);
	if (m == 1)
	{
		return 0;
	}
	cilk_for (long i = 1; i<n; i++) {
		double r = (double)rand() / RAND_MAX;
		C[i] = (r >= 0.5) ? HEAD : TAIL;
	}

	cilk_for (long i = 1; i < m; i++)
	{
		if (C[E[i].u] == TAIL && C[E[i].v] == HEAD)
		{
			L[E[i].u] = L[E[i].v];
		}
	}

	cilk_for (long i = 1; i < m; i++)
	{
		if (L[E[i].u] != L[E[i].v])
		{
			S[i] = 1;
		}
		else
		{
			S[i] = 0;
		}
	}
	Par_Prefix_Sum(S, T);
	vector<Edge> F(T[m - 1] + 1);
	cilk_for (long i = 1; i < m; i++)
	{
		if (L[E[i].u] != L[E[i].v])
		{
			F[T[i]].u = L[E[i].u];
			F[T[i]].v = L[E[i].v];
		}
	}
	par_random_cc(n, F, L, M);
	cilk_for (long i = 1; i < m; i++)
	{
		if (E[i].v == L[E[i].u])
			M[E[i].u] = M[E[i].v];
	}
}

void Par_Prefix_Sum(vector<long> &arr, vector<long> &out)
{
	int len = arr.size();

	if (len == 1) {
		out[0] = arr[0];
		return;
	}

	vector<long> ll(len / 2);
	vector<long> lr(len / 2);

	cilk_for (long i = 0; i<len / 2; i++)
	{
		ll[i] = arr[2 * i] + arr[2 * i + 1];
	}

	Par_Prefix_Sum(ll, lr);
	cilk_for (long i = 0; i<len; i++)
	{
		if (i == 0)
			out[i] = arr[i];
		else if (i % 2 != 0)
			out[i] = lr[i / 2];
		else
			out[i] = lr[(i - 1) / 2] + arr[i];
	}
}

