all: knn

knn: main.o io.o KDNode.o train.o KDTree.o
	g++ -O3 main.o io.o KDNode.o train.o KDTree.o -o knn

main.o: main.cpp io.h KDNode.h train.h
	g++ -O3 -c main.cpp

io.o: io.cpp io.h
	g++ -O3 -c io.cpp

KDNode.o: KDNode.cpp KDNode.h
	g++ -O3 -c KDNode.cpp

KDTree.o: KDTree.cpp KDTree.h
	g++ -O3 -c KDTree.cpp

train.o: train.cpp train.h KDNode.h
	g++ -O3 -c train.cpp

clean:
	rm *.o knn
