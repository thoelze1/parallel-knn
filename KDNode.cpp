/*
 * Tanner Hoelzel
 */

#include "KDNode.h"

KDNode::KDNode(float median) {
    this->val.median = median;
}

KDNode::KDNode(uint32_t start, uint32_t end) {
    this->left = (KDNode *)((uint64_t)start);
    this->right = (KDNode *)((uint64_t)end);
    this->val.isLeaf = LEAF_VAL;
}
