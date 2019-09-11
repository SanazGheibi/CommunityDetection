
// File: community.h
// -- community detection source file
//-----------------------------------------------------------------------------
// Community detection
// Based on the article "Fast unfolding of community hierarchies in large networks"
// Copyright (C) 2008 V. Blondel, J.-L. Guillaume, R. Lambiotte, E. Lefebvre
//
// This program must not be distributed without agreement of the above mentionned authors.
//-----------------------------------------------------------------------------
// Author   : E. Lefebvre, adapted by J.-L. Guillaume
// Email    : jean-loup.guillaume@lip6.fr
// Location : Paris, France
// Time	    : February 2008
//-----------------------------------------------------------------------------
// see readme.txt for more details

#include "community.h"

using namespace std;

Community::Community(char * filename, char * filename_w, int type, int nbp, double minm) {
  //Sanaz: this is the constructor which is called initially (before ABFS_preodering); 
  //max_degree is therefore computed here
  g = Graph(filename, filename_w, type);
  size = g.nb_nodes; //size shows nodes upto what index are valid - all the nodes before prune
 
  neigh_weight.resize(size,-1);
  neigh_pos.resize(size);
  neigh_last=0;

  //sanaz:
  //miniBatch[i] = j means: miniBatch starting from node i, has	size = j
  miniBatch.resize(size, -1); //used to keep track of which nodes blong to which minBatch
  max_batchSize = 0; 
  max_degree = 0; //to be used for the bucketSort function
  total_size = size; 
  boundary = size; //boundary shows starting from which are the nodes pruned out - no nodes before prune

  //sanaz: the following varaibles will be further initialized inside the prune_graph() function
  //for now, we don't keep the degree information intact 
  n2c.resize(size, -1);
  in.resize(size, -1);
  tot.resize(size, -1);
  max_degree = 0; 

  nb_pass = nbp;
  min_modularity = minm;

}

Community::Community(Graph gc, int nbp, double minm) {
  //sanaz: this constructor is called in the later levels. Now, we don't need miniBatch[] vector
  g = gc;
  size = g.nb_nodes;

  //sanaz: 
  total_size = size;
  boundary = size; //boundary shows starting from which are the nodes pruned out - no nodes after level0
  
  neigh_weight.resize(size,-1);
  neigh_pos.resize(size);
  neigh_last=0;

  //sanaz:
  //miniBatch[i] = j means: miniBatch starting from node i, has size = j
  miniBatch.resize(size, -1); //used to	keep track of which nodes blong to which minBatch
  max_batchSize = 0;
  max_degree = 0; //to be used for the bucketSort function 

  n2c.resize(size);
  in.resize(size);
  tot.resize(size);

  for (int i=0 ; i<size ; i++) {
    n2c[i] = i;
    in[i]  = g.nb_selfloops(i);
    tot[i] = g.weighted_degree(i);
  }

  nb_pass = nbp;
  min_modularity = minm;
}
//---------------------------------------------------------------------------------------------------------
//sanaz: function added by me, used to prune the low degree neighbors of the high degree nodes
void
Community::prune_graph(int T){
  //first perform a re-numberig: otherwise the neighbor IDs will be wrong
  vector<int> renumber (size, 0);
  int final = 0; 
  //renumber the high-degree nodes
  for(int node = 0; node < size; node++){
      int deg = g.nb_neighbors(node);
      if(deg > T)
         renumber[node] = final++;
  }
  boundary = final;
  //the value of "index" at this point, defines
  if(boundary == 0){
     cerr << "No node remains after pruning!" << endl;
     exit (EXIT_FAILURE);
  }
  //renumber the low-degree nodes 
  for(int node = 0; node < size; node++){
      int deg = g.nb_neighbors(node);
      if(deg <=	T)
       	 renumber[node] = final++;
  }

  Graph g2;
  g2.nb_nodes = g.nb_nodes;
  g2.nb_links = g.nb_links;
  g2.total_weight = g.total_weight;
  g2.degrees.resize(g.nb_nodes); //g.nb_nodes = size 
  g2.links.resize(g.nb_links, -1); //some space will be wasted, but the will be fixed in transform()
  int link_index = 0; 
  
  g2.degrees[0] = 0; 
  //now the pruning starts:
  //the low degree nodes should be transferred to the end: two passes required
  int index = 0; 
  for(int node=0; node<size; node++){
      int deg = g.nb_neighbors(node);
      if(deg <= T)
         continue;
      if(index != 0)
         g2.degrees[index] = g2.degrees[index-1]; //initialization
      pair<vector<unsigned int>::iterator, vector<float>::iterator> p = g.neighbors(node);
      for (int i=0 ; i<deg ; i++) {
           int neigh = *(p.first+i);  
           double weight = (g.weights.size()==0)?1.:*(p.second+i);
           int neigh_deg = g.nb_neighbors(neigh);
           if(neigh_deg > T){
              neigh = renumber[neigh];
              g2.degrees[index]++;
              g2.links[link_index] = neigh;
	      link_index++;
              if(g.weights.size()!=0)
                 g2.weights.push_back(weight); 
           }else{
	      g2.nb_links--;
              g2.total_weight -= weight;
           }
      } 
      index++;    
  }

  //now the second pass;
  for(int node=0; node<size; node++){
      int deg = g.nb_neighbors(node);
      if(deg > T)
         continue;
      g2.degrees[index] = g2.degrees[index-1] + deg; 
      pair<vector<unsigned int>::iterator, vector<float>::iterator> p = g.neighbors(node);
      for (int i=0 ; i<deg ; i++) {
           int neigh = renumber[*(p.first+i)];
           g2.links[link_index] = neigh;
	   link_index++;
           if(g.weights.size()!=0)
              g2.weights.push_back(*(p.second+i)); 
      } 
      index++;    
  }  
  g = g2;

  int m_deg = 0;
  for (int i=0 ; i<size ; i++) {
    n2c[i] = i;
    tot[i] = g.weighted_degree(i);
    in[i]  = g.nb_selfloops(i);
    m_deg = g.nb_neighbors(i);
    if(m_deg > max_degree)
       max_degree = m_deg;
  }

  size = boundary;  
}
//---------------------------------------------------------------------------------------------------------
void
Community::init_partition(char * filename) {
  ifstream finput;
  finput.open(filename,fstream::in);

  // read partition
  while (!finput.eof()) {
    unsigned int node, comm;
    finput >> node >> comm;
    
    if (finput) {
      int old_comm = n2c[node];
      neigh_comm(node);

      remove(node, old_comm, neigh_weight[old_comm]);

      unsigned int i=0;
      for ( i=0 ; i<neigh_last ; i++) {
	unsigned int best_comm     = neigh_pos[i];
	float best_nblinks  = neigh_weight[neigh_pos[i]];
	if (best_comm==comm) {
	  insert(node, best_comm, best_nblinks);
	  break;
	}
      }
      if (i==neigh_last)
	insert(node, comm, 0);
    }
  }
  finput.close();
}

void
Community::display() {
  for (int i=0 ; i<size ; i++)
    cerr << " " << i << "/" << n2c[i] << "/" << in[i] << "/" << tot[i] ;
  cerr << endl;
}

double
Community::modularity() {
  double q  = 0.;
  double m2 = (double)g.total_weight;

  for (int i=0 ; i<size ; i++) {
    if (tot[i]>0)
      q += (double)in[i]/m2 - ((double)tot[i]/m2)*((double)tot[i]/m2);
  }

  return q;
}

void
Community::neigh_comm(unsigned int node) {
  for (unsigned int i=0 ; i<neigh_last ; i++)
    neigh_weight[neigh_pos[i]]=-1;
  neigh_last=0;

  pair<vector<unsigned int>::iterator, vector<float>::iterator> p = g.neighbors(node);

  unsigned int deg = g.nb_neighbors(node);

  neigh_pos[0]=n2c[node];
  neigh_weight[neigh_pos[0]]=0;
  neigh_last=1;

  for (unsigned int i=0 ; i<deg ; i++) {
    unsigned int neigh        = *(p.first+i);
    unsigned int neigh_comm   = n2c[neigh];
    double neigh_w = (g.weights.size()==0)?1.:*(p.second+i);
    
    if (neigh!=node) {
      if (neigh_weight[neigh_comm]==-1) {
	neigh_weight[neigh_comm]=0.;
	neigh_pos[neigh_last++]=neigh_comm;
      }
      neigh_weight[neigh_comm]+=neigh_w;
    }
  }
}
//-------------------------------------------------------------------------------------------------------------------------
unsigned int
Community::neigh_comm_low(unsigned int node) {
  for (unsigned int i=0 ; i<neigh_last ; i++)
    neigh_weight[neigh_pos[i]]=-1;
  neigh_last=0;

  //for keeping track of the community to which the input node has the most strong connection
  int max_inDeg = 0; 
  unsigned int ind = 0; 

  pair<vector<unsigned int>::iterator, vector<float>::iterator> p = g.neighbors(node);

  unsigned int deg = g.nb_neighbors(node);

  neigh_pos[0]=n2c[node];
  neigh_weight[neigh_pos[0]]=0;
  neigh_last=1;

  for (unsigned int i=0 ; i<deg ; i++) {
    unsigned int neigh        = *(p.first+i);
    unsigned int neigh_comm   = n2c[neigh];
    double neigh_w = (g.weights.size()==0)?1.:*(p.second+i);
    
    if (neigh!=node) {
      if (neigh_weight[neigh_comm]==-1) {
	neigh_weight[neigh_comm]=0.;
	neigh_pos[neigh_last++]=neigh_comm;
      }
      neigh_weight[neigh_comm]+=neigh_w;
      if(neigh_weight[neigh_comm] > max_inDeg){
         max_inDeg = neigh_weight[neigh_comm];
         ind = neigh_comm;
      }
    }
  }
  return ind;
}
//-------------------------------------------------------------------------------------------------------------------------
//sanaz: function added by me, to be able to keep track of the max miniBatch size at each level
//that is used to initialize the arrayQueue at the one_level funtion 
void
Community::miniBatch_assign(long W){
   int start = 0; 
   while(start < size){
   	long sum = 0;
   	int index = start;
   	int max_deg = 0; //neigh_weight[] and neigh_pos[] are used for a single node at each time
   	int diff = 0;
   	while(index < size && sum < W){
      		int deg = g.nb_neighbors(index);
      		diff = 0;
      		if(deg > max_deg){
        	    diff = deg - max_deg;
         	    max_deg = deg;
      		}
      		long tmp = sizeof(int) + sizeof(unsigned long) + deg * (sizeof(unsigned int) + sizeof(float)); //Accounting for n2c, g.degrees[], g.links[] and g.weights[]
      		if(sum + tmp + diff *(long)(sizeof(double)+sizeof(unsigned int)) >= W)
        	   break;
      		sum += tmp + diff * (sizeof(double)+sizeof(unsigned int));
      		index++;
   	}
   	if(index == size)
           index--;
        miniBatch[start] = (index-start+1);
        if(miniBatch[start] > max_batchSize)
           max_batchSize = miniBatch[start];
        start = index + 1; 
   }
}
//-------------------------------------------------------------------------------------------------------------------------
//sanaz: function added by me. A mapping function mapping each node group's start index to its end index
int
Community::size_estimate(int start, long W){
   //this version is just for test
   if(miniBatch[start] == -1){
      cerr << "inconsistency in start indices in miniBatch[]" << endl;
      exit (EXIT_FAILURE);
   }
   int res = start + miniBatch[start] -1;
   return res; 
}
//--------------------------------------------------------------------------------------------------------------------------
//sanaz: sort function added by me
void
Community::bucketSort(vector<int>::iterator first, vector<int>::iterator last, bool ascending){

  //initialize a vector of vectors for recording nodes with degrees ranging from 0 to max_degree
  vector<vector<int>> lst(max_degree+1); //indices: node degrees, value: a vector of node indices with that degree
  unsigned int i;
  unsigned int j;

  //phase1: iterate over the input vector and insert the elements in the proper list of the map
  vector<int>::iterator it;
  for(it = first; it < last; it++){
      int index = *it;
      int deg = g.nb_neighbors(index);
      lst[deg].push_back(index);
  }

  //iterate over the map in the order of the keys (it is an ordered map), overwrite them in the original vector
  it = first;
  if(ascending){
     for(i=0; i<lst.size(); i++)
         for(j=0; j<lst[i].size(); j++){
             *it = lst[i][j];
             it++;
         }
  }else{
     for(unsigned int ii=0; ii<lst.size(); ii++){
         i = lst.size()-1-ii;
         for(j=0; j<lst[i].size(); j++){
             *it = lst[i][j];
             it++;
         }
      }
  }

}
//--------------------------------------------------------------------------------------------------------------------------
//sanaz: function added by me. ABFS: Adaptive BFS. Unlike cm, here nodes are sorted in decreasing order of their degrees
void
Community::ABFS_preorder(vector<int>& retorder, long W){
   //initial order: the order in which nodes appear in the input file
   //initOrder[k]: what node is the k'th node in this initial ordering
   vector<int> initOrder(size);
   for(int i=0; i<size; i++){
	initOrder[i]=i;
   }

   bucketSort(initOrder.begin(), initOrder.end(), false);

   //now the Adaptive BFS starts
   //at this stage, only consider nodes with degrees > 1 
   int* done = new int[size]; //keeps track of nodes entered into cache (fully processed)
   int* enqued = new int[size]; //keeps track of nodes currently in the queue
   arrayQueue<int> BFS(W/sizeof(int)); //changed the type from queue to vector, so that the elements can be sorted in place 
   for(int i=0; i<size; i++)
       done[i] = 0;
   for(int i=0; i<size; i++)
       enqued[i] = 0;

   int start = 0;
   int num = 0; //index is inclusive
   int k=0; 
   bool middleStart = false; //indicates if a new node has been pushed into the empty queue in the middle of a mini-batch
   int batchID = 0;

   while(k<size){ 
      batchID++;
      //this outer loop is processed only when the queue is empty
      if(done[initOrder[k]] == 1){
         k++;
         continue;
      }
      
      BFS.push(initOrder[k]); //this will be the start point for the current mini-batch
      start = initOrder[k];
      enqued[initOrder[k]] = batchID;

      k++; //the first node in a mini-batch will definitely be processed 
      num = 0; 
      long sum = 0; 
      int max_deg = 0; //neigh_weight[] and neigh_pos[] are used for a single node at each time
      int diff = 0; 

      while(sum < W){

           if(BFS.empty()){
              while(k<size && done[initOrder[k]]==1)
                    k++;
              if(k == size)
                 break;
              BFS.push(initOrder[k]);
              enqued[initOrder[k]] = batchID;
              k++;
              middleStart = true; //current node pushed into an empty queue at the middle of a mini-batch
           }

           int curr = BFS.front();
 
           //if it is negative: its abs shows the number of childern that follow (and have to be sorted)
           if(curr < 0){
             BFS.pop(); //similar to pop() in case of a queue
             curr = -curr;
             //sorting the "curr" following elements in decreasing order of their degrees
             /*sort(BFS.begin(), BFS.begin()+curr, [&](const int& a, const int& b)->bool{
                    if(g.nb_neighbors(a)>g.nb_neighbors(b))
                        return true;
                    else
                        return false;
             });*/

             curr = BFS.front();
           }           
           int deg = g.nb_neighbors(curr);
           diff = 0; 
	   if(deg > max_deg){
	      diff = deg - max_deg;
	      max_deg = deg; 
	   }
	   long tmp = sizeof(int) + sizeof(unsigned long) + deg * (sizeof(unsigned int) + sizeof(float)); //Accounting for n2c, g.degrees[], g.links[] and g.weights[]
	   if(sum + tmp + diff *(long)(sizeof(double)+sizeof(unsigned int)) >= W){
                //curr dosn't fit
                if(num == 0){
                   done[curr] = 1; //a singleton: should be included in this mini-batch
                   retorder.push_back(curr);
                   num++;
                }
                if(middleStart)
                   k--;
		BFS.flush();
		break;
           }
	   sum += tmp + diff * (sizeof(double)+sizeof(unsigned int));
           middleStart = false;
           done[curr] = 1;
           retorder.push_back(curr); //define the order only after the node has been used
           num++; 

           //insert the neighbors and remove curr from the queue
           pair<vector<unsigned int>::iterator, vector<float>::iterator> p = g.neighbors(curr);
           vector<int> children;
          
           int ch_num = 0; 
           for (int i=0 ; i<deg ; i++) {
                int neigh = *(p.first+i); 
                if(enqued[neigh] != batchID && done[neigh] == 0){
                   ch_num++;
                   children.push_back(neigh);
                }
           }

           if(ch_num > 0)
              BFS.push(-ch_num);

           for (int i=0 ; i < ch_num ; i++) {
                int neigh = children[i];
                enqued[neigh] = batchID;
                BFS.push(neigh);
           }  
           BFS.pop(); 
      }
      //set num
      miniBatch[start] = num; 
      if(num > max_batchSize)
         max_batchSize = num;
   }

   return;
}
//..........................................................................................................................................................................
//sanaz: function added by me. Given the new order, transforms the graph accordig to the order 
void
Community::transform(long W){
  //maps new indices to old indices
  vector<int> order;
  ABFS_preorder(order, W);

  if(order.size() != (unsigned int) size){
     cerr << "order.size() != graph size" << endl;
     exit (EXIT_FAILURE);
  }

  for(int i=boundary; i<total_size; i++)
      order.push_back(i);

  //A reverse map: maps old indices to new indices 
  vector<int> reverse_map(total_size, -1);
  for(int i=0; i<total_size; i++)
     reverse_map[order[i]] = i;
  
  //construct the new graph
  // Compute the reordered graph
  Graph g2;
  g2.nb_nodes = g.nb_nodes;
  g2.nb_links = g.nb_links;
  g2.total_weight = g.total_weight;
  g2.degrees.resize(g.nb_nodes); //g.nb_nodes = size 
  g2.links.resize(g.nb_links); //number of links should not change in ABFS preordering

  //g2.weights.resize(g.weights.size()); --> the cause of a bug
  int link_index = 0;
  for(int index=0; index<total_size; index++){
      int node = order[index];
      int deg = g.nb_neighbors(node);
      g2.degrees[index] = (index==0)?deg:g2.degrees[index-1]+deg;
      pair<vector<unsigned int>::iterator, vector<float>::iterator> p = g.neighbors(node);
      for(int i=0; i<deg; i++){
	  int neigh = *(p.first+i);
	  g2.links[link_index] = reverse_map[neigh];
	  link_index++;
          if(g.weights.size()!=0)
             g2.weights.push_back(*(p.second+i)); 
      }      
  }  

  //transform in-place
  g = g2;

  //modify the Community class data structures 
  //no need to modify data for size < i < total_size: the indices don't change after pruning 
  vector<int> batch2(size, -1);
  vector<double> tot_tmp(size);
  vector<double> in_tmp(size);

  for(int i=0; i<size; i++){
     //traverses over the old indices 
     batch2[reverse_map[i]] = miniBatch[i];
     tot_tmp[reverse_map[i]] = tot[i];
     in_tmp[reverse_map[i]] = in[i];
  }
  miniBatch = batch2;

  tot = tot_tmp;
  in = in_tmp; 
 
  //to solve the problem of node - community ID mismatch 
  for(int i=0; i<size; i++){
     n2c[i] = i;
  }

}

//.............................................................................................................................
void
Community::partition2graph() {
  vector<int> renumber(size, -1);
  for (int node=0 ; node<size ; node++) {
    renumber[n2c[node]]++;
  }

  int final=0;
  for (int i=0 ; i<size ; i++)
    if (renumber[i]!=-1)
      renumber[i]=final++;


  for (int i=0 ; i<size ; i++) {
    pair<vector<unsigned int>::iterator, vector<float>::iterator> p = g.neighbors(i);

    int deg = g.nb_neighbors(i);
    for (int j=0 ; j<deg ; j++) {
      int neigh = *(p.first+j);
      cout << renumber[n2c[i]] << " " << renumber[n2c[neigh]] << endl;
    }
  }
}

void
Community::display_partition() {
  vector<int> renumber(size, -1);
  for (int node=0 ; node<size ; node++) {
    renumber[n2c[node]]++;
  }

  int final=0;
  for (int i=0 ; i<size ; i++)
    if (renumber[i]!=-1)
      renumber[i]=final++;

  for (int i=0 ; i<size ; i++)
    cout << i << " " << renumber[n2c[i]] << endl;
}


Graph
Community::partition2graph_binary() {
  // Renumber communities
  vector<int> renumber(size, -1);
  for (int node=0 ; node<size ; node++) {
    renumber[n2c[node]]++;
  }

  int final=0;
  for (int i=0 ; i<size ; i++)
    if (renumber[i]!=-1)
      renumber[i]=final++;

  // Compute communities
  vector<vector<int> > comm_nodes(final);
  for (int node=0 ; node<size ; node++) {
    comm_nodes[renumber[n2c[node]]].push_back(node);
  }

  // Compute weighted graph
  Graph g2;
  g2.nb_nodes = comm_nodes.size();
  g2.degrees.resize(comm_nodes.size());

  int comm_deg = comm_nodes.size();
  for (int comm=0 ; comm<comm_deg ; comm++) {
    map<int,float> m;
    map<int,float>::iterator it;

    int comm_size = comm_nodes[comm].size();
    for (int node=0 ; node<comm_size ; node++) {
      pair<vector<unsigned int>::iterator, vector<float>::iterator> p = g.neighbors(comm_nodes[comm][node]);
      int deg = g.nb_neighbors(comm_nodes[comm][node]);
      for (int i=0 ; i<deg ; i++) {
	int neigh        = *(p.first+i);
	int neigh_comm   = renumber[n2c[neigh]];
	double neigh_weight = (g.weights.size()==0)?1.:*(p.second+i);
 
	it = m.find(neigh_comm);
	if (it==m.end())
	  m.insert(make_pair(neigh_comm, neigh_weight));
	else
	  it->second+=neigh_weight;
      }
    }
    g2.degrees[comm]=(comm==0)?m.size():g2.degrees[comm-1]+m.size();
    g2.nb_links+=m.size();

    for (it = m.begin() ; it!=m.end() ; it++) {
      g2.total_weight  += it->second;
      g2.links.push_back(it->first);
      g2.weights.push_back(it->second);
    }
  }

  return g2;
}
//---------------------------------------------------------------------------------
//sanaz: function added by me. To deal with the assymetry inherent in level0 graph
//(one way links from low degree nodes to high degree nodes)
Graph
Community::partition2graph_binary_l0() {
  size = total_size;
  // Renumber communities
  vector<int> renumber(size, -1);
  for (int node=0 ; node<size ; node++) {
    renumber[n2c[node]]++;
  }

  int final=0;
  for (int i=0 ; i<size ; i++)
    if (renumber[i]!=-1)
      renumber[i]=final++;

  //sanaz: fix the one-way links between low degree and high degree nodes
  vector<vector<pair<int, double> > > comm_lowNeighs(final); //keeping track of low degree neighbors of communities 
  for(int i=boundary; i<size; i++){
      int deg = g.nb_neighbors(i);
      pair<vector<unsigned int>::iterator, vector<float>::iterator> p = g.neighbors(i);
      for(int j=0; j<deg; j++){
          int neigh = *(p.first+j);
          double neigh_weight = (g.weights.size()==0)?1.:*(p.second+j);
          if(neigh < boundary)
             comm_lowNeighs[renumber[n2c[neigh]]].push_back(make_pair(renumber[n2c[i]],neigh_weight));
      }
  }

  // Compute communities
  vector<vector<int> > comm_nodes(final);
  for (int node=0 ; node<size ; node++) {
    comm_nodes[renumber[n2c[node]]].push_back(node);
  }

  // Compute weighted graph
  Graph g2;
  g2.nb_nodes = comm_nodes.size();
  g2.degrees.resize(comm_nodes.size());

  int comm_deg = comm_nodes.size();
  for (int comm=0 ; comm<comm_deg ; comm++) {

    map<int,float> m;
    map<int,float>::iterator it;

    int comm_size = comm_nodes[comm].size();
    for (int node=0 ; node<comm_size ; node++) {
      pair<vector<unsigned int>::iterator, vector<float>::iterator> p = g.neighbors(comm_nodes[comm][node]);
      int deg = g.nb_neighbors(comm_nodes[comm][node]);
      for (int i=0 ; i<deg ; i++) {
	int neigh        = *(p.first+i);
	int neigh_comm   = renumber[n2c[neigh]];
	double neigh_weight = (g.weights.size()==0)?1.:*(p.second+i);
        it = m.find(neigh_comm);
	if (it==m.end())
          m.insert(make_pair(neigh_comm, neigh_weight));
        else
          it->second+=neigh_weight;
      }
    }

    //sanaz: here, we should account for the links to low degree nodes
    for(unsigned int i=0; i< comm_lowNeighs[comm].size(); i++){
        int neigh_comm = comm_lowNeighs[comm][i].first;
        double neigh_weight = comm_lowNeighs[comm][i].second;
        it = m.find(neigh_comm);
	if (it==m.end())
          m.insert(make_pair(neigh_comm, neigh_weight));
	else
          it->second+=neigh_weight;
    }  

    g2.degrees[comm]=(comm==0)?m.size():g2.degrees[comm-1]+m.size();
    g2.nb_links+=m.size();

    for (it = m.begin() ; it!=m.end() ; it++) {
      g2.total_weight  += it->second;
      g2.links.push_back(it->first);
      g2.weights.push_back(it->second);
    }
  }

  return g2;
}
//---------------------------------------------------------------------------------
//sanaz: W is the window size (size of LLC/fast memory in Bytes)
bool
Community::one_level(int W, bool level0) {
  bool improvement=false ;
  int nb_moves;

  //for level0, we can keep track of the max batch_size inside the ABFS_preorder()
  if(!level0)
     miniBatch_assign(W);

  //sanaz: experiment with the full sweep at the beginning:
  improvement = singleFullSweep();

  //sanaz: flag used for experiment with the full sweep in the middle:
  //int middleReached = 0; 

  //sanaz: for keepint track of the range of the node indices used in each round 
  int start = 0; 
  int end = -1; 

  // repeat while 
  //   there is an improvement of modularity
  //   or there is an improvement of modularity greater than a given epsilon 
  //   or a predefined number of pass have been done

  //sanaz:the do-while loop is added by me 
  arrayQueue<int> Q(max_batchSize);
  int* insider = new int[size];
  do{
	  start = end+1; 
	  end = size_estimate(start, W);
	  for(int j=start; j<=end; j++){
 	       Q.push(j);
 	       insider[j] = 1;
	  }
	  nb_moves = 0;

          //experiment with doing the full sweep in the middle:
          /*if(start >= size/2 && middleReached == 0){
             middleReached = 1;
             singleFullSweep();
          }*/          

          do {
                      int node = Q.front();
                      int node_comm = n2c[node];
		      double w_degree = g.weighted_degree(node);

		      // computation of all neighboring communities of current node
		      neigh_comm(node);
		      // remove node from its current community
		      remove(node, node_comm, neigh_weight[node_comm]);

		      // compute the nearest community for node
		      // default choice for future insertion is the former community
		      int best_comm        = node_comm;
		      double best_nblinks  = 0.;
		      double best_increase = 0.;
		      for (unsigned int i=0 ; i<neigh_last ; i++) {
			double increase = modularity_gain(node, neigh_pos[i], neigh_weight[neigh_pos[i]], w_degree);                 
      			if (increase>best_increase) {
			  best_comm     = neigh_pos[i];
			  best_nblinks  = neigh_weight[neigh_pos[i]];
			  best_increase = increase;     
			}
		      }

		      // insert node in the nearest community
		      insert(node, best_comm, best_nblinks);
                                         		                    
		      Q.pop();
                      insider[node] = 0; 	     
		      if(best_comm!=node_comm){
		        //add neighbors to the queue, whose community != best_community
		        //assumption: the neighbor oder here is the same as what appears in neigh_pos[] array
		        pair<vector<unsigned int>::iterator, vector<float>::iterator> p = g.neighbors(node);
		        unsigned int deg = g.nb_neighbors(node);
		        for(unsigned int i=0; i<deg ; i++){
		           int neigh = *(p.first+i);
                           if(neigh < start || neigh > end)
                              continue;
		           int n_comm = n2c[neigh];
		           if(n_comm != best_comm && insider[neigh] == 0){
                             Q.push(neigh);
                             insider[neigh] = 1;
		           }
		        }
			nb_moves++;
		      }

	  } while (!Q.empty());
          
          if (nb_moves>0)
	     improvement=true;
     
  } while(end < size-1);

  //sanaz: to merge low degree nodes with the rest of the graph
  if(level0)
     merge_low();

  return improvement;
}

//----------------------------------------------------------------------------------------------
//sanaz: added by me. The single global sweep. It was previously placed at the end of one_level()
bool
Community::singleFullSweep() {
 //This is equivalent to the first sweep which is the same in both MoveFast
 //and original Louvain
 int nb_moves = 0;  
 for (int node=0; node<size; node++) {
       int node_comm     = n2c[node];
       double w_degree = g.weighted_degree(node);

       // computation of all neighboring communities of current node
       neigh_comm(node);
       // remove node from its current community
       remove(node, node_comm, neigh_weight[node_comm]);

       // compute the nearest community for node
       // default choice for future insertion is the former community
       int best_comm        = node_comm;
       double best_nblinks  = 0.;
       double best_increase = 0.;
       for (unsigned int i=0 ; i<neigh_last ; i++) {
           double increase = modularity_gain(node, neigh_pos[i], neigh_weight[neigh_pos[i]], w_degree);
           if (increase>best_increase) {
               best_comm     = neigh_pos[i];
               best_nblinks  = neigh_weight[neigh_pos[i]];
               best_increase = increase;
           }
       }

       // insert node in the nearest community
       insert(node, best_comm, best_nblinks);

       if (best_comm!=node_comm)
           nb_moves++;
  }
 
  if(nb_moves > 0)
    return true;
  
  return false;
}
//----------------------------------------------------------------------
//sanaz: nodes of degrees <= threshold are ignored at mini batch assignment
//and at level 0. They are merged with other communities at the end of level0
void
Community::merge_low(){
 //boundary is only valid after it is set in the original costructor
 for (int node=boundary; node<total_size; node++) {
       int deg = g.nb_neighbors(node);
       if(deg == 0)
          continue;
       
       int node_comm     = n2c[node];
       // computation of all neighboring communities of current node
       int best_comm = neigh_comm_low(node);   
       double best_nblinks = neigh_weight[best_comm];

       if(best_comm != node_comm){
          remove(node, node_comm, neigh_weight[node_comm]);
          insert(node, best_comm, best_nblinks);
       }
  }
}
//----------------------------------------------------------------------
