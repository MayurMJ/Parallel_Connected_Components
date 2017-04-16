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
	int u, v;
};

bool compare_func(int i, int j);
void Par_Prefix_Sum(vector<int> &arr, vector<int> &out);
int par_random_cc(int n, vector<Edge> &E, vector<int> &L, vector<int> &M);

int main(int argc, const char * argv[]) {

	ifstream graph_input(argv[1]);
	if (graph_input.is_open())
	{
		string line;
		int in[2], value;
		getline(graph_input, line);
		std::istringstream ss(line);
		int i = 0;
		while (ss >> value)
		{
			in[i] = value;
			i++;
		}
		int n, m;
		n = in[0] + 1;
		m = (2*in[1]) + 1;
		vector<int> V(n);
		vector<Edge> E(m);
		vector<int> L(n);
		vector<int> M(n);
		i = 1;
		int j = 0;
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

		cilk_for (int i = 1; i < n; i++)
		{
			L[i] = i;
		}
		cilk_for (int i = 1; i < n; i++)
		{
			M[i] = i;	
		}
		gettimeofday(&start,NULL); 
                par_random_cc(n, E, L, M);
                gettimeofday(&end,NULL); //Stop timing the computation
                double myTime = (end.tv_sec+(double)end.tv_usec/1000000) - (start.tv_sec+(double)start.tv_usec/1000000);
                cout << argv[1] << ": Implemented in: " << myTime << " seconds.\n";

		map<int, int> CC;
		vector<int> result;
		for (int k = 1; k<M.size(); k++)
			CC[M[k]]++;

		for (map<int, int>::iterator m_iter = CC.begin(); m_iter != CC.end(); ++m_iter) {
			result.push_back(m_iter->second);
		}

		sort(result.begin(), result.end(), compare_func);
		cout << result.size() << endl;

		for (int i = 0; i<result.size(); i++)
			cout << result[i] << endl;
	}
}
bool compare_func(int i, int j) {
	return i>j;
}

void printarr(vector <int> &arr)
{
	printf("\n");
	for (int i = 0; i< arr.size(); i++)
		printf("%d\t", arr[i]);
}
int par_random_cc(int n, vector<Edge> &E, vector<int> &L, vector<int> &M)
{
	int m = E.size();
	vector<int> C(n);
	vector<int> S(m);
	vector<int> T(m);
	if (m == 1)
	{
		return 0;
	}
	cilk_for (int i = 1; i<n; i++) {
		double r = (double)rand() / RAND_MAX;
		C[i] = (r >= 0.5) ? HEAD : TAIL;
	}

	cilk_for (int i = 1; i < m; i++)
	{
		if (C[E[i].u] == TAIL && C[E[i].v] == HEAD)
		{
			L[E[i].u] = L[E[i].v];
		}
	}

	cilk_for (int i = 1; i < m; i++)
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
	cilk_for (int i = 1; i < m; i++)
	{
		if (L[E[i].u] != L[E[i].v])
		{
			F[T[i]].u = L[E[i].u];
			F[T[i]].v = L[E[i].v];
		}
	}
	par_random_cc(n, F, L, M);
	cilk_for (int i = 1; i < m; i++)
	{
		if (E[i].v == L[E[i].u])
			M[E[i].u] = M[E[i].v];
	}
}

void Par_Prefix_Sum(vector<int> &arr, vector<int> &out)
{
	int len = arr.size();

	if (len == 1) {
		out[0] = arr[0];
		return;
	}

	vector<int> y(len / 2);
	vector<int> z(len / 2);

	cilk_for (int i = 0; i<len / 2; i++)
	{
		y[i] = arr[2 * i] + arr[2 * i + 1];
	}

	Par_Prefix_Sum(y, z);
	cilk_for (int i = 0; i<len; i++)
	{
		if (i == 0)
			out[i] = arr[i];
		else if (i % 2 != 0)
			out[i] = z[i / 2];
		else
			out[i] = z[(i - 1) / 2] + arr[i];
	}
}

