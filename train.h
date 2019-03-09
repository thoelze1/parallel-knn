/*
 * Tanner Hoelzel
 */

#ifndef _TRAIN_H_
#define _TRAIN_H_

#include "KDTree.h"

KDTree *
buildTree(float *points, uint64_t nPoints, uint64_t nDim);

#endif
