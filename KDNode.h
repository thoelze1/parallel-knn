/*
 * Tanner Hoelzel
 */

#ifndef _KDNODE_H_
#define _KDNODE_H_

#include <cstdint>

class KDNode {
    public:
        KDNode(){}
        KDNode(float med);
        KDNode(uint64_t start, uint64_t end);
        float median;
        bool isLeaf;
        KDNode *left;
        KDNode *right;
    private:
};

#endif
