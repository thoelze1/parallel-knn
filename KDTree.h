/*
 * Tanner Hoelzel
 */

#ifndef _KDTREE_H_
#define _KDTREE_H_

#include <cstdint>

#include "Node.h"

class KDTree {
    public:
        KDTree(){}
        KDTree(Node *root, float *points, uint64_t nPoints, uint64_t nDim);
        Node *root;
        float *points;
        uint64_t nPoints, nDim;
    private:
};

#endif
