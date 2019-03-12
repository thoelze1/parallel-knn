/*
 * Tanner Hoelzel
 */

#ifndef _KDTREE_H_
#define _KDTREE_H_

#include <cstdint>

#include "KDNode.h"
#include <queue>

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
    KDTree(float *points, uint64_t nPoints, uint64_t nDim, int nCores);
    ~KDTree();
    void query(float *queries, uint64_t nQueries, uint64_t k, float *out, int nCores);
    void print(void);
private:
    KDNode *root;
    float *points;
    uint64_t nPoints, nDim;
    float distanceToPoint(float *point1, float *point2);
    void queryHelper(float *queries, uint64_t nQueries, uint64_t k, float *out);
    void getNN(KDNode *node,
               std::priority_queue<struct pair, std::vector<struct pair>, CompareDistance> &nn,
               float *queryPoint,
               int currD);
    void printVisitor(KDNode *node, int currD);
    float getPivot(uint64_t startIndex, uint64_t endIndex, uint64_t currd);
    uint64_t partition(uint64_t startIndex, uint64_t endIndex, uint64_t currd, float *pivotVal);
    void buildTree(KDNode **node, uint64_t startIndex, uint64_t endIndex, uint64_t currd, int nCores);
    void destroyNode(KDNode* node);
};

#endif
