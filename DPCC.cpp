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
	for (i = 0; i < E.size(); i++)
	{
		printf("%d, %d\n",E[i].u,E[i].v);
	}
	
  }
}






