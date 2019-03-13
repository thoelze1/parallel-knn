/*
 * Tanner Hoelzel
 */

#ifndef _KDTREE_H_
#define _KDTREE_H_

#include <queue>
#include <atomic>
#include <cstdint>

#include "KDNode.h"

struct pair {
    float distance;
    uint32_t index;
};

struct CompareDistance {
    bool operator()(const struct pair &lhs, const struct pair &rhs) {
        return lhs.distance < rhs.distance;
    }
};

class KDTree {
public:
    KDTree(float *points, uint32_t nPoints, uint32_t nDim);
    ~KDTree();
    void train(int nCores);
    void query(float *queries, uint32_t nQueries, uint32_t k, float *out, int nCores);
    void print(void);
private:
    /* Vars */
    KDNode *root;
    float *points;
    uint32_t nPoints, nDim;
    std::atomic<int> threadPool;
    /* Querying */
    float distanceToBox(float *point, float *box);
    float distanceToPoint(float *point1, float *point2);
    void queryHelper(float *queries, uint32_t nQueries, uint32_t k, float *out);
    void getNN(KDNode *node,
               std::priority_queue<struct pair, std::vector<struct pair>, CompareDistance> &nn,
               float *queryPoint,
               int currD,
               float *box,
               bool inBox);
    /* Printing */
    void printVisitor(KDNode *node, int currD);
    /* Training */
    float getPivot(uint32_t startIndex, uint32_t endIndex, uint32_t currd);
    uint32_t partition(uint32_t startIndex, uint32_t endIndex, uint32_t currd, float *pivotVal);
    void buildTreeParallel(KDNode **node, uint32_t startIndex, uint32_t endIndex, uint32_t currd);
    KDNode *buildTree(uint32_t startIndex, uint32_t endIndex, uint32_t currd);
    /* Destructor */
    void destroyNode(KDNode* node);
};

#endif
