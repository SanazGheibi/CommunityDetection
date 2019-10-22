#!/bin/bash
n=10
cache=4 #size of cache in MBs
#Each code is ran n times and mean runtimes and modularities are reported
logFile="./nBulkIterativeLog.txt"
echo -n "" > "$logFile"
for name in amazon #net6_0.2 net6_0.4 net6_0.6 net6_0.8
do
./CE_Louvain_V5/iterativeRuns.sh "${name}" ./CE_Louvain_V5 ~ ${cache} $n >> "$logFile"
./CE_Louvain_V5_1/iterativeRuns.sh "${name}" ./CE_Louvain_V5_1 ~ ${cache} $n >> "$logFile"
./CE_Louvain_V5_StructuralPrune/iterativeRuns.sh "${name}" ./CE_Louvain_V5_StructuralPrune ~ ${cache} $n >> "$logFile"
./Louvain_seq/iterativeRuns.sh RandomShuffle "${name}" ./Louvain_seq ~ $n >> "$logFile"
./Louvain_seq/iterativeRuns.sh rcmOrdered "${name}" ./Louvain_seq ~ $n >> "$logFile"
./Louvain_seq/iterativeRuns.sh uscOrdered "${name}" ./Louvain_seq ~ $n >> "$logFile"
./Louvain_updated/iterativeRuns.sh "${name}" ./Louvain_updated ~ $n >> "$logFile"
./MoveFastLouvian/iterativeRuns.sh "${name}" ./MoveFastLouvian ~ $n >> "$logFile"
./MoveFast_seq/iterativeRuns.sh RandomShuffle "${name}" ./MoveFast_seq ~ $n >> "$logFile"
./MoveFast_seq/iterativeRuns.sh rcmOrdered "${name}" ./MoveFast_seq ~ $n >> "$logFile"
./MoveFast_seq/iterativeRuns.sh uscOrdered "${name}" ./MoveFast_seq ~ $n >> "$logFile"
done
