#!/bin/bash
path=$1  #path to current folder
dpath=$2 #path to Louvain_input folder (datasets)
cache=$3 #cache size in MBs 
n=$4
#sample use: ./nBulkIterativeRun.sh . ~ 4 10
#Each code is ran n times and mean runtimes and modularities are reported
logFile="${path}/uk2005_3BulkIterativeLog.txt"
echo -n "" > "$logFile"
for name in uk2005
do
"${path}"/CE_Louvain_V5/nIterativeRuns.sh "${name}" "${path}"/CE_Louvain_V5 ${dpath} ${cache} $n >> "$logFile"
"${path}"/CE_Louvain_V5_1/nIterativeRuns.sh "${name}" "${path}"/CE_Louvain_V5_1 ${dpath} ${cache} $n >> "$logFile"
"${path}"/CE_Louvain_V5_StructuralPrune/nIterativeRuns.sh "${name}" "${path}"/CE_Louvain_V5_StructuralPrune ${dpath} ${cache} $n >> "$logFile"
"${path}"/Louvain_seq/nIterativeRuns.sh RandomShuffle "${name}" "${path}"/Louvain_seq ${dpath} $n >> "$logFile"
"${path}"/Louvain_seq/nIterativeRuns.sh rcmOrdered "${name}" "${path}"/Louvain_seq ${dpath} $n >> "$logFile"
"${path}"/Louvain_seq/nIterativeRuns.sh uscOrdered "${name}" "${path}"/Louvain_seq ${dpath} $n >> "$logFile"
"${path}"/Louvain_updated/nIterativeRuns.sh "${name}" "${path}"/Louvain_updated ${dpath} $n >> "$logFile"
"${path}"/MoveFastLouvian/nIterativeRuns.sh "${name}" "${path}"/MoveFastLouvian ${dpath} $n >> "$logFile"
"${path}"/MoveFast_seq/nIterativeRuns.sh RandomShuffle "${name}" "${path}"/MoveFast_seq ${dpath} $n >> "$logFile"
"${path}"/MoveFast_seq/nIterativeRuns.sh rcmOrdered "${name}" "${path}"/MoveFast_seq ${dpath} $n >> "$logFile"
"${path}"/MoveFast_seq/nIterativeRuns.sh uscOrdered "${name}" "${path}"/MoveFast_seq ${dpath} $n >> "$logFile"
done
