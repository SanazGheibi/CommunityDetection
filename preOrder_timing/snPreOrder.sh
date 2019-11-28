#!/bin/bash
#no random shuffle is needed, but the naming remains the same to be compatible with real network scripts 
path=$1
cache=$2
n=$3 
#sample use: ./snPreOrder.sh . 1048576 10 
for name in net6_0.2 #net6_0.4 net6_0.6 net6_0.8
do
"${path}"/Gorder-master/Gorder -n $n "${path}"/Data/"${name}.txt" > "${path}"/Gorder-master/"${name}_log.txt"
"${path}"/graphReordering-master/block_reordering/br 64 ${cache} $n "${path}"/Data/"${name}_Gorder.txt" "${path}"/Data/"${name}_USC.bin" > "${path}"/graphReordering-master/block_reordering/"${name}_log.txt"
rm "${path}"/Data/"${name}_Gorder.txt"
"${path}"/SimpleBFS_RCM/BFSrcm "${path}"/Data/"${name}.txt" $n > "${path}"/SimpleBFS_RCM/"${name}_log.txt"
done
