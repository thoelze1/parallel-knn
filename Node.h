/*
 * Tanner Hoelzel
 */

#include <cstdint>

class Node {
    public:
        Node(){}
        Node(uint64_t start, uint64_t end);
        Node(uint64_t start, uint64_t end, float med, unsigned int d);
        uint64_t startIndex, endIndex;
        float median;
        unsigned int dim;
        Node *left;
        Node *right;
    private:
};
