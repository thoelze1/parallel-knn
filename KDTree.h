/*
 * Tanner Hoelzel
 */

#ifndef _KDTREE_H_
#define _KDTREE_H_

#include <cstdint>

#include "KDNode.h"

class KDTree {
    public:
        KDTree(){}
        KDTree(float *points, uint64_t nPoints, uint64_t nDim);
        ~KDTree();
        void query(float *queries, uint64_t nQueries, uint64_t k, float *out);
    private:
        KDNode *root;
        float *points;
        uint64_t nPoints, nDim;
        float getPivot(uint64_t startIndex, uint64_t endIndex, uint64_t currd);
        uint64_t partition(uint64_t startIndex, uint64_t endIndex, uint64_t currd);
        KDNode *buildTree(uint64_t startIndex, uint64_t endIndex, uint64_t currd);
        void destroyNode(KDNode* node);
};

#endif
