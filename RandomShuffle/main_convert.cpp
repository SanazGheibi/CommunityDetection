#include "graph.h"

using namespace std;

//string infile   = NULL;
//string outfile  = NULL;

string extractFilename(string& name){
	//string name(filename);
	int pos=name.find_last_of('.');

	return name.substr(0, pos);
}

int
main(int argc, char **argv) {
  
  if(argc == 1){
     cout << "input file name missing1" << endl;
     exit(0);
  }
  string infile = argv[1];
  string outfile = extractFilename(filename) + "_RSH";

  Graph g(infile);
 
  g.clean();

  g.renumber();

  vector<int> order;
  g.RandomShuffle(order);

  g.display_ascii(outfile, outfile_w, order);

}
