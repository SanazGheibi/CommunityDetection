#!/bin/bash
name=$1
echo "Code: MoveFastLouvain"
echo "dataset: ${name}"
echo -n "" > runtimes.txt
echo -n "" > bc.txt
./community ~/Louvain_input/"RandomShuffle_${name}.bin" -l -1 -v  -q 0.0001 > graph.tree 2> res.txt
grep 'duration' ./res.txt >> runtimes.txt
grep 'finalModularity' ./res.txt > mod_info.txt
grep 'lastLevel' ./res.txt > level_info.txt
read -r pass level < level_info.txt
./hierarchy graph.tree -l $level > comm.txt
rm graph.tree

stop=0 #run for at least a seond iteration
while [  ${stop} -lt 1 ]; do
    ./community ~/Louvain_input/"RandomShuffle_${name}.bin" -p comm.txt -l -1 -v -q 0.0001 > graph.tree 2> res.txt
    grep 'duration' ./res.txt >> runtimes.txt
    grep 'finalModularity' ./res.txt > mod_info.txt
    grep 'stopIterating' ./res.txt > stop_info.txt
    read -r pass stop < stop_info.txt
    grep 'lastLevel' ./res.txt > level_info.txt
    read -r pass level < level_info.txt
    ./hierarchy graph.tree -l $level > comm.txt
    rm graph.tree
done

echo -e "sum = 0.0\n" >> bc.txt
while read -r col1 col2 col3 col4
do
   echo -e "sum = sum + $col3\n" >> bc.txt
done < runtimes.txt
echo -e "print sum\n" >> bc.txt
s=`cat bc.txt | bc -l`
echo "overall runtime: $s" 
cat mod_info.txt
echo -e "\n"

#final clean up 
rm bc.txt
rm comm.txt
rm level_info.txt
rm res.txt
rm runtimes.txt
rm mod_info.txt
rm stop_info.txt
