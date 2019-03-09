/*
 * Tanner Hoelzel
 */

#include "KDNode.h"

KDNode::KDNode(float med) {
    this->median = med;
    this->isLeaf = false;
}

KDNode::KDNode(uint64_t start, uint64_t end) {
    this->left = (KDNode *)start;
    this->right = (KDNode *)end;
    this->isLeaf = true;
}
