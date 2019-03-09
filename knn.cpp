/*
 * Tanner Hoelzel
 */

#include <iostream>
#include <assert.h>
#include <string.h>
#include <random>
#include <vector>
#include <algorithm>

#include "io.h"
#include "Node.h"

#define CELLSIZE	8
#define SAMPLESIZE	10'000

// change to use SAMPLESIZE
float
getPivot(float *points, uint64_t startIndex, uint64_t endIndex, uint64_t d, uint64_t currd) {
    uint64_t numPoints = endIndex - startIndex;
    std::vector<float> sample(numPoints);
    std::default_random_engine eng;
    std::uniform_int_distribution<int> dist(0, numPoints);
    for(unsigned int i = 0; i < numPoints; i++) {
        int randomIndex = dist(eng);
        sample[i] = points[(startIndex+randomIndex)*d+currd];
    }
    std::nth_element(sample.begin(), sample.begin() + sample.size()/2, sample.end());
    return sample[sample.size()/2];
}

/* see http://www.cplusplus.com/reference/algorithm/partition/ */
uint64_t
partition(float *points, uint64_t startIndex, uint64_t endIndex, uint64_t d, uint64_t currd) {
    int first, last;
    float pivotVal = getPivot(points, startIndex, endIndex, d, currd);
    float *tempPoint = new float[d];
    first = startIndex;
    last = endIndex;
    while (first != last) {
      while (points[first*d+currd] < pivotVal) {
        ++first;
        if (first == last) return first;
      }
      do {
        --last;
        if (first == last) return first;
      } while (points[last*d+currd] >= pivotVal);
      memcpy(tempPoint, &points[first*d], d*sizeof(float));
      memcpy(&points[first*d], &points[last*d], d*sizeof(float));
      memcpy(&points[last*d], tempPoint, d*sizeof(float));
      ++first;
    }
    return first;
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
    node->left = buildTree(points, startIndex, pivotIndex, d, currd+1%d);
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

    float *newPoints = new float[nPoints*nDim];
    for(int i = 0; i < nPoints*nDim; i++) {
        newPoints[i] = points[i];
    }

    float *newQueries = new float[nQueries*nDim];
    for(int i = 0; i < nQueries*nDim; i++) {
        newQueries[i] = queries[i];
    }

    rv = closeFile(trainingData, trainingFd, trainingFileSize);
    rv = closeFile(queryData, queryFd, queryFileSize);

    Node *tree = buildTree(newPoints, 0, nPoints, nDim, 0);

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

    rv = closeFile(resultsData, resultsFd, resultsDataSize);

    return 0;
}
