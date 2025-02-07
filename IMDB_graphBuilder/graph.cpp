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

Graph::Graph(char *filename, int type) {
  ifstream finput;
  finput.open(filename,fstream::in);

  if(!finput){
     cerr << "could not open the input file" << endl;
     exit(EXIT_FAILURE);
  }

  int nb_links=0;
  vector<int> tmp;
  string line;

  while (getline(finput, line) && !finput.eof()) {
    int node;
    unsigned int src, dest;
    double weight = 1.;

    stringstream stream(line);
    while(stream >> node)
	   tmp.push_back(node);
    //ignore tmp[0] --> movie ID
   for(unsigned int i=1; i<tmp.size(); i++){
        for(unsigned int j=i+1; j<tmp.size(); j++){
	    if(tmp[j] > tmp[i]){
               src = tmp[i]; 
               dest = tmp[j]; 
            }else{
               src = tmp[j]; 
               dest = tmp[i];
            }
            if(links.size() <= dest+1)
               links.resize(dest+1);
            links[src].push_back(make_pair(dest, weight));
            nb_links++;
        }
    }
    tmp.clear();
  }

  finput.close();
}

void
Graph::renumber(int type) {
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
Graph::clean(int type) {
    for (unsigned int i=0 ; i<links.size() ; i++) {
	    map<int, float> m;
	    map<int, float>::iterator it;

	    for (unsigned int j=0 ; j<links[i].size() ; j++) {
	      it = m.find(links[i][j].first);
	      if (it==m.end())
		m.insert(make_pair(links[i][j].first, links[i][j].second));
	      else if (type==WEIGHTED)
	      	it->second+=links[i][j].second;
	    }
	    
	    vector<pair<int,float> > v;
	    for (it = m.begin() ; it!=m.end() ; it++)
	      v.push_back(*it);
	    links[i].clear();
	    links[i]=v;
   }
}

void
Graph::display_ascii(char *filename, char *filename_w, int type) {
  ofstream foutput;
  foutput.open(filename);
  for(unsigned int i=0; i<links.size(); i++)
      for(unsigned int j=0; j<links[i].size(); j++)
          foutput << i << "\t" << links[i][j].first << endl;
  
  foutput.close();

  // outputs weights in a separate file
  if (type==WEIGHTED) {
    ofstream foutput_w;
    foutput_w.open(filename_w);

  for(unsigned int i=0; i<links.size(); i++)
      for(unsigned int j=0; j<links[i].size(); j++)
          foutput << links[i][j].second << endl;

    foutput_w.close();
  }
}

