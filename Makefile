all: knn

knn: knn.cpp io.cpp io.h Node.h Node.cpp train.h train.cpp
	g++ knn.cpp train.cpp io.cpp Node.cpp -o knn -O3

clean:
	rm knn
