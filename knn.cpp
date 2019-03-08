/*
 * Tanner Hoelzel
 */

#include <iostream>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <iomanip>
#include <random>
#include <vector>

#define CELLSIZE	1'000'000'000
#define SAMPLESIZE	10'000

class Node {
    public:
        Node(uint64_t start, uint64_t end) {
            startIndex = start;
            endIndex = end;
        }
        Node(uint64_t start, uint64_t end, float med, unsigned int d) {
            startIndex = start;
            endIndex = end;
            median = med;
            dim = d;
        }
        uint64_t startIndex, endIndex;
        float median;
        unsigned int dim;
        Node *left;
        Node *right;
    private:
};

char *
readFile(char *filename, unsigned int *size) {

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        int en = errno;
        std::cerr << "Couldn't open " << std::string(filename) << ": " << strerror(en) << "." << std::endl;
        exit(2);
    }

    struct stat sb;
    int rv = fstat(fd, &sb); assert(rv == 0);

    // Use some flags that will hopefully improve performance.
    void *vp = mmap(nullptr, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (vp == MAP_FAILED) {
        int en = errno;
        fprintf(stderr, "mmap() failed: %s\n", strerror(en));
        exit(3);
    }
    char *file_mem = (char *) vp;

    // Tell the kernel that it should evict the pages as soon as possible.
    rv = madvise(vp, sb.st_size, MADV_SEQUENTIAL|MADV_WILLNEED); assert(rv == 0);

    rv = close(fd); assert(rv == 0);

    *size = sb.st_size;

    return file_mem;
}

std::vector<float> *
randomSample(float *points, uint64_t startIndex, uint64_t endIndex, uint64_t d, uint64_t currd) {
    /*
    uint64_t numPoints = endIndex - startIndex / d;
    std::vector<float> *sample = new std::vector<float>(SAMPLESIZE);
    std::default_random_engine eng;
    std::uniform_int_distribution<int> dist(0, numPoints);
    for(unsigned int i = 0; i < SAMPLESIZE; i++) {
        int randomIndex = dist(eng);
        *sample[i] = points[startIndex + currd + d*randomIndex];
    }
    return sample;
    */
    return new std::vector<float>;
}

uint64_t
partition(float *points, uint64_t startIndex, uint64_t endIndex, uint64_t d, uint64_t currd) {
    /*
    if(endIndex - startIndex < CELLSIZE) {

    }
    */
    return 0;
}

// Make Leaf class?
Node *
buildTree(float *points, uint64_t startIndex, uint64_t endIndex, uint64_t d, uint64_t currd) {
    if(endIndex - startIndex < CELLSIZE) {
        return new Node(startIndex, endIndex);
    }
    uint64_t pivotIndex = partition(points, startIndex, endIndex, d, currd);
    uint64_t pivotValue = points[pivotIndex*d+currd];
    Node *node = new Node(startIndex, endIndex, pivotValue, d);
    node->left = buildTree(points, startIndex, pivotIndex-1, d, currd+1%d);
    node->right = buildTree(points, pivotIndex, endIndex, d, currd+1%d);
}

int
main(int argc, char **argv) {

    int rv;
    char *trainingData, *queryData;
    unsigned int trainingFileSize, queryFileSize;

    trainingData = readFile(argv[2], &trainingFileSize);
    queryData = readFile(argv[3], &queryFileSize);

    uint64_t trainingFileId = *(uint64_t *)(trainingData+8);
    uint64_t nPoints = *(uint64_t *)(trainingData+16);
    uint64_t nDim = *(uint64_t *)(trainingData+24);
    float *points = (float *)(trainingData+32);

    uint64_t queryFileId = *(uint64_t *)(queryData+8);
    uint64_t nQueries = *(uint64_t *)(queryData+16);
    uint64_t k = *(uint64_t *)(queryData+32);
    float *queries = (float *)(queryData+40);

    std::cout << trainingFileId << std::endl;
    std::cout << nPoints << std::endl;
    std::cout << nDim << std::endl;
    std::cout << queryFileId << std::endl;
    std::cout << nQueries<< std::endl;
    std::cout << k << std::endl;

    Node *tree = buildTree(points, 0, nPoints-1, nDim, 0);

    rv = munmap(trainingData, trainingFileSize); assert(rv == 0);
    rv = munmap(queryData, queryFileSize); assert(rv == 0);
}
