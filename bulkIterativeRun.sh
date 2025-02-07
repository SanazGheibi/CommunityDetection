#!/bin/bash
path=$1  #path to current folder
dpath=$2 #path to Louvain_input folder (datasets)
cache=$3 #cache size in MBs
#sample use: ./bulkIterativeRun.sh . ~ 4
logFile="${path}/liveJournal_bulkIterativeLog.txt"
echo -n "" > "$logFile"
for name in liveJournal 
do
"${path}"/CE_Louvain_V5/iterativeRuns.sh "${name}" "${path}"/CE_Louvain_V5 ${dpath} ${cache} >> "$logFile"
"${path}"/CE_Louvain_V5_1/iterativeRuns.sh "${name}" "${path}"/CE_Louvain_V5_1 ${dpath} ${cache} >> "$logFile"
"${path}"/CE_Louvain_V5_StructuralPrune/iterativeRuns.sh "${name}" "${path}"/CE_Louvain_V5_StructuralPrune ${dpath} ${cache} >> "$logFile"
"${path}"/MoveFastLouvian/iterativeRuns.sh "${name}" "${path}"/MoveFastLouvian ${dpath} >> "$logFile"
"${path}"/MoveFast_seq/iterativeRuns.sh RandomShuffle "${name}" "${path}"/MoveFast_seq ${dpath} >> "$logFile"
"${path}"/MoveFast_seq/iterativeRuns.sh rcmOrdered "${name}" "${path}"/MoveFast_seq ${dpath} >> "$logFile"
"${path}"/MoveFast_seq/iterativeRuns.sh uscOrdered "${name}" "${path}"/MoveFast_seq ${dpath} >> "$logFile"
done
