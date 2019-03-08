all: knn

knn: knn.cpp
	g++ knn.cpp io.cpp -o knn

clean:
	rm knn
