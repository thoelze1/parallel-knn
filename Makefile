all: knn

knn: knn.cpp
	g++ knn.cpp -o knn

clean:
	rm knn
