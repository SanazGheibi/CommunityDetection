#!/bin/bash
preOrder=$1
name=$2
path=$3  #path to current folder
dpath=$4 #path to dataset
n=$5
#sample use: ./nIterativeRuns.sh RandomShuffle amazon . ~ 10
#Here, unlike iterativeRuns.sh, the preordering time is not included in the overall runtime (of one run)
echo "Code: Louvain_seq"
echo "dataset: ${preOrder}_${name}"
echo -n "" > "${path}"/nRuntime.txt
echo -n "" > "${path}"/nMod.txt
for ((i=1; i <= $n; i++)); do
	#echo "run number: ${i}"
	echo -n "" > "${path}"/runtimes.txt
	echo -n "" > "${path}"/bc.txt
	"${path}"/community "${dpath}"/Louvain_input/"${preOrder}_${name}.bin" -l -1 -v  -q 0.001 > "${path}"/graph.tree 2> "${path}"/res.txt
	#cat "${path}"/res.txt
	grep 'duration' "${path}"/res.txt >> "${path}"/runtimes.txt
	grep 'finalModularity' "${path}"/res.txt > "${path}"/mod_info.txt
	grep 'lastLevel' "${path}"/res.txt > "${path}"/level_info.txt
	read -r pass level < "${path}"/level_info.txt
	"${path}"/hierarchy "${path}"/graph.tree -l $level > "${path}"/comm.txt
	#echo "done with hierarchy"
	rm "${path}"/graph.tree

	stop=0 #run for at least a seond iteration
	while [  ${stop} -lt 1 ]; do
	    "${path}"/community "${dpath}"/Louvain_input/"${preOrder}_${name}.bin" -p "${path}"/comm.txt -l -1 -v -q 0.001 > "${path}"/graph.tree 2> "${path}"/res.txt
	    #cat "${path}"/res.txt
	    grep 'duration' "${path}"/res.txt >> "${path}"/runtimes.txt
	    grep 'finalModularity' "${path}"/res.txt > "${path}"/mod_info.txt
	    grep 'stopIterating' "${path}"/res.txt > "${path}"/stop_info.txt
	    read -r pass stop < "${path}"/stop_info.txt
	    grep 'lastLevel' "${path}"/res.txt > "${path}"/level_info.txt
	    read -r pass level < "${path}"/level_info.txt
	    "${path}"/hierarchy "${path}"/graph.tree -l $level > "${path}"/comm.txt
	    #echo "done with hierarchy" 
	    rm "${path}"/graph.tree
	done

	echo -e "sum = 0.0\n" >> "${path}"/bc.txt
	while read -r col1 col2 col3 col4
	do
	   echo -e "sum = sum + $col3\n" >> "${path}"/bc.txt
	done < "${path}"/runtimes.txt
	echo -e "print sum\n" >> "${path}"/bc.txt
	s=`cat "${path}"/bc.txt | bc -l`
	echo "overall runtime: $s" >> "${path}"/nRuntime.txt
	cat "${path}"/mod_info.txt >> "${path}"/nMod.txt

	#final clean up 
	rm "${path}"/bc.txt
	rm "${path}"/comm.txt
	rm "${path}"/level_info.txt
	rm "${path}"/res.txt
	rm "${path}"/runtimes.txt
	rm "${path}"/mod_info.txt
	rm "${path}"/stop_info.txt
done
#compute mean community detectin time
echo -n "" > "${path}"/bc.txt
echo -e "sum = 0.0\n" >> "${path}"/bc.txt
while read -r col1 col2 col3
do
   echo -e "sum = sum + $col3\n" >> "${path}"/bc.txt
done < "${path}"/nRuntime.txt
echo -e "sum = sum/$n\n" >> "${path}"/bc.txt
echo -e "print sum\n" >> "${path}"/bc.txt
s=`cat "${path}"/bc.txt | bc -l`
echo mean community detection time: $s

#compute mean modularity
echo -n "" > "${path}"/bc.txt
echo -e "sum = 0.0\n" >> "${path}"/bc.txt
while read -r col1 col2 
do
   echo -e "sum = sum + $col2\n" >> "${path}"/bc.txt
done < "${path}"/nMod.txt
echo -e "sum = sum/$n\n" >> "${path}"/bc.txt
echo -e "print sum\n" >> "${path}"/bc.txt
s=`cat "${path}"/bc.txt | bc -l`
echo mean modularity: $s
