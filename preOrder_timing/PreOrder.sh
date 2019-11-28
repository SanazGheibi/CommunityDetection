#!/bin/bash
path=$1
cache=$2 #1048576
n=$3 #10
#sample use:./PreOrder.sh . 1048576 10 
for name in amazon # IMDB liveJournal dblp
do
"${path}"/Gorder-master/Gorder -n $n "${path}"/Data/"${name}_RSH.txt" > "${path}"/Gorder-master/"${name}_log.txt"
"${path}"/graphReordering-master/block_reordering/br 64 ${cache} $n "${path}"/Data/"${name}_RSH_Gorder.txt" "${path}"/Data/"${name}_USC.bin" > "${path}"/graphReordering-master/block_reordering/"${name}_log.txt"
rm "${path}"/Data/"${name}_RSH_Gorder.txt"
"${path}"/SimpleBFS_RCM/BFSrcm "${path}"/Data/"${name}_RSH.txt" $n > "${path}"/SimpleBFS_RCM/"${name}_log.txt"
done
