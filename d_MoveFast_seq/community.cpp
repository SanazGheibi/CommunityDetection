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
#include <stdio.h>

using namespace std;

Community::Community(char * filename, char * filename_w, int type, int nbp, double minm) {
  g = Graph(filename, filename_w, type);
  size = g.nb_nodes;

  neigh_weight.resize(size,-1);
  neigh_pos.resize(size);
  neigh_last=0;

  n2c.resize(size);
  in.resize(size);
  tot.resize(size);

  for (int i=0 ; i<size ; i++) {
    n2c[i] = i;
    tot[i] = g.weighted_degree(i);
    in[i]  = g.nb_selfloops(i);
  }

  nb_pass = nbp;
  min_modularity = minm;
}

Community::Community(Graph gc, int nbp, double minm) {
  g = gc;
  size = g.nb_nodes;

  neigh_weight.resize(size,-1);
  neigh_pos.resize(size);
  neigh_last=0;

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

// inline void
// Community::remove(int node, int comm, double dnodecomm) {
//   assert(node>=0 && node<size);

//   tot[comm] -= g.weighted_degree(node);
//   in[comm]  -= 2*dnodecomm + g.nb_selfloops(node);
//   n2c[node]  = -1;
// }

// inline void
// Community::insert(int node, int comm, double dnodecomm) {
//   assert(node>=0 && node<size);

//   tot[comm] += g.weighted_degree(node);
//   in[comm]  += 2*dnodecomm + g.nb_selfloops(node);
//   n2c[node]=comm;
// }

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

  int finaln=0;
  for (int i=0 ; i<size ; i++)
    if (renumber[i]!=-1)
      renumber[i]=finaln++;

  // Compute communities
  vector<vector<int> > comm_nodes(finaln);
  for (int node=0 ; node<size ; node++) {
    comm_nodes[renumber[n2c[node]]].push_back(node);
  }

  // Compute weighted graph
  Graph g2;
  g2.nb_nodes = comm_nodes.size();
  g2.degrees.resize(comm_nodes.size());
  g2.total_weight = 0.;

  int comm_deg = comm_nodes.size();

  for (int comm=0 ; comm<comm_deg ; comm++) { 
    map<int,float> m;
    map<int,float>::iterator it;
    

    int comm_size = comm_nodes[comm].size();
    for (int node=int(comm_size*0) ; node<comm_size ; node++) {
      pair<vector<unsigned int>::iterator, vector<float>::iterator> p = g.neighbors(comm_nodes[comm][node]);
      int deg = g.nb_neighbors(comm_nodes[comm][node]);
      for (int i=0 ; i<deg ; i++) {
	int neigh        = *(p.first+i);
	int neigh_comm   = renumber[n2c[neigh]];
	float neigh_weight = (g.weights.size()==0)?1.:*(p.second+i);

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


bool
Community::one_level() {
  bool improvement=false ;
  int nb_moves;
    
  //sanaz: 
  arrayQueue<int> Q(size);
  int* insider = new int[size];
  for(int i=0; i<size; i++)
     insider[i] = 1; //they are all inside the queue in the first pass

  for (int node_tmp=0 ; node_tmp<size ; node_tmp++){
    Q.push(node_tmp);
  }

  //sanaz: the structure of the loop changed to conform to the MoveNodesFast
  //algorithm in the extended version of the Leiden paper
  nb_moves = 0; 
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
                   unsigned int neigh = *(p.first+i);
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

  return improvement;
}

