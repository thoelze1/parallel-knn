/*
 * Tanner Hoelzel
 */

#include "Node.h"

Node::Node(uint64_t start, uint64_t end) {
    startIndex = start;
    endIndex = end;
}

Node::Node(uint64_t start, uint64_t end, float med, unsigned int d) {
    startIndex = start;
    endIndex = end;
    median = med;
    dim = d;
}
