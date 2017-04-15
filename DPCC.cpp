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

struct Edge{
	int u,v;
};

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
  }
}

	
int par_deterministic_cc(int n,vector<Edge> &E, vector<int> &L)
{
	int m = E.size();
	vector<int> l2h(n);
        vector<int> h2l(n);
        vector<int> s(m);
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
	n1 = par_sum(l2h);
	n2 = par_sum(h2l);
	cilk_for(int i = 0; i < m; i++)
	{
		if(n1 ≥ n2 && E[i].u < E[i].v)
			L[E[i].u] = E[i].v;
		else if(n1 < n2 && u > v) 
			L[E[i].u] = E[i].v;
	}
	find_roots(n,L,L);
	cilk_for(i = 1; i < m; i++)
	{
		if (L[E[i].u] != L[E[i].v])
			S[i] = 1;
		else
			S[i] = 0;
	}
	S = Par_Prefix_Sum(S);	
}






