/*
 * Tanner Hoelzel
 */

#include <iostream>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <iomanip>
#include <random>
#include <vector>
#include <algorithm>

#include "io.hpp"

#define CELLSIZE	1'000'000'000
#define SAMPLESIZE	10'000

class Node {
    public:
        Node(uint64_t start, uint64_t end) {
            startIndex = start;
            endIndex = end;
        }
        Node(uint64_t start, uint64_t end, float med, unsigned int d) {
            startIndex = start;
            endIndex = end;
            median = med;
            dim = d;
        }
        uint64_t startIndex, endIndex;
        float median;
        unsigned int dim;
        Node *left;
        Node *right;
    private:
};

float
getPivot(float *points, uint64_t startIndex, uint64_t endIndex, uint64_t d, uint64_t currd) {
    uint64_t numPoints = endIndex - startIndex;
    std::vector<float> sample(SAMPLESIZE);
    std::default_random_engine eng;
    std::uniform_int_distribution<int> dist(0, numPoints);
    for(unsigned int i = 0; i < SAMPLESIZE; i++) {
        int randomIndex = dist(eng);
        sample[i] = points[(startIndex+randomIndex)*d+currd];
    }
    std::nth_element(sample.begin(), sample.begin() + sample.size()/2, sample.end());
    return sample[sample.size()/2];
}

/* see http://www.cplusplus.com/reference/algorithm/partition/ */
uint64_t
partition(float *points, uint64_t startIndex, uint64_t endIndex, uint64_t d, uint64_t currd) {
    /*
    if(endIndex - startIndex < CELLSIZE) {

    }
    while (first!=last) {
      while (pred(*first)) {
        ++first;
        if (first==last) return first;
      }
      do {
        --last;
        if (first==last) return first;
      } while (!pred(*last));
      swap (*first,*last);
      ++first;
    }
    return first;
    */
    return 0;
}

// Make Leaf class?
Node *
buildTree(float *points, uint64_t startIndex, uint64_t endIndex, uint64_t d, uint64_t currd) {
    if(endIndex - startIndex < CELLSIZE) {
        return new Node(startIndex, endIndex);
    }
    uint64_t pivotIndex = partition(points, startIndex, endIndex, d, currd);
    uint64_t pivotValue = points[pivotIndex*d+currd];
    Node *node = new Node(startIndex, endIndex, pivotValue, d);
    node->left = buildTree(points, startIndex, pivotIndex-1, d, currd+1%d);
    node->right = buildTree(points, pivotIndex, endIndex, d, currd+1%d);
}

int
main(int argc, char **argv) {

    int rv, trainingFd, queryFd, resultsFd;
    char *trainingData, *queryData, *resultsData;
    unsigned int trainingFileSize, queryFileSize, resultsDataSize;

    trainingData = readFile(argv[2], &trainingFd, &trainingFileSize);
    queryData = readFile(argv[3], &queryFd, &queryFileSize);

    uint64_t trainingFileId = *(uint64_t *)(trainingData+8);
    uint64_t nPoints = *(uint64_t *)(trainingData+16);
    uint64_t nDim = *(uint64_t *)(trainingData+24);
    float *points = (float *)(trainingData+32);

    uint64_t queryFileId = *(uint64_t *)(queryData+8);
    uint64_t nQueries = *(uint64_t *)(queryData+16);
    uint64_t k = *(uint64_t *)(queryData+32);
    float *queries = (float *)(queryData+40);

    Node *tree = buildTree(points, 0, nPoints-1, nDim, 0);

    resultsDataSize = 7*8 + nQueries*k*nDim*sizeof(float);
    resultsData= writeFile(argv[4], &resultsFd, resultsDataSize);

    std::string results("RESULTS\0");
    results.copy(resultsData, results.length());
    *(uint64_t *)(resultsData+8) = trainingFileId;
    *(uint64_t *)(resultsData+16) = queryFileId;
    //*(uint64_t *)(resultsData+24) = from /dev/urandom
    *(uint64_t *)(resultsData+32) = nQueries;
    *(uint64_t *)(resultsData+40) = nDim;
    *(uint64_t *)(resultsData+48) = k;

    rv = closeFile(trainingData, trainingFd, trainingFileSize);
    rv = closeFile(queryData, queryFd, queryFileSize);
    rv = closeFile(resultsData, resultsFd, resultsDataSize);

    return 0;
}
