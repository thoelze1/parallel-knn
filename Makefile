all: knn

knn: knn.cpp io.cpp io.h Node.h Node.cpp
	g++ knn.cpp io.cpp Node.cpp -o knn

clean:
	rm knn
