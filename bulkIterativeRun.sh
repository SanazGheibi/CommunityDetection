#!/bin/bash
logFile="./bulkIterativeLog.txt"
echo -n "" > "$logFile"
for name in net6_0.2 net6_0.4 net6_0.6 net6_0.8
do
./CE_Louvain_V5/iterativeRuns.sh "${name}" ./CE_Louvain_V5 ~ >> "$logFile"
./CE_Louvain_V5_1/iterativeRuns.sh "${name}" ./CE_Louvain_V5_1 ~ >> "$logFile"
./CE_Louvain_V5_StructuralPrune/iterativeRuns.sh "${name}" ./CE_Louvain_V5_StructuralPrune ~ >> "$logFile"
./Louvain_seq/iterativeRuns.sh RandomShuffle "${name}" ./Louvain_seq ~ >> "$logFile"
./Louvain_seq/iterativeRuns.sh rcmOrdered "${name}" ./Louvain_seq ~ >> "$logFile"
./Louvain_seq/iterativeRuns.sh uscOrdered "${name}" ./Louvain_seq ~ >> "$logFile"
./Louvain_updated/iterativeRuns.sh "${name}" ./Louvain_updated ~ >> "$logFile"
./MoveFastLouvian/iterativeRuns.sh "${name}" ./MoveFastLouvian ~ >> "$logFile"
./MoveFast_seq/iterativeRuns.sh RandomShuffle "${name}" ./MoveFast_seq ~ >> "$logFile"
./MoveFast_seq/iterativeRuns.sh rcmOrdered "${name}" ./MoveFast_seq ~ >> "$logFile"
./MoveFast_seq/iterativeRuns.sh uscOrdered "${name}" ./MoveFast_seq ~ >> "$logFile"
done
