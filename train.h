/*
 * Tanner Hoelzel
 */

#ifndef _TRAIN_H_
#define _TRAIN_H_

#include "Node.h"

Node *
buildTree(float *points, uint64_t startIndex, uint64_t endIndex, uint64_t d, uint64_t currd);

#endif
