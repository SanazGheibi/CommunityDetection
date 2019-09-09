#!/bin/bash
for i in $(seq 1.1 0.1 2); do
   ./community ~/Louvain_input/RandomShuffle_uk2005.bin -l -1 -v -q 0.0001 -a $i > graph.tree 2>"result$i.txt"
   #rm graph.tree 
done

