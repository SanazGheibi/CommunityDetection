#!/bin/bash
name="IMDB"
./Louvain_updated/community ~/Louvain_input/"RandomShuffle_${name}.bin" -l -1 -v -q 0.0001 > ./Louvain_updated/graph.tree 2> "./Louvain_updated/${name}_res.txt" 
rm ./Louvain_updated/graph.tree
./Louvain_seq/community ~/Louvain_input/"RandomShuffle_${name}.bin" -l -1 -v -q 0.0001 > ./Louvain_seq/graph.tree 2> "./Louvain_seq/${name}_res.txt"
rm ./Louvain_seq/graph.tree
./Louvain_seq/community ~/Louvain_input/"uscOrdered_${name}.bin" -l -1 -v -q 0.0001 > ./Louvain_seq/graph.tree 2> "./Louvain_seq/usc_${name}_res.txt"
rm ./Louvain_seq/graph.tree
./Louvain_seq/community ~/Louvain_input/"rcmOrdered_${name}.bin" -l -1 -v -q 0.0001 > ./Louvain_seq/graph.tree 2> "./Louvain_seq/rcm_${name}_res.txt"
rm ./Louvain_seq/graph.tree
./MoveFastLouvain/community ~/Louvain_input/"RandomShuffle_${name}.bin" -l -1 -v -q 0.0001 > ./MoveFastLouvain/graph.tree 2> "./MoveFastLouvain/${name}_res.txt"
rm ./MoveFastLouvain/graph.tree
./MoveFast_seq/community ~/Louvain_input/"RandomShuffle_${name}.bin" -l -1 -v -q 0.0001 > ./MoveFast_seq/graph.tree 2> "./MoveFast_seq/${name}_res.txt"
rm ./MoveFast_seq/graph.tree
./MoveFast_seq/community ~/Louvain_input/"uscOrdered_${name}.bin" -l -1 -v -q 0.0001 > ./MoveFast_seq/graph.tree 2> "./MoveFast_seq/usc_${name}_res.txt"
rm ./MoveFast_seq/graph.tree
./MoveFast_seq/community ~/Louvain_input/"rcmOrdered_${name}.bin" -l -1 -v -q 0.0001 > ./MoveFast_seq/graph.tree 2> "./MoveFast_seq/rcm_${name}_res.txt"
rm ./MoveFast_seq/graph.tree
./CE_Louvain_V5_1/community ~/Louvain_input/"rcmOrdered_${name}.bin" -l -1 -v -q 0.0001 > ./CE_Louvain_V5_1/graph.tree 2> "./CE_Louvain_V5_1/rcm_${name}_res.txt"
rm ./CE_Louvain_V5_1/graph.tree
./CE_Louvain_V5/community ~/Louvain_input/"RandomShuffle_${name}.bin" -l -1 -v -q 0.0001 > ./CE_Louvain_V5/graph.tree 2> "./CE_Louvain_V5/${name}_res.txt"
rm ./CE_Louvain_V5/graph.tree
./CE_Louvain_V5_StructuralPrune/community ~/Louvain_input/"RandomShuffle_${name}.bin" -l -1 -v -q 0.0001 > ./CE_Louvain_V5_StructuralPrune/graph.tree 2> "./CE_Louvain_V5_StructuralPrune/${name}_res.txt"
rm ./CE_Louvain_V5_StructuralPrune/graph.tree
./CE_Louvain_V3/community ~/Louvain_input/"RandomShuffle_${name}.bin" -l -1 -v -q 0.0001 > ./CE_Louvain_V3/graph.tree 2> "./CE_Louvain_V3/${name}_res.txt"
rm ./CE_Louvain_V3/graph.tree
