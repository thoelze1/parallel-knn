GPROFFLAGS=-pg -g -no-pie

all: k-nn

k-nn: main.o io.o KDNode.o KDTree.o
	g++ -O3 main.o io.o KDNode.o KDTree.o -o k-nn -pthread

main.o: main.cpp io.h KDNode.h
	g++ -O3 -c main.cpp

io.o: io.c io.h
	gcc -O3 -c io.c

KDNode.o: KDNode.cpp KDNode.h
	g++ -O3 -c KDNode.cpp

KDTree.o: KDTree.cpp KDTree.h
	g++ -O3 -c KDTree.cpp

clean:
	rm -f *.o k-nn
