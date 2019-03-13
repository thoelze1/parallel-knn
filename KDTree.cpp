/*
 * Tanner Hoelzel
 */

#include <iomanip>
#include <iostream>
#include <string.h>
#include <random>
#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>
#include <thread>
#include <future>

#define CELLSIZE	10
#define SAMPLESIZE	10'000

#define MAX_TRAINING_THREADS	6

#include "KDTree.h"
#include "KDNode.h"

void
constructorHelper(float *newPoints, float *oldPoints, uint32_t startIndex, uint32_t endIndex, uint32_t nDim) {
    for(int i = startIndex*nDim; i < endIndex*nDim; i++) {
        newPoints[i] = oldPoints[i];
    }
}

KDTree::KDTree(float *points, uint32_t nPoints, uint32_t nDim) {
    this->nDim = nDim;
    this->points = new float[nPoints*nDim];
    this->nPoints = nPoints;
    /*
    std::vector<std::thread> threads;
    uint64_t startIndex = 0;
    for(int i = 0; i < nCores-1; i++) {
        threads.emplace_back(constructorHelper,this->points,points,startIndex,startIndex+(nPoints/nCores),nDim);
        startIndex += nPoints/nCores;
    }
    constructorHelper(this->points, points, startIndex, nPoints, nDim);
    for(std::thread &t : threads) {
        t.join();
    }
    threads.clear();
    */
    constructorHelper(this->points,points,0,nPoints,nDim);
}

KDTree::~KDTree() {
    destroyNode(this->root);
    delete [] this->points;
}

void
KDTree::destroyNode(KDNode *node) {
    if(node->val.isLeaf == LEAF_VAL) {
        delete node;
    } else {
        destroyNode(node->left);
        destroyNode(node->right);
        delete node;
    }
}

void
KDTree::printVisitor(KDNode *node, int currD) {
    if(node->val.isLeaf == LEAF_VAL) {
        std::cout << std::setw(currD * 10) << "Leaf" << std::endl;
        for(uint32_t i = (uint32_t)((uint64_t)node->left); i < (uint32_t)((uint64_t)node->right); i++) {
            std::cout << std::setw(currD * 10 + 10) << "Point: ";
            for(int d = 0; d < nDim; d++) {
                std::cout << std::setw(10) << this->points[i*nDim+d] << "," ;
            }
            std::cout << std::endl;
        }
        return;
    }
    std::cout << std::setw(currD * 10) << "Dim: " << currD << " Med: " << node->val.median << std::endl;
    printVisitor(node->left, currD + 1);
    printVisitor(node->right, currD + 1);
}

void
KDTree::print(void) {
    printVisitor(root, 0);
}

float
KDTree::distanceToBox(float *point, float *box) {
    double distance = 0, delta;
    for(int d = 0; d < this->nDim; d++) {
        if(point[d] < box[2*d+0]) {
            delta = (double)(point[d]) - box[2*d+0];
            distance += delta * delta;
        } else if(point[d] > box[2*d+1]) {
            delta = (double)(point[d]) - box[2*d+1];
            distance += delta * delta;
        }
    }
    return (float)sqrt(distance);
}

float
KDTree::distanceToPoint(float *point1, float *point2) {
    double distance = 0, delta;
    for(int i = 0; i < this->nDim; i++) {
        delta = (double)(point1[i]) - point2[i];
        distance += delta * delta;
    }
    return (float)sqrt(distance);
}

void
KDTree::getNN(KDNode *node,
              std::priority_queue<struct pair, std::vector<struct pair>, CompareDistance> &nn,
              float *queryPoint,
              int currD) {
    // Base Case
    if(node->val.isLeaf == LEAF_VAL) {
        for(uint32_t i = (uint32_t)((uint64_t)node->left); i < (uint32_t)((uint64_t)node->right); i++) {
            float *neighbor = &(this->points[i*this->nDim]);
            float d = this->distanceToPoint(queryPoint, neighbor);
            if(d < nn.top().distance) {
                nn.pop();
                struct pair newPair;
                newPair.index = (uint32_t)i;
                newPair.distance = d;
                nn.push(newPair);
                //std::cout << neighbor[0] << "," << neighbor[1] << "\t" << d << std::endl;
            }
        }
        return;
    }
    // Determine better subtree
    KDNode *first, *second;
    if(queryPoint[currD] < node->val.median) {
         first = node->left;
         second = node->right;
    } else {
         first = node->right;
         second = node->left;
    }
    // Search better subtree
    getNN(first, nn, queryPoint, (currD+1)%this->nDim);
    //std::cout << currD << std::endl;
    // Search worse subtree if necessary
    if(nn.top().distance > std::abs(queryPoint[currD] - node->val.median)) {
        getNN(second, nn, queryPoint, (currD+1)%this->nDim);
    }
}

// use vector instead to use reserve
void
KDTree::queryHelper(float *queries, uint32_t nQueries, uint32_t k, float *out) {
    for(uint32_t queryIndex = 0; queryIndex < nQueries; queryIndex++) {
        std::priority_queue<struct pair, std::vector<struct pair>, CompareDistance> nn;
        for(int i = 0; i < k; i++) {
            struct pair newPair;
            newPair.distance = std::numeric_limits<float>::max();
            nn.push(newPair);
        }
        getNN(this->root, nn, &queries[queryIndex*this->nDim], 0);
        for(int i = 0; i < k; i++) {
            float *dest = &out[queryIndex*k*this->nDim + i*this->nDim];
            uint32_t pointsIndex = nn.top().index;
            memcpy(dest, &this->points[pointsIndex*this->nDim], this->nDim*sizeof(float));
            //std::cout << nn.top().d << std::endl;
            nn.pop();
        }
    }
}

void
KDTree::query(float *queries, uint32_t nQueries, uint32_t k, float *out, int nCores) {
    std::vector<std::thread> threads;
    if(nCores >= nQueries) {
        int i;
        for(i = 0; i < nQueries-1; i++) {
            float *subset = &queries[i*this->nDim];
            threads.emplace_back(&KDTree::queryHelper,this,subset,1,k,&out[i*k*this->nDim]);
        }
        float *subset = &queries[i*this->nDim];
        queryHelper(subset, 1, k, &out[i*k*this->nDim]);
        for(std::thread &t : threads) {
            t.join();
        }
        threads.clear();
    } else {
        int i;
        int subsetSize = nQueries/nCores;
        for(i = 0; i < nCores-1; i += subsetSize) {
            float *subset = &queries[i*this->nDim];
            threads.emplace_back(&KDTree::queryHelper,this,subset,subsetSize,k,&out[i*k*this->nDim]);
        }
        float *subset = &queries[i*this->nDim];
        queryHelper(subset, nQueries-i, k, &out[i*k*this->nDim]);
        for(std::thread &t : threads) {
            t.join();
        }
        threads.clear();
    }
    /*
    queryHelper(queries, nQueries, k, out);
    */
}

void
KDTree::getNNPruning(KDNode *node,
                     std::priority_queue<struct pair, std::vector<struct pair>, CompareDistance> &nn,
                     float *queryPoint,
                     int currD,
                     float *box,
                     bool inBox)
{
    // Base Case
    if(node->val.isLeaf == LEAF_VAL) {
        for(uint32_t i = (uint32_t)((uint64_t)node->left); i < (uint32_t)((uint64_t)node->right); i++) {
            float *neighbor = &(this->points[i*this->nDim]);
            float d = this->distanceToPoint(queryPoint, neighbor);
            if(d < nn.top().distance) {
                nn.pop();
                struct pair newPair;
                newPair.index = (uint32_t)i;
                newPair.distance = d;
                nn.push(newPair);
                //std::cout << neighbor[0] << "," << neighbor[1] << "\t" << d << std::endl;
            }
        }
        return;
    }
    // Construct new boxes
    float *leftBox = new float[2*this->nDim];
    float *rightBox = new float[2*this->nDim];
    for(int i = 0; i < 2*this->nDim; i++) {
        leftBox[i] = box[i];
        rightBox[i] = box[i];
    }
    leftBox[2*currD+1] = node->val.median;
    rightBox[2*currD+0] = node->val.median;
    float dLeft = distanceToBox(queryPoint, leftBox);
    float dRight = distanceToBox(queryPoint, rightBox);
    if(inBox) {
        // Search better subtree
        if(queryPoint[currD] < node->val.median) {
            getNNPruning(node->left, nn, queryPoint, (currD+1)%this->nDim, leftBox, true);
        } else {
            getNNPruning(node->right, nn, queryPoint, (currD+1)%this->nDim, rightBox, true);
        }
        // Search worse subtree if necessary
        if(nn.top().distance > std::abs(queryPoint[currD] - node->val.median)) {
            if(queryPoint[currD] < node->val.median) {
                getNNPruning(node->right, nn, queryPoint, (currD+1)%this->nDim, rightBox, false);
            } else {
                getNNPruning(node->left, nn, queryPoint, (currD+1)%this->nDim, leftBox, false);
            }
        }
    } else {
        if(dLeft < dRight) {
            if(nn.top().distance > dLeft) {
                getNNPruning(node->left, nn, queryPoint, (currD+1)%this->nDim, leftBox, false);
            }
            if(nn.top().distance > dRight) {
                getNNPruning(node->right, nn, queryPoint, (currD+1)%this->nDim, rightBox, false);
            }
        } else {
            if(nn.top().distance > dRight) {
                getNNPruning(node->right, nn, queryPoint, (currD+1)%this->nDim, rightBox, false);
            }
            if(nn.top().distance > dLeft) {
                getNNPruning(node->left, nn, queryPoint, (currD+1)%this->nDim, leftBox, false);
            }
        }
    }
    delete [] leftBox;
    delete [] rightBox;
}

void
KDTree::queryPruningHelper(float *queries, uint32_t nQueries, uint32_t k, float *out) {
    for(uint32_t queryIndex = 0; queryIndex < nQueries; queryIndex++) {
        std::priority_queue<struct pair, std::vector<struct pair>, CompareDistance> nn;
        for(int i = 0; i < k; i++) {
            struct pair newPair;
            newPair.distance = std::numeric_limits<float>::max();
            nn.push(newPair);
        }
        float *box = new float[2*this->nDim];
        for(int i = 0; i < this->nDim; i++) {
            box[2*i+0] = -1*std::numeric_limits<float>::max();
            box[2*i+1] = std::numeric_limits<float>::max();
        }
        getNNPruning(this->root, nn, &queries[queryIndex*this->nDim], 0, box, true);
        for(int i = 0; i < k; i++) {
            float *dest = &out[queryIndex*k*this->nDim + i*this->nDim];
            uint32_t pointsIndex = nn.top().index;
            memcpy(dest, &this->points[pointsIndex*this->nDim], this->nDim*sizeof(float));
            //std::cout << nn.top().d << std::endl;
            nn.pop();
        }
        delete [] box;
    }
}

void
KDTree::queryPruning(float *queries, uint32_t nQueries, uint32_t k, float *out, int nCores) {
    std::vector<std::thread> threads;
    if(nCores >= nQueries) {
        int i;
        for(i = 0; i < nQueries-1; i++) {
            float *subset = &queries[i*this->nDim];
            threads.emplace_back(&KDTree::queryPruningHelper,this,subset,1,k,&out[i*k*this->nDim]);
        }
        float *subset = &queries[i*this->nDim];
        queryPruningHelper(subset, 1, k, &out[i*k*this->nDim]);
        for(std::thread &t : threads) {
            t.join();
        }
        threads.clear();
    } else {
        int i;
        int subsetSize = nQueries/nCores;
        for(i = 0; i < nCores-1; i += subsetSize) {
            float *subset = &queries[i*this->nDim];
            threads.emplace_back(&KDTree::queryPruningHelper,this,subset,subsetSize,k,&out[i*k*this->nDim]);
        }
        float *subset = &queries[i*this->nDim];
        queryPruningHelper(subset, nQueries-i, k, &out[i*k*this->nDim]);
        for(std::thread &t : threads) {
            t.join();
        }
        threads.clear();
    }
    /*
    queryPruningHelper(queries, nQueries, k, out);
    */
}

// change to use SAMPLESIZE
float
KDTree::getPivot(uint32_t startIndex, uint32_t endIndex, uint32_t currd) {
    uint32_t numPoints = endIndex - startIndex;
    std::vector<float> sample(SAMPLESIZE);
    std::default_random_engine eng;
    std::uniform_int_distribution<int> dist(0, numPoints);
    for(uint32_t i = 0; i < SAMPLESIZE; i++) {
        uint32_t randomIndex = dist(eng);
        sample[i] = this->points[(startIndex+randomIndex)*this->nDim+currd];
    }
    std::nth_element(sample.begin(), sample.begin() + SAMPLESIZE/2, sample.end());
    return sample[SAMPLESIZE/2];
}

/* see http://www.cplusplus.com/reference/algorithm/partition/ */
uint32_t
KDTree::partition(uint32_t startIndex, uint32_t endIndex, uint32_t currd, float *retPivotVal) {
    int first, last;
    float pivotVal;
    uint32_t numPoints = endIndex - startIndex;
    if(numPoints < SAMPLESIZE) {
        std::vector<float> pointsVec(numPoints);
        for(uint32_t i = 0; i < numPoints; i++) {
            pointsVec[i] = this->points[(startIndex+i)*this->nDim+currd];
        }
        std::nth_element(pointsVec.begin(), pointsVec.begin() + numPoints/2, pointsVec.end());
        pivotVal = pointsVec[numPoints/2];
    } else {
        pivotVal = getPivot(startIndex, endIndex, currd);
    }
    *retPivotVal = pivotVal;
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
      /*
      for(int i = 0; i < this->nDim*sizeof(float); i++) {
        tempPoint[i] = this->points[first*this->nDim+i];
        this->points[first*this->nDim+i] = this->points[last*this->nDim+i];
        this->points[last*this->nDim+i] = tempPoint[i];
      }
      */
      memcpy(tempPoint, &this->points[first*this->nDim], this->nDim*sizeof(float));
      memcpy(&this->points[first*this->nDim], &points[last*this->nDim], this->nDim*sizeof(float));
      memcpy(&this->points[last*this->nDim], tempPoint, this->nDim*sizeof(float));
      ++first;
    }
    delete [] tempPoint;
    return first;
}

KDNode *
KDTree::buildTree(uint32_t startIndex, uint32_t endIndex, uint32_t currd) {
    if(endIndex - startIndex < CELLSIZE) {
        return new KDNode(startIndex, endIndex);
    }
    float pivotValue;
    uint32_t pivotIndex = partition(startIndex, endIndex, currd, &pivotValue);
    KDNode *node = new KDNode(pivotValue);
    node->left = buildTree(startIndex, pivotIndex, (currd+1)%this->nDim);
    node->right = buildTree(pivotIndex, endIndex, (currd+1)%this->nDim);
    return node;
}

void
KDTree::buildTreeParallel(KDNode **node, uint32_t startIndex, uint32_t endIndex, uint32_t currd) {
    if(endIndex - startIndex < CELLSIZE) {
        *node = new KDNode(startIndex, endIndex);
        return;
    }
    float pivotValue;
    uint32_t pivotIndex = partition(startIndex, endIndex, currd, &pivotValue);
    KDNode *newNode = new KDNode(pivotValue);
    int numAvail = (this->threadPool)--;
    if(numAvail > 0) {
        std::thread t(&KDTree::buildTreeParallel, this, &(newNode->left), startIndex, pivotIndex, (currd+1)%this->nDim);
        buildTreeParallel(&(newNode->right), pivotIndex, endIndex, (currd+1)%this->nDim);
        t.join();
        (this->threadPool)++;
    } else {
        (this->threadPool)++;
        buildTreeParallel(&(newNode->left), startIndex, pivotIndex, (currd+1)%this->nDim);
        buildTreeParallel(&(newNode->right), pivotIndex, endIndex, (currd+1)%this->nDim);
    }
    *node = newNode;
}

void
KDTree::train(int nCores) {
    int n = std::thread::hardware_concurrency();
    this->threadPool = std::min(nCores-1,n-1);
    buildTreeParallel(&(this->root), 0, nPoints, 0);
}
