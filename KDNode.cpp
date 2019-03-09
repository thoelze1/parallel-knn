/*
 * Tanner Hoelzel
 */

#include "KDNode.h"

KDNode::KDNode(float med) {
    median = med;
}

KDNode::KDNode(uint64_t start, uint64_t end) {
    left = (KDNode *)start;
    right = (KDNode *)end;
}
