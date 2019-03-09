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
        KDNode *root;
        float *points;
        uint64_t nPoints, nDim;
    private:
};

#endif
