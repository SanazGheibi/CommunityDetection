#!/bin/bash
name=$1
path=$2  #path to current folder
dpath=$3 #path to dataset
cache=$4 #cache size in MBs
#sample use: ./iterativeRuns.sh amazon . ~ 4
echo "Code: CE_Louvain_V5_StructuralPrune"
echo "dataset: ${name}"
echo -n "" > "${path}"/runtimes.txt
echo -n "" > "${path}"/bc.txt
"${path}"/community "${dpath}"/Louvain_input/"RandomShuffle_${name}.bin" -r "${path}"/reordered.bin -c $cache -l -1 -v -q 0.001 > "${path}"/graph.tree 2> "${path}"/res.txt
grep 'pruning' "${path}"/res.txt >> "${path}"/runtimes.txt
grep 'pre_ordering' "${path}"/res.txt >> "${path}"/runtimes.txt
cat "${path}"/runtimes.txt 
grep 'duration' "${path}"/res.txt >> "${path}"/runtimes.txt
grep 'finalModularity' "${path}"/res.txt > "${path}"/mod_info.txt
grep 'lastLevel' "${path}"/res.txt > "${path}"/level_info.txt
read -r pass level < "${path}"/level_info.txt
"${path}"/hierarchy "${path}"/graph.tree -l $level > "${path}"/comm.txt
rm "${path}"/graph.tree

stop=0 #run for at least a seond iteration
while [  ${stop} -lt 1 ]; do
    "${path}"/community "${path}"/reordered.bin -p "${path}"/comm.txt -c $cache -l -1 -v -q 0.001 > "${path}"/graph.tree 2> "${path}"/res.txt
    grep 'duration' "${path}"/res.txt >> "${path}"/runtimes.txt
    grep 'finalModularity' "${path}"/res.txt > "${path}"/mod_info.txt
    grep 'stopIterating' "${path}"/res.txt > "${path}"/stop_info.txt
    read -r pass stop < "${path}"/stop_info.txt
    grep 'lastLevel' "${path}"/res.txt > "${path}"/level_info.txt
    read -r pass level < "${path}"/level_info.txt
    "${path}"/hierarchy "${path}"/graph.tree -l $level > "${path}"/comm.txt
    rm "${path}"/graph.tree
done

echo -e "sum = 0.0\n" >> "${path}"/bc.txt
while read -r col1 col2 col3 col4
do
   echo -e "sum = sum + $col3\n" >> "${path}"/bc.txt
done < "${path}"/runtimes.txt
echo -e "print sum\n" >> "${path}"/bc.txt
s=`cat "${path}"/bc.txt | bc -l`
echo "overall runtime: $s" 
cat "${path}"/mod_info.txt
echo -e "\n"

#final clean up 
rm "${path}"/bc.txt
rm "${path}"/comm.txt
rm "${path}"/level_info.txt
rm "${path}"/res.txt
rm "${path}"/runtimes.txt
rm "${path}"/mod_info.txt
rm "${path}"/stop_info.txt

