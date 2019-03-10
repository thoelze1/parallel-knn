/*
 * Tanner Hoelzel
 */

#ifndef _KDTREE_H_
#define _KDTREE_H_

#include <cstdint>

#include "KDNode.h"
#include <queue>

union pair {
    float d;
    uint64_t i;
};

struct CompareDistance {
    bool operator()(const union pair &lhs, const union pair &rhs) {
        return lhs.d < rhs.d;
    }
};

class KDTree {
public:
    KDTree(float *points, uint64_t nPoints, uint64_t nDim);
    ~KDTree();
    void query(float *queries, uint64_t nQueries, uint64_t k, float *out);
private:
    KDNode *root;
    float *points;
    uint64_t nPoints, nDim;
    float distanceToPoint(float *point1, float *point2);
    void getNN(KDNode *node,
               std::priority_queue<union pair, std::vector<union pair>, CompareDistance> &nn,
               float *queryPoint,
               int currD);
    float getPivot(uint64_t startIndex, uint64_t endIndex, uint64_t currd);
    uint64_t partition(uint64_t startIndex, uint64_t endIndex, uint64_t currd);
    KDNode *buildTree(uint64_t startIndex, uint64_t endIndex, uint64_t currd);
    void destroyNode(KDNode* node);
};

#endif
