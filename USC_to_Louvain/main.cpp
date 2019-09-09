#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <unordered_map>

#include "graph.h"

using namespace std;

int main(int argc, char** argv)
{

    // 2 files for 2 different CSR representations
    if (argc != 3)
    {
        printf("Usage : %s <inputFile1> <outputFile>\n", argv[0]);
        exit(1);
    }
    graph G1;  
    read_csr(argv[1], &G1);
    printGraph (argv[2], &G1);
    freeMem(&G1);

    return 0;
}

