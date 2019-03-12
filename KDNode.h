/*
 * Tanner Hoelzel
 */

#ifndef _KDNODE_H_
#define _KDNODE_H_

#include <cstdint>

#define LEAF_VAL 0xFFFFFFFF

union nodeVal {
    float median;
    uint32_t isLeaf;
};

class KDNode {
public:
    KDNode(){}
    KDNode(float median);
    KDNode(uint64_t start, uint64_t end);
    union nodeVal val;
    KDNode *left;
    KDNode *right;
private:
};

#endif
