// File: main_community.cpp
// -- community detection, sample main file
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

#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream> 
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <sys/time.h>

#include "graph_binary.h"
#include "community.h"

//#define W  1048576
//#define W 4194304
//#define W 200

using namespace std;

char *filename = NULL;
char *filename_w = NULL;
char *filename_part = NULL;
int type       = UNWEIGHTED;
int nb_pass    = 0;
double precision = 0.000001;
int display_level = -2;
int k1 = 16;
//sanaz:
char *reorder_file = NULL;
double t_imp = 0.001;
int TK = 0;

bool verbose = false;

//sanaz: cache size variable added
long W = 1048576;
int coef = 1; //multiple of W (size of cache in MB)

void
usage(char *prog_name, const char *more) {
  cerr << more;
  cerr << "usage: " << prog_name << " input_file [-w weight_file] [-p part_file] [-q epsilon] [-l display_level] [-v] [-h]" << endl << endl;
  cerr << "input_file: file containing the graph to decompose in communities." << endl;
  cerr << "-w file\tread the graph as a weighted one (weights are set to 1 otherwise)." << endl;
  cerr << "-p file\tstart the computation with a given partition instead of the trivial partition." << endl;
  cerr << "\tfile must contain lines \"node community\"." << endl;
  cerr << "-q eps\ta given pass stops when the modularity is increased by less than epsilon." << endl;
  cerr << "-l k\tdisplays the graph of level k rather than the hierachical structure." << endl;
  cerr << "\tif k=-1 then displays the hierarchical structure rather than the graph at a given level." << endl;
  cerr << "-v\tverbose mode: gives computation time, information about the hierarchy and modularity." << endl;
  cerr << "-h\tshow this usage message." << endl;
  exit(0);
}

void
parse_args(int argc, char **argv) {
  if (argc<2)
    usage(argv[0], "Bad arguments number\n");

  for (int i = 1; i < argc; i++) {
    if(argv[i][0] == '-') {
      switch(argv[i][1]) {
      case 'w':
	type = WEIGHTED;
        filename_w = argv[i+1];
	i++;
	break;
      case 'p':
        filename_part = argv[i+1];
	i++;
	break;
      case 'q':
	precision = atof(argv[i+1]);
        t_imp = precision;
	i++;
	break;
      case 'l':
	display_level = atoi(argv[i+1]);
	i++;
	break;
      case 'k':
	k1 = atoi(argv[i+1]);
	i++;
	break;
      case 'v':
	verbose=true;
	break;
      case 'c': //added by sanaz
	coef = atoi(argv[i+1]);
	i++;
	break;
      case 't': //added by sanaz
        TK = atoi(argv[i+1]);
        i++;
        break;
      //sanaz: the re-ordered graph should be written out and be used in next iterations
      case 'r':
	reorder_file = argv[i+1];
	i++;
      break;
      default:
	usage(argv[0], "Unknown option\n");
      }
    } else {
      if (filename==NULL)
        filename = argv[i];
      else
        usage(argv[0], "More than one filename\n");
    }
  }
}

void
display_time(const char *str) {
  time_t rawtime;
  time ( &rawtime );
  cerr << str << ": " << ctime (&rawtime);
}


int
main(int argc, char **argv) {
  srand(time(NULL)+getpid());

  parse_args(argc, argv);
  W = coef * W; 
  clock_t time_begin1, time_end1, time_begin2, time_end2;
  time_begin1 = clock();
  if (verbose)
    display_time("Begin");

  Community c(filename, filename_w, type, -1, precision);
  if (filename_part!=NULL)
    c.init_partition(filename_part);

  time_end1 = clock();

  //sanaz: run ABFS preodering only in the begining of an iterative approach
  if (filename_part == NULL){
	  clock_t preOrder_begin, preOrder_end;
	  preOrder_begin=clock();
	  c.transform(W);
	  preOrder_end=clock();
	  cerr << "pre_ordering time: " << (double)(preOrder_end-preOrder_begin)/CLOCKS_PER_SEC << " sec." << endl;
          //sanaz: in order for the iterative method to work correctly, we should produced a new .bin file based 
	  //on the transformed graph
	  if(reorder_file != NULL)
	  	c.g.display_binary(reorder_file);
  }


  time_begin2 = clock();
  Graph g;
  bool improvement=true;
  double mod=c.modularity(), new_mod;
  double init_mod = mod;
  int level=0;

  do {
    if (verbose) {
      cerr << "level " << level << ":\n";
      display_time("  start computation");
      cerr << "  network size: " 
	   << c.g.nb_nodes << " nodes, " 
	   << c.g.nb_links << " links, "
	   << c.g.total_weight << " weight." << endl;
    }
    //sanaz: in iterations with initial community assignments, "transform()" is not called
    //therefore, "miniBatch_assign()" should be called anyway
    bool level0 = (level == 0);
    if(filename_part!=NULL)
	level0 = false;
    improvement = c.one_level(W, level0, t_imp, TK);
    new_mod = c.modularity();
    if (++level==display_level)
      g.display();
    if (display_level==-1)
      c.display_partition();
    g = c.partition2graph_binary();
    c = Community(g, -1, precision);

    if (verbose)
      cerr << "  modularity increased from " << mod << " to " << new_mod << endl;

    mod=new_mod;
    if (verbose)
      display_time("  end computation");

    if (filename_part!=NULL && level==1) // do at least one more computation if partition is provided
      improvement=true;
  } while(improvement);

  time_end2 = clock();
  if (verbose) {
    display_time("End");
  }
  //modified by sanaz: show the total duration regardless of being verbose or not
  cerr << "Total duration: " << (double)((time_end2-time_begin2)+(time_end1-time_begin1))/CLOCKS_PER_SEC << " sec." << endl;

  //sanaz: added in order to keep track of max level number for using in ./hierarchy
  cerr << "lastLevel: " << (level-1) << endl;
  if(new_mod-init_mod < 0.01)
	cerr << "stopIterating: 1" << endl;
  else
	cerr << "stopIterating: 0" << endl;
  
  cerr << "finalModularity: " << new_mod << endl;
}

