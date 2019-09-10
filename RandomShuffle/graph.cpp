// File: graph.cpp
// -- simple graph handling source file
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

#include "graph.h"

using namespace std;

Graph::Graph(char *filename) {
  ifstream finput;
  finput.open(filename,fstream::in);

  int nb_links=0;

  while (!finput.eof()) {
    unsigned int src, dest;
    double weight=1.;
    
    if (finput) {
      if (links.size()<=max(src,dest)+1) {
        links.resize(max(src,dest)+1);
      }
      
      links[src].push_back(make_pair(dest,weight));
      if (src!=dest)
        links[dest].push_back(make_pair(src,weight));

      nb_links++;
    }
  }

  finput.close();
}

void
Graph::renumber() {
  vector<int> linked(links.size(),-1);
  vector<int> renum(links.size(),-1);
  int nb=0;
  
  for (unsigned int i=0 ; i<links.size() ; i++) {
    for (unsigned int j=0 ; j<links[i].size() ; j++) {
      linked[i]=1;
      linked[links[i][j].first]=1;
    }
  }
  
  for (unsigned int i=0 ; i<links.size() ; i++) {
    if (linked[i]==1)
      renum[i]=nb++;
  }

  for (unsigned int i=0 ; i<links.size() ; i++) {
    if (linked[i]==1) {
      for (unsigned int j=0 ; j<links[i].size() ; j++) {
	links[i][j].first = renum[links[i][j].first];
      }
      links[renum[i]]=links[i];
    }
  }
  links.resize(nb);
}

void
Graph::clean() {
  for (unsigned int i=0 ; i<links.size() ; i++) {
    map<int, float> m;
    map<int, float>::iterator it;

    for (unsigned int j=0 ; j<links[i].size() ; j++) {
      it = m.find(links[i][j].first);
      if (it==m.end())
	m.insert(make_pair(links[i][j].first, links[i][j].second));
    }
    
    vector<pair<int,float> > v;
    for (it = m.begin() ; it!=m.end() ; it++)
      v.push_back(*it);
    links[i].clear();
    links[i]=v;
  }
}

//sanaz: replaced display_binary with display_ascii
void
Graph::display_ascii(char *filename, char *filename_w, vector<int>& order) {
  ofstream foutput;
  foutput.open(filename);
  for(unsigned int i=0; i<links.size(); i++)
      for(unsigned int j=0; j<links[i].size(); j++)
          foutput << order[i] << "\t" << order[links[i][j].first] << endl;
  
  foutput.close();
}

//sanaz: Random Shuffle function, added by me
void Graph::RandomShuffle(vector<int>& retorder){
  int vsize = links.size();
  retorder.resize(vsize);

  for (int i=0 ; i<vsize ; i++)
    retorder[i]=i;
  for (int i=0 ; i<vsize-1 ; i++) {
    int rand_pos = rand()%(vsize-i)+i;
    int tmp      = retorder[i];
    retorder[i] = retorder[rand_pos];
    retorder[rand_pos] = tmp;
  }
}
