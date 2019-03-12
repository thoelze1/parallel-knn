/*
 * Tanner Hoelzel
 */

#include "KDNode.h"

KDNode::KDNode(float median) {
    this->val.median = median;
}

KDNode::KDNode(uint64_t start, uint64_t end) {
    this->left = (KDNode *)start;
    this->right = (KDNode *)end;
    this->val.isLeaf = LEAF_VAL;
}
