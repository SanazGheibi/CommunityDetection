#!/bin/bash
for name in net6_0.2 net6_0.4 net6_0.6 net6_0.8
do
cd ./CE_Louvain_V5
./iterativeRuns.sh "${name}" 
cd ../CE_Louvain_V5_1
./iterativeRuns.sh "${name}" 
cd ../CE_Louvain_V5_StructuralPrune
./iterativeRuns.sh "${name}" 
cd ../Louvain_seq
./iterativeRuns.sh RandomShuffle "${name}" 
./iterativeRuns.sh rcmOrdered "${name}" 
./iterativeRuns.sh uscOrdered "${name}"
cd ../Louvain_updated
./iterativeRuns.sh "${name}" 
cd ../MoveFastLouvian
./iterativeRuns.sh "${name}" 
cd ../MoveFast_seq
./iterativeRuns.sh RandomShuffle "${name}"
./iterativeRuns.sh rcmOrdered "${name}" 
./iterativeRuns.sh uscOrdered "${name}"
cd ..
done
