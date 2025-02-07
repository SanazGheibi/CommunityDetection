#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#define GRAPH_HEADER_INCL

typedef struct graph 
{
    unsigned int numVertex;
    unsigned int numEdges;
    unsigned int* VI;
    unsigned int* EI;
} graph;

int read_csr (char* filename, graph*);

void printGraph (char* filename, graph* G);

void freeMem(graph*);

