#!/bin/bash
for counter in 0.00001 0.0001 0.001 0.01 0.1 
do
    echo "threshold increment = ${counter}"
    ./community /home/sanaz/Louvain_input/RandomShuffle_amazon.bin -c 1 -l -1 -v -q ${counter} > graph.tree 
    echo "*"
done
