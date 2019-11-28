#!/bin/bash
#no random shuffle is needed, but the naming remains the same to be compatible with real network scripts 
cache=1048576
for name in net6_0.2 net6_0.4 net6_0.6 net6_0.8
do
./Gorder-master/Gorder -n $n ./Data/"${name}.txt" > "./Gorder-master/${name}_log.txt"
./graphReordering-master/block_reordering/br 64 ${cache} $n ./Data/"${name}_Gorder.txt" ./Data/"${name}_USC.bin" > "./graphReordering-master/block_reordering/${name}_log.txt"
rm ./Data/"${name}_Gorder.txt"
./SimpleBFS_RCM/BFSrcm ./Data/"${name}.txt" $n > "./SimpleBFS_RCM/${name}_log.txt"
done
