#!/bin/bash
for name in lj #net6_0.2 net6_0.4 net6_0.6 net6_0.8
do
./RandomShuffle/RSH ./Data/"${name}.txt" > "./RandomShuffle/${name}_log.txt"
./Louvain_updated/convert -i ./Data/"${name}_RSH.txt" -o ~/Louvain_input/"RandomShuffle_${name}.bin" -r > "./Louvain_updated/${name}0_log.txt"
rm ./Data/"${name}.txt"
./Gorder-master/Gorder ./Data/"${name}_RSH.txt" > "./Gorder-master/${name}_log.txt"
./graphReordering-master/block_reordering/br 64 4194304 ./Data/"${name}_RSH_Gorder.txt" ./Data/"${name}_USC.bin" > "./graphReordering-master/block_reordering/${name}_log.txt"
rm ./Data/"${name}_RSH_Gorder.txt"
./USC_to_Louvain/Reformat ./Data/"${name}_USC.bin" ./Data/"${name}_uscLouvainInput.txt" > "./USC_to_Louvain/${name}_log.txt"
rm ./Data/"${name}_USC.bin"
./Louvain_updated/convert -i ./Data/"${name}_uscLouvainInput.txt" -o ~/Louvain_input/"uscOrdered_${name}.bin" -r > "./Louvain_updated/${name}1_log.txt"
rm ./Data/"${name}_uscLouvainInput.txt"
./SimpleBFS_RCM/BFSrcm ./Data/"${name}_RSH.txt" > "./SimpleBFS_RCM/${name}_log.txt"
./Louvain_updated/convert -i ./Data/"${name}_RSH_BFSrcm.txt" -o ~/Louvain_input/"rcmOrdered_${name}.bin" -r > "./Louvain_updated/${name}2_log.txt"
rm ./Data/"${name}_RSH_BFSrcm.txt"
done
