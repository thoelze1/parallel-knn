/*
 * Tanner Hoelzel
 */

#include <string.h>
#include <random>
#include <vector>
#include <algorithm>

#define CELLSIZE	8
#define SAMPLESIZE	10'000

#include "KDTree.h"
#include "KDNode.h"

KDTree::KDTree(KDNode *root, float *points, uint64_t nPoints, uint64_t nDim) {
    this->root = root;
    this->points = points;
    this->nPoints = nPoints;
    this->nDim = nDim;
}

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
KDNode *
buildTree(float *points, uint64_t startIndex, uint64_t endIndex, uint64_t d, uint64_t currd) {
    if(endIndex - startIndex < CELLSIZE) {
        return new KDNode(startIndex, endIndex);
    }
    uint64_t pivotIndex = partition(points, startIndex, endIndex, d, currd);
    uint64_t pivotValue = points[pivotIndex*d+currd];
    KDNode *node = new KDNode(startIndex, endIndex, pivotValue, d);
    node->left = buildTree(points, startIndex, pivotIndex, d, currd+1%d);
    node->right = buildTree(points, pivotIndex, endIndex, d, currd+1%d);
    return node;
}

KDTree *
buildTree(float *points, uint64_t nPoints, uint64_t nDim) {
    float *newPoints = new float[nPoints*nDim];
    for(int i = 0; i < nPoints*nDim; i++) {
        newPoints[i] = points[i];
    }
    KDNode *root = buildTree(newPoints, 0, nPoints, nDim, 0);
    return new KDTree(root, newPoints, nPoints, nDim);
}
