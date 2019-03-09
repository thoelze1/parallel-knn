all: knn

knn: main.o io.o Node.o train.o KDTree.o
	g++ -O3 main.o io.o Node.o train.o KDTree.o -o knn

main.o: main.cpp io.h Node.h train.h
	g++ -O3 -c main.cpp

io.o: io.cpp io.h
	g++ -O3 -c io.cpp

Node.o: Node.cpp Node.h
	g++ -O3 -c Node.cpp

KDTree.o: KDTree.cpp KDTree.h
	g++ -O3 -c KDTree.cpp

train.o: train.cpp train.h Node.h
	g++ -O3 -c train.cpp

clean:
	rm *.o knn
