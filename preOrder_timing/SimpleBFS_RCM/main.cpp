#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <set>
#include <functional>
#include <climits>
#include <ctime>
#include <stdlib.h>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <chrono>
#include <sys/time.h>

#include "Graph.h"
#include "Util.h"

using namespace std;

const int INPUTNUM=1;

int main(int argc, char* argv[]){
	ios::sync_with_stdio(false);
        //sanaz: removed anything regarding the window size (W). Not needed here 
	clock_t start, end;
        double avgTime = 0.0; 

	string filename;

	if(argc < 3){
		cout << "please provide parameter" << endl;
		exit(0);
	}
	
        filename=argv[1];
	int n = atoi(argv[2]);	
     
	for(int i=0; i<n; i++){
		srand(time(0));

		Graph g;
		string name;
		name=extractFilename(filename.c_str());
		g.setFilename(name);

		start=clock();
		g.readGraph(filename);
		cout << "Graph reading complete..." << endl;
		end=clock();
		cout << "Time Cost: " << (double)(end-start)/CLOCKS_PER_SEC << endl;

		start=clock();
		vector<int> order;
		g.RCMOrder(order);
		end=clock();
		cout << "ReOrdered Time Cost: " << (double)(end-start)/CLOCKS_PER_SEC << endl;

		avgTime += (double)(end-start)/CLOCKS_PER_SEC;


		//cout << "Begin Output the Reordered Graph" << endl;
		double overHead = g.PrintReOrderedGraph(order);
		cout << "Sorting Overhead: " << overHead << endl;
		avgTime += overHead;
		//cout << endl;	
	}
	avgTime /= n; 
	cout << "average time over " << n << " runs (including the sorting overhead): " << avgTime << endl;

}

