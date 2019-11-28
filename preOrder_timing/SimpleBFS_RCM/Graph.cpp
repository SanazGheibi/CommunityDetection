/*
MIT License

Copyright (c) 2016, Hao Wei.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Graph.h"

string Graph::getFilename(){
	return name;
}

void Graph::setFilename(string name){
	this->name.swap(name);
}

Graph::Graph() {
	edgenum=vsize=0;
}

Graph::~Graph() {
}

void Graph::clear() {
	vsize = 0;
	edgenum=0;
	name.clear();
	graph.clear();
	outedge.clear();
	inedge.clear();
}


void Graph::readGraph(const string& fullname) {
	FILE* fp;
	fp=fopen(fullname.c_str(), "r");
	if(fp==NULL){
		cout << "Fail to open " << fullname << endl;
		quit();
	}

	char line[40];
	int u, v;
	const char* str=NULL;
	
	vsize=0;
	edgenum=0;
	vector< pair<int, int> > edges;
	edges.reserve(100000000);

	while(feof(fp)!=true){
		if(fgets(line, 40, fp)){
			u=v=0;
			str=line;
			while(isdigit(*str))
				u=u*10+(*str++ - '0');
			str++;
			while(isdigit(*str))
				v=v*10+(*str++ - '0');

			if(u==v)
				continue;
			edgenum++;
			if(u>vsize)
				vsize=u;
			if(v>vsize)
				vsize=v;

			edges.push_back(make_pair(u, v));
		}
	}
	vsize++;

	fclose(fp);
	graph.resize(vsize+1);
	for(long long i=0; i<edges.size(); i++){
		graph[edges[i].first].outdegree++;
		graph[edges[i].second].indegree++;
	}
	graph[0].outstart=0;
	graph[0].instart=0;
	for(int i=1; i<vsize; i++){
		graph[i].outstart=graph[i-1].outstart+graph[i-1].outdegree;
		graph[i].instart=graph[i-1].instart+graph[i-1].indegree;
	}
	
	sort(edges.begin(), edges.end(), [](const pair<int, int>& a, const pair<int, int>& b)->bool{
		if(a.first<b.first)
			return true;
		else if(a.first>b.first)
			return false;
		else{

			if(a.second<=b.second)
				return true;
			else
				return false;
		}

	});
	outedge.resize(edgenum);
	for(long long i=0; i<edges.size(); i++){
		outedge[i]=edges[i].second;
	}

	vector< pair<int, int> >().swap(edges);
#ifndef Release	
	vector<int> inpos(vsize);
	for(int i=0; i<vsize; i++){
		inpos[i]=graph[i].instart;
	}
	inedge.resize(edgenum);
	for(int u=0; u<vsize; u++){
		for(int j=graph[u].outstart; j<graph[u].outstart+graph[u].outdegree; j++){
			inedge[inpos[outedge[j]]]=u;
			inpos[outedge[j]]++;
		}
	}
#endif

	cout << "vsize: " << vsize << endl;
	cout << "edgenum: " << edgenum << endl;
	graph[vsize].outstart=edgenum;
	graph[vsize].instart=edgenum;
}

//modified by sanaz: doesn't print anything. Returns sorting overhead time
double Graph::PrintReOrderedGraph(const vector<int>& order){
	//ofstream out((name+"_BFSrcm.txt").c_str());

	vector<int>().swap(inedge);
        
        clock_t startO, endO;
        startO=clock();

	vector< vector<int> > ReOrderedGraph(vsize);
	int u, v;
	for(int i=0; i<vsize; i++){
		u=order[i];
		ReOrderedGraph[u].reserve(graph[i+1].outstart-graph[i].outstart);
		for(int j=graph[i].outstart; j<graph[i].outstart+graph[i].outdegree; j++){
			v=order[outedge[j]];
			ReOrderedGraph[u].push_back(v);
		}
		sort(ReOrderedGraph[u].begin(), ReOrderedGraph[u].end());
	}
	endO=clock();
	//cout << "Sorting Overhead: " << (double)(endO-startO)/CLOCKS_PER_SEC << endl;
	return (double)(endO-startO)/CLOCKS_PER_SEC;

	/*for(int u=0; u<vsize; u++){
		for(int j=0; j<ReOrderedGraph[u].size(); j++){
			out << u << '\t' << ReOrderedGraph[u][j] << endl;
		}
	}
	out.close();*/
}

void Graph::RCMOrder(vector<int>& retorder){
	queue<int> que;
	bool* BFSflag=new bool[vsize];
	bool* QueFlag=new bool[vsize];
	memset(BFSflag, 0, sizeof(bool)*vsize);
	memset(QueFlag, 0, sizeof(bool)*vsize);

	vector<int> tmp;
	vector<int> degreevertex(vsize);
	for(int i=0; i<vsize; i++){
		degreevertex[i]=i;
	}

	sort(degreevertex.begin(), degreevertex.end(), [&](const int& a, const int& b)->bool{
		if(graph[a].outdegree+graph[a].indegree<graph[b].outdegree+graph[b].indegree)
			return true;
		else
			return false;
	});

        int now;
	vector<int> order;

	for(int k=0; k<vsize; k++){
		int i=degreevertex[k];
		if(BFSflag[i]==false){
			que.push(i);
//			QueFlag[i]=true;
			BFSflag[i]=true;
			order.push_back(i);

			while(que.empty()==false){
				now=que.front();
				que.pop();

//				BFSflag[now]=true;
				tmp.clear();
				for(int it=graph[now].outstart, limit=graph[now+1].outstart; it<limit; it++){
					tmp.push_back(outedge[it]);
				}
				sort(tmp.begin(), tmp.end(), [&](const int& a, const int& b)->bool{
					if(graph[a].outdegree+graph[a].indegree<graph[b].outdegree+graph[b].indegree)
						return true;
					else
						return false;
				});
				if(tmp.size()!=graph[now].outdegree)
					cout << "tmp.size()!=graph[now].outdegree" << endl;

				for(int i=0; i<tmp.size(); i++){
//					if((BFSflag[tmp[i]]==false)&&(QueFlag[tmp[i]]==false)){
					if(BFSflag[tmp[i]]==false){
						que.push(tmp[i]);
                        			BFSflag[tmp[i]]=true;
						order.push_back(tmp[i]);
                        		}
				}
        		}
		}
	}

        delete[] BFSflag;
        delete[] QueFlag;

	if(order.size()!=vsize){
		cout << "order.size()!=vsize" << endl;
		quit();
	}

	retorder.resize(vsize);
	for(int i=0; i<order.size(); i++){
		retorder[order[i]]=order.size()-1-i;
	}
}


