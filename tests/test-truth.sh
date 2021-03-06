#!/bin/bash

: '
for trainingSize in `seq 2 2 10`;
do
	for dimension in `seq 1 2 10`;
	do
		for k in `seq 2 2 $trainingSize`;
		do
			for numQueries in `seq 1 2 10`;
			do
				rel1=$(./make_training_file $trainingSize $dimension 1)
				abs1=$(realpath $rel1)
				rel2=$(./make_query_file $numQueries $dimension 1 $k)
				abs2=$(realpath $rel2)
				for numCores in `seq 1 4`;
				do
					../k-nn $numCores $abs1 $abs2 results.dat
					if ! ./verify-results $abs1 $abs2 results.dat; then
						echo "FAILED WITH " $rel1 $rel2
						exit -1
					fi
				done
			done
		done
	done
done
'

for trainingSize in `seq 10000 1000 15000`;
do
	for dimension in `seq 5 5 10`;
	do
		rel1=$(./make_training_file $trainingSize $dimension 1)
		abs1=$(realpath $rel1)
		rel2=$(./make_query_file 50 $dimension 1 100)
		abs2=$(realpath $rel2)
		echo "--------------------------------------"
		../k-nn 8 $abs1 $abs2 results.dat
		if ! ./verify-results $abs1 $abs2 results.dat; then
			echo "FAILED WITH " $rel1 $rel2
			exit -1
		fi
		rm $abs1
		rm $abs2
	done
done

echo "Passed all test cases"
