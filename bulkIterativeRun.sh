#!/bin/bash
for name in amazon dblp IMDB liveJournal
do
cd ./CE_Louvain_V5
./iterativeRuns.sh "${name}" 5
cd ../CE_Louvain_V5_1
./iterativeRuns.sh "${name}" 5
cd ../CE_Louvain_V5_StructuralPrune
./iterativeRuns.sh "${name}" 5
cd ../Louvain_seq
./iterativeRuns.sh RandomShuffle "${name}" 5
./iterativeRuns.sh rcmOrdered "${name}" 5
./iterativeRuns.sh uscOrdered "${name}" 5
cd ../Louvain_updated
./iterativeRuns.sh "${name}" 5
cd ../MoveFastLouvian
./iterativeRuns.sh "${name}" 5
cd ../MoveFast_seq
./iterativeRuns.sh RandomShuffle "${name}" 5
./iterativeRuns.sh rcmOrdered "${name}" 5
./iterativeRuns.sh uscOrdered "${name}" 5
cd ..
done
