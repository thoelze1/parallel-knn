all: knn

knn: main.o io.o KDNode.o KDTree.o
	g++ -O3 main.o io.o KDNode.o KDTree.o -o knn

main.o: main.cpp io.h KDNode.h
	g++ -O3 -c main.cpp

io.o: io.cpp io.h
	g++ -O3 -c io.cpp

KDNode.o: KDNode.cpp KDNode.h
	g++ -O3 -c KDNode.cpp

KDTree.o: KDTree.cpp KDTree.h
	g++ -O3 -c KDTree.cpp

clean:
	rm *.o knn
