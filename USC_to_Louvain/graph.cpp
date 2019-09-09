#include "graph.h"

int read_csr (char* filename, graph* G)
{
    FILE* graphFile = fopen(filename, "rb");
    if (graphFile == NULL)
    {
        fputs("file error", stderr);
        return -1;
    }
    fread (&(G->numVertex), sizeof(unsigned int), 1, graphFile);
    fread (&(G->numEdges), sizeof(unsigned int), 1, graphFile);


    G->VI = new unsigned int[G->numVertex];

    fread (G->VI, sizeof(unsigned int), G->numVertex, graphFile);
    if (feof(graphFile))
    {
        delete[] G->VI;
        printf("unexpected end of file while reading vertices\n");
        return -1;
    }
    else if (ferror(graphFile))
    {
        delete[] G->VI;
        printf("error reading file\n");
        return -1;
    }

    G->EI = new unsigned int[G->numEdges];
    fread (G->EI, sizeof(unsigned int), G->numEdges, graphFile);
    if (feof(graphFile))
    {
        delete[] G->VI;
        delete[] G->EI;
        printf("unexpected end of file while reading edges\n");
        return -1;
    }
    else if (ferror(graphFile))
    {
        delete[] G->VI;
        delete[] G->EI;
        printf("error reading file\n");
        return -1;
    }
    fclose(graphFile);
    return 1;
}
void printGraph (char* filename, graph* G){
	FILE* fp = fopen(filename, "w");
	if (fp == NULL){
	    fputs("file error", stderr);
	    return;
	}
	for(unsigned int u=0; u<G->numVertex; u++){
		unsigned int maxId = (u==G->numVertex-1) ? G->numEdges : G->VI[u+1];
		for (unsigned int j=G->VI[u]; j<maxId; j++){
			unsigned int v=G->EI[j];
			fprintf(fp, "%d\t%d\n", u, v);
		}
	}
}
void freeMem (graph* G)
{
    delete[] G->VI;
    delete[] G->EI;
}

