#!/bin/bash
#no random shuffle is needed, but the naming remains the same to be compatible with real network scripts 
cache=1048576
for name in net6_0.2 net6_0.4 net6_0.6 net6_0.8
do
./Louvain_updated/convert -i ./Data/"${name}.txt" -o ~/Louvain_input/"RandomShuffle_${name}.bin" -r > "./Louvain_updated/${name}0_log.txt"
./Gorder-master/Gorder ./Data/"${name}.txt" > "./Gorder-master/${name}_log.txt"
./graphReordering-master/block_reordering/br 64 ${cache} ./Data/"${name}_Gorder.txt" ./Data/"${name}_USC.bin" > "./graphReordering-master/block_reordering/${name}_log.txt"
rm ./Data/"${name}_Gorder.txt"
./USC_to_Louvain/Reformat ./Data/"${name}_USC.bin" ./Data/"${name}_uscLouvainInput.txt" > "./USC_to_Louvain/${name}_log.txt"
rm ./Data/"${name}_USC.bin"
./Louvain_updated/convert -i ./Data/"${name}_uscLouvainInput.txt" -o ~/Louvain_input/"uscOrdered_${name}.bin" -r > "./Louvain_updated/${name}1_log.txt"
rm ./Data/"${name}_uscLouvainInput.txt"
./SimpleBFS_RCM/BFSrcm ./Data/"${name}.txt" > "./SimpleBFS_RCM/${name}_log.txt"
./Louvain_updated/convert -i ./Data/"${name}_BFSrcm.txt" -o ~/Louvain_input/"rcmOrdered_${name}.bin" -r > "./Louvain_updated/${name}2_log.txt"
rm ./Data/"${name}_BFSrcm.txt"
done
