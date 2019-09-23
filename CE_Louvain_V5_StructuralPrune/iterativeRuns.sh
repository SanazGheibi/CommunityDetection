#!/bin/bash
n=$1
echo -n "" > runtimes.txt
echo -n "" > allResults.txt
echo -n "" > bc.txt
./community ~/Louvain_input/RandomShuffle_amazon.bin -r reordered.bin -c 4 -l -1 -v -q 0.0001 > graph.tree 2> res.txt
grep 'duration' ./res.txt >> runtimes.txt
grep 'lastLevel' ./res.txt > level_info.txt
grep 'boundary' ./res.txt > bnd_info.txt
read -r pass level < level_info.txt
read -r pass boundary < bnd_info.txt
./hierarchy graph.tree -l $level > comm.txt
cat res.txt >> allResults.txt
echo -e "\n" >> allResults.txt

for((i=1; i < $n; i++)); do  
    #boundary is the same in all iterations following the first iteration
    ./community reordered.bin -p comm.txt -b $boundary -c 4 -l -1 -v -q 0.0001 > graph.tree 2> res.txt
    grep 'duration' ./res.txt >> runtimes.txt
    grep 'lastLevel' ./res.txt > level_info.txt
    read -r pass level < level_info.txt
    ./hierarchy graph.tree -l $level > comm.txt
    cat res.txt >> allResults.txt
    echo -e "\n" >> allResults.txt
done

echo -e "sum = 0.0\n" >> bc.txt
while read -r col1 col2 col3 col4
do
   echo -e "sum = sum + $col3\n" >> bc.txt
done < runtimes.txt
echo -e "print sum\n" >> bc.txt
s=`cat bc.txt | bc -l`
echo "overall runtime: $s"

#final clean up 
rm bc.txt
rm comm.txt
rm level_info.txt
rm res.txt
rm runtimes.txt
rm bnd_info.txt
