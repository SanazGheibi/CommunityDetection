#!/bin/bash
preOrder=$1
name=$2
n=$3
echo -n "" > runtimes.txt
echo -n "" > "${preOrder}_${name}.txt"
echo -n "" > bc.txt
./community ~/Louvain_input/"${preOrder}_${name}.bin" -l -1 -v  -q 0.0001 > graph.tree 2> res.txt
grep 'duration' ./res.txt >> runtimes.txt
grep 'lastLevel' ./res.txt > level_info.txt
read -r pass level < level_info.txt
./hierarchy graph.tree -l $level > comm.txt
rm graph.tree
cat res.txt >> "allResults_${preOrder}_${name}.txt"
echo -e "\n" >> "allResults_${preOrder}_${name}.txt"

for((i=1; i < $n; i++)); do
    ./community ~/Louvain_input/"${preOrder}_${name}.bin" -p comm.txt -l -1 -v -q 0.0001 > graph.tree 2> res.txt
    grep 'duration' ./res.txt >> runtimes.txt
    grep 'lastLevel' ./res.txt > level_info.txt
    read -r pass level < level_info.txt
    ./hierarchy graph.tree -l $level > comm.txt
    rm graph.tree
    cat res.txt >> "allResults_${preOrder}_${name}.txt"
    echo -e "\n" >> "allResults_${preOrder}_${name}.txt"
done

echo -e "sum = 0.0\n" >> bc.txt
while read -r col1 col2 col3 col4
do
   echo -e "sum = sum + $col3\n" >> bc.txt
done < runtimes.txt
echo -e "print sum\n" >> bc.txt
s=`cat bc.txt | bc -l`
echo "overall runtime: $s" >> "allResults_${preOrder}_${name}.txt"

#final clean up 
rm bc.txt
rm comm.txt
rm level_info.txt
rm res.txt
rm runtimes.txt
