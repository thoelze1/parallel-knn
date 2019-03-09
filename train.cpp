/*
 * Tanner Hoelzel
 */

#include <string.h>
#include <random>
#include <vector>
#include <algorithm>

#define CELLSIZE	8
#define SAMPLESIZE	10'000

#include "Node.h"

// change to use SAMPLESIZE
float
getPivot(float *points, uint64_t startIndex, uint64_t endIndex, uint64_t d, uint64_t currd) {
    uint64_t numPoints = endIndex - startIndex;
    unsigned int sampleSize = (numPoints < SAMPLESIZE)? numPoints : SAMPLESIZE;
    std::vector<float> sample(sampleSize);
    std::default_random_engine eng;
    std::uniform_int_distribution<int> dist(0, numPoints);
    for(unsigned int i = 0; i < sampleSize; i++) {
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
