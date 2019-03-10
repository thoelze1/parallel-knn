/*
 * Tanner Hoelzel
 */

#include <iostream>
#include <string.h>
#include <random>
#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>

#define CELLSIZE	10
#define SAMPLESIZE	10'000

#include "KDTree.h"
#include "KDNode.h"

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

float
KDTree::distanceToPoint(float *point1, float *point2) {
    double distance = 0;
    for(int i = 0; i < this->nDim; i++) {
        double delta = (double)(point1[i]) - point2[i];
        distance += delta * delta;
    }
    return (float)sqrt(distance);
}

void
KDTree::getNN(KDNode *node,
              std::priority_queue<union pair, std::vector<union pair>, CompareDistance> &nn,
              float *queryPoint,
              int currD) {
    // Base Case
    if(node->isLeaf) {
        for(uint64_t i = (uint64_t)node->left; i < (uint64_t)node->right; i++) {
            float *neighbor = &(this->points[i*this->nDim]);
            float d = this->distanceToPoint(queryPoint, neighbor);
            if(d < nn.top().d) {
                nn.pop();
                union pair newPair;
                newPair.i = i << 32;
                newPair.d = d;
                nn.push(newPair);
            }
        }
        return;
    }
    // Determine better subtree
    KDNode *first, *second;
    if(queryPoint[currD] < node->median) {
         first = node->left;
         second = node->right;
    } else {
         first = node->left;
         second = node->right;
    }
    // Search better subtree
    getNN(first, nn, queryPoint, currD+1%this->nDim);
    // Search worse subtree if necessary
    if(nn.top().d > abs(queryPoint[currD] - node->median)) {
        getNN(second, nn, queryPoint, currD+1%this->nDim);
    }
}

// use vector instead to use reserve
void
KDTree::query(float *queries, uint64_t nQueries, uint64_t k, float *out) {
    for(uint64_t queryIndex = 0; queryIndex < nQueries; queryIndex++) {
        std::priority_queue<union pair, std::vector<union pair>, CompareDistance> nn;
        for(int i = 0; i < k; i++) {
            union pair newPair;
            //newPair.i = (long)i << 32;
            newPair.d = std::numeric_limits<float>::max();
            nn.push(newPair);
        }
        getNN(this->root, nn, &queries[queryIndex*this->nDim], 0);
        for(int i = 0; i < k; i++) {
            float *dest = &out[queryIndex*k*this->nDim + i*this->nDim];
            uint32_t pointsIndex = nn.top().i >> 32;
            std::cout << pointsIndex << std::endl;
            memcpy(dest, &this->points[pointsIndex*this->nDim], this->nDim*sizeof(float));
            nn.pop();
        }
    }
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

KDNode *
KDTree::buildTree(uint64_t startIndex, uint64_t endIndex, uint64_t currd) {
    if(endIndex - startIndex < CELLSIZE) {
        for(uint64_t i = startIndex; i < endIndex; i++) {
            for(int d = 0; d < this->nDim; d++) {
                std::cout << points[i*this->nDim+d] << std::endl;
             }
        }
        return new KDNode(startIndex, endIndex);
    }
    uint64_t pivotIndex = partition(startIndex, endIndex, currd);
    uint64_t pivotValue = this->points[pivotIndex*this->nDim+currd];
    KDNode *node = new KDNode(pivotValue);
    node->left = buildTree(startIndex, pivotIndex, currd+1%this->nDim);
    node->right = buildTree(pivotIndex, endIndex, currd+1%this->nDim);
    return node;
}

/*
bool
prunable(float *point, float *box, float threshold) {
    float *closestCorner = new float[this->nDim];
    double distance = 0, delta;
    for(int d = 0; d < this->nDim; d++) {
        if(abs(point[i] - box[2*d+1]) < abs(point[i] - box[2*d+2])) {
            closestCorner[d] = box[2*d+1];
        } else {
            closestCorner[d] = box[2*d+2];
        }
        delta = (double)(closestCorner[d]) - point[d];
        distance += delta*delta;
        if(sqrt(distance) > threshold) {
            return true;
        }
    }
    return false;
}
*/
