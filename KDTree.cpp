/*
 * Tanner Hoelzel
 */

#include "KDTree.h"

KDTree::KDTree(Node *root, float *points, uint64_t nPoints, uint64_t nDim) {
    this->root = root;
    this->points = points;
    this->nPoints = nPoints;
    this->nDim = nDim;
}
