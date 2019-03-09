/*
 * Tanner Hoelzel
 */

#include <string.h>
#include <random>
#include <vector>
#include <algorithm>
#include <limits>

#define CELLSIZE	10
#define SAMPLESIZE	10'000

#include "KDTree.h"
#include "KDNode.h"

struct CompareDistance {
    bool operator()(const union pair &lhs, const union pair &rhs) {
        return lhs.d < rhs.d;
    }
};

KDTree::KDTree(float *points, uint64_t nPoints, uint64_t nDim) {
    float *newPoints = new float[nPoints*nDim];
    for(int i = 0; i < nPoints*nDim; i++) {
        newPoints[i] = points[i];
    }
    this->points = newPoints;
    this->nPoints = nPoints;
    this->nDim = nDim;
    this->root = buildTree(0, nPoints, 0);
}

KDTree::~KDTree(void) {
    delete [] this->points;
    destroyNode(this->root);
}

void
KDTree::destroyNode(KDNode *node) {
    if(node->isLeaf) {
        delete node;
    } else {
        destroyNode(node->left);
        destroyNode(node->right);
        delete node;
    }
}

void
KDTree::getNN(KDNode *node, std::vector<union pair> &nn, float *cube, float *queryPoint, uint64_t k, bool inCube) {

}

void
KDTree::query(float *queries, uint64_t nQueries, uint64_t k, float *out) {
    float *cube = new float[this->nDim*2];
    for(int i = 0; i < this->nDim; i++) {
        cube[2*i+0] = std::numeric_limits<float>::min();
        cube[2*i+1] = std::numeric_limits<float>::max();
    }
    std::vector<union pair> nn;
    nn.reserve(k);
    for(uint64_t queryIndex = 0; queryIndex < nQueries; queryIndex++) {
        for(int i = 0; i < k; i++) {
            nn[i].d = std::numeric_limits<float>::max();
        }
        getNN(this->root, nn, cube, &queries[queryIndex*this->nDim], k, true);
        for(int i = 0; i < k; i++) {
            float *dest = out + queryIndex*k*this->nDim + i*this->nDim;
            uint32_t pointsIndex = nn[i].i >> 32;
            memcpy(dest, &this->points[pointsIndex], this->nDim*sizeof(float));
        }
    }
    delete [] cube;
}

// change to use SAMPLESIZE
float
KDTree::getPivot(uint64_t startIndex, uint64_t endIndex, uint64_t currd) {
    uint64_t numPoints = endIndex - startIndex;
    std::vector<float> sample(SAMPLESIZE);
    std::default_random_engine eng;
    std::uniform_int_distribution<int> dist(0, numPoints);
    for(unsigned int i = 0; i < SAMPLESIZE; i++) {
        int randomIndex = dist(eng);
        sample[i] = this->points[(startIndex+randomIndex)*this->nDim+currd];
    }
    std::nth_element(sample.begin(), sample.begin() + SAMPLESIZE/2, sample.end());
    return sample[SAMPLESIZE/2];
}

/* see http://www.cplusplus.com/reference/algorithm/partition/ */
uint64_t
KDTree::partition(uint64_t startIndex, uint64_t endIndex, uint64_t currd) {
    int first, last;
    float pivotVal;
    uint64_t numPoints = endIndex - startIndex;
    if(numPoints < SAMPLESIZE) {
        std::vector<float> pointsVec(numPoints);
        for(unsigned int i = 0; i < numPoints; i++) {
            pointsVec[i] = this->points[(startIndex+i)*this->nDim+currd];
        }
        std::nth_element(pointsVec.begin(), pointsVec.begin() + numPoints/2, pointsVec.end());
        pivotVal = pointsVec[numPoints/2];
    } else {
        pivotVal = getPivot(startIndex, endIndex, currd);
    }
    float *tempPoint = new float[this->nDim];
    first = startIndex;
    last = endIndex;
    while (first != last) {
      while (this->points[first*this->nDim+currd] < pivotVal) {
        ++first;
        if (first == last) return first;
      }
      do {
        --last;
        if (first == last) return first;
      } while (this->points[last*this->nDim+currd] >= pivotVal);
      memcpy(tempPoint, &this->points[first*this->nDim], this->nDim*sizeof(float));
      memcpy(&this->points[first*this->nDim], &points[last*this->nDim], this->nDim*sizeof(float));
      memcpy(&this->points[last*this->nDim], tempPoint, this->nDim*sizeof(float));
      ++first;
    }
    delete [] tempPoint;
    return first;
}

// Make Leaf class?
KDNode *
KDTree::buildTree(uint64_t startIndex, uint64_t endIndex, uint64_t currd) {
    if(endIndex - startIndex < CELLSIZE) {
        return new KDNode(startIndex, endIndex);
    }
    uint64_t pivotIndex = partition(startIndex, endIndex, currd);
    uint64_t pivotValue = this->points[pivotIndex*this->nDim+currd];
    KDNode *node = new KDNode(pivotValue);
    node->left = buildTree(startIndex, pivotIndex, currd+1%this->nDim);
    node->right = buildTree(pivotIndex, endIndex, currd+1%this->nDim);
    return node;
}
