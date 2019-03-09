/*
 * Tanner Hoelzel
 */

#ifndef _KDNODE_H_
#define _KDNODE_H_

#include <cstdint>

class KDNode {
    public:
        KDNode(){}
        KDNode(uint64_t start, uint64_t end);
        KDNode(uint64_t start, uint64_t end, float med, unsigned int d);
        uint64_t startIndex, endIndex;
        float median;
        unsigned int dim;
        KDNode *left;
        KDNode *right;
    private:
};

#endif
