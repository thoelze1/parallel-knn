#!/bin/bash

for trainingSize in `seq 100000 20000 200000`;
do
	for dimension in `seq 2 3`;
	do
		rel1=$(./make_training_file $trainingSize $dimension 1)
		abs1=$(realpath $rel1)
		rel2=$(./make_query_file 1 $dimension 1 5)
		abs2=$(realpath $rel2)
		../knn 0 $abs1 $abs2 results.dat
		if ! ./verify-results $abs1 $abs2 results.dat; then
			echo "FAILED WITH " $rel1
			exit -1
		fi
	done
done
