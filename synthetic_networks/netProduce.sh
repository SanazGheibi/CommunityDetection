#!/bin/bash
for muVal in $(seq 0.2 0.2 0.8); do
	nVal=1000000
	nIndx=6
	./benchmark -N "${nVal}" -k 10 -maxk 50 -mu "${muVal}" -minc 50 -maxc 50
	mv network.dat "net${nIndx}_${muVal}.txt"
	rm community.dat
	rm statistics.dat
done
for muVal in $(seq 0.2 0.2 0.8); do
	nVal=10000000
	nIndx=7
	./benchmark -N "${nVal}" -k 10 -maxk 50 -mu "${muVal}" -minc 50 -maxc 50
	mv network.dat "net${nIndx}_${muVal}.txt"
	rm community.dat
	rm statistics.dat
done
