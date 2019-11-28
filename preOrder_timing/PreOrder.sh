#!/bin/bash
cache=1048576
n=10
for name in amazon IMDB liveJournal dblp
do
./Gorder-master/Gorder -n $n ./Data/"${name}_RSH.txt" > "./Gorder-master/${name}_log.txt"
./graphReordering-master/block_reordering/br 64 ${cache} $n ./Data/"${name}_RSH_Gorder.txt" ./Data/"${name}_USC.bin" > "./graphReordering-master/block_reordering/${name}_log.txt"
rm ./Data/"${name}_RSH_Gorder.txt"
./SimpleBFS_RCM/BFSrcm ./Data/"${name}_RSH.txt" $n > "./SimpleBFS_RCM/${name}_log.txt"
done
