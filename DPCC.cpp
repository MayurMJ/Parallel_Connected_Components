using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdbool.h>
#include <cilk/cilk.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
struct Edge{
	int u,v;
};
bool compare_func(int i, int j);
void find_roots(int n, vector<int> &P, vector<int> &S);
void Par_Prefix_Sum(vector<int> &arr, vector<int> &out);
int par_deterministic_cc(int n,vector<Edge> &E, vector<int> &L);
int main(int argc, const char * argv[]) {
    ifstream graph_input ("input.txt");
    
    if (graph_input.is_open())
    {
    	string line;
	int in[2], value;
	getline (graph_input, line);
	std::istringstream ss(line);
	int i = 0;
	while (ss >> value)
	{
		in[i] = value;
		i++; 
	}
	vector<int> V(in[0]);
	vector<Edge> E(in[1]);
	vector<int> L(in[0]);
	int n,m;
	n = in[0];
	m = in[1];	

	i = 0;
	int j =0;
    	while ( getline (graph_input,line) )
   	{
        	std::istringstream ss(line);
       		j=0;
        	while (ss >> value)
        	{
                	in[j] = value;;
                	if (ss.peek() == ',' || ss.peek() == ' ')
                	ss.ignore();
                	j++;
        	}
        	E[i].u = in[0];
		E[i].v = in[1];
		i++;
   	}
	//for (i = 0; i < E.size(); i++)
	//{
	//	printf("%d, %d\n",E[i].u,E[i].v);
	//}
	par_deterministic_cc(n,E,L);
/*	map<int, int> CC;
	vector<int> result;
	for(int k=1;k<L.size();k++)
		CC[L[k]]++;
	
	for(map<int,int>::iterator m_iter = CC.begin();m_iter != CC.end();++m_iter){
		result.push_back(m_iter->second);
	}

	sort(result.begin(), result.end(), compare_func);	
	cout<<result.size()<<endl;
	
	for(int i=0;i<result.size();i++)
		cout<<result[i]<<endl;
*/	
  }
}
bool compare_func(int i, int j){
	return i>j;
}

void printarr(vector <int> &arr)
{
	printf("\n");
	for(int i = 0; i< arr.size(); i++)
		printf("%d\t", arr[i]);
}	
int par_deterministic_cc(int n,vector<Edge> &E, vector<int> &L)
{
	int m = E.size();
	printf("Size: %d\n", m);
	vector<int> l2h(n);
        vector<int> h2l(n);
        vector<int> S(m);
	vector<int> T(m);
	vector<int> L1(L.size());
	if(m == 0) 
	{
		return 0;
	}
	cilk_for(int i =0; i < n; i++)
	{
		l2h[i] = 0;
		h2l[i] = 0;
	}
	cilk_for(int i = 0; i < m; i++)
	{
		if( E[i].u < E[i].v)
		{
			l2h[E[i].u] = 1;
		}
		 else
		{ 
			h2l[E[i].u] = 1;
		}
	}
	printarr(l2h);
	printarr(h2l);
	//n1 = par_sum(l2h);
	//n2 = par_sum(h2l);
	int n1 = 0;
	for (int i = 0; i < l2h.size(); i++)
	{
		n1 += l2h[i];	
	}
	int n2 = 0;
       for (int i = 0; i < h2l.size(); i++)
        {
                n2 += l2h[i];
        }
	printf("\n%d\t%d",n1,n2);
	cilk_for(int i = 0; i < m; i++)
	{
		if(n1 >= n2 && E[i].u < E[i].v)
			L[E[i].u] = E[i].v;
		else if(n1 < n2 && E[i].u > E[i].v) 
			L[E[i].u] = E[i].v;
	}
	/*find_roots(n,L,L1);
	L = L1;
	cilk_for(int i = 0; i < m; i++)
	{
		if (L1[E[i].u] != L1[E[i].v])
			S[i] = 1;
		else
			S[i] = 0;
	}
	Par_Prefix_Sum(S, T);
	vector<Edge> F(T[m-1]);
	cilk_for(int i = 0; i < m; i++ )
	{
		if (L1[E[i].u] != L1[E[i].v])
		{
			F[T[i]].u = L1[E[i].u];
			F[T[i]].v = L1[E[i].v];
		} 
	}*/		
}

void find_roots(int n, vector<int> &P, vector<int> &S)
{
	bool flag = true;
	cilk_for(int v = 0; v < n; v++)
	{
		S[v] = P[v];
	}
	while(flag == true)
	{
		flag = false;
		cilk_for(int v =0; v < n; v++)
		{
			S[v] = S[S[v]];
			if(S[v]!= S[S[v]])
				flag = true;
		}
	}
	
}

void Par_Prefix_Sum(vector<int> &arr, vector<int> &out)
{
	int len = arr.size();

	if(len == 1){
		out[0] = arr[0];	
		return;
	}
	
	vector<int> y(len/2);
	vector<int> z(len/2);
	
	cilk_for(int i=0;i<len/2;i++)
	{
		for(int i=0;i<len/2;i++)
		{
			y[i] = arr[2*i]+arr[2*i+1];	
		}
	}
	
	Par_Prefix_Sum(y,z);
	cilk_for(int i=0;i<len;i++)
	{
		for(int i=0;i<len;i++)
		{
			if(i==0)
				out[i] = arr[i];										
			else if(i%2 != 0)
				out[i] = z[i/2];
			else
				out[i] = z[(i-1)/2] + arr[i];
		}
	}
}

