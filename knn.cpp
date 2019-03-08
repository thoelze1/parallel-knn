/*
 * Tanner Hoelzel
 */

#include <iostream>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <iomanip>
#include <random>
#include <vector>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
writeFile(char *filename, int *retfd, unsigned int size) {

    int fd = open(filename, O_CREAT | O_RDWR, 0777);
    if (fd < 0) {
        int en = errno;
        std::cerr << "Couldn't open " << std::string(filename) << ": " << strerror(en) << "." << std::endl;
        exit(2);
    }

    int rv = ftruncate(fd, size);
    if (rv < 0) {
        int en = errno;
        std::cerr << strerror(en) << std::endl;
        exit(2);
    }

    // Use some flags that will hopefully improve performance.
    void *vp = mmap(nullptr, size, PROT_WRITE, MAP_SHARED, fd, 0);
    if (vp == MAP_FAILED) {
        int en = errno;
        fprintf(stderr, "mmap() failed: %s\n", strerror(en));
        exit(3);
    }
    char *file_mem = (char *) vp;

    // Tell the kernel that it should evict the pages as soon as possible.
    rv = madvise(vp, size, MADV_SEQUENTIAL|MADV_WILLNEED); assert(rv == 0);

    *retfd = fd;

    return file_mem;
}


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

float
getPivot(float *points, uint64_t startIndex, uint64_t endIndex, uint64_t d, uint64_t currd) {
    uint64_t numPoints = endIndex - startIndex;
    std::vector<float> sample(SAMPLESIZE);
    std::default_random_engine eng;
    std::uniform_int_distribution<int> dist(0, numPoints);
    for(unsigned int i = 0; i < SAMPLESIZE; i++) {
        int randomIndex = dist(eng);
        sample[i] = points[(startIndex+randomIndex)*d+currd];
    }
    std::nth_element(sample.begin(), sample.begin() + sample.size()/2, sample.end());
    return sample[sample.size()/2];
}

/* see http://www.cplusplus.com/reference/algorithm/partition/ */
uint64_t
partition(float *points, uint64_t startIndex, uint64_t endIndex, uint64_t d, uint64_t currd) {
    /*
    if(endIndex - startIndex < CELLSIZE) {

    }
    while (first!=last) {
      while (pred(*first)) {
        ++first;
        if (first==last) return first;
      }
      do {
        --last;
        if (first==last) return first;
      } while (!pred(*last));
      swap (*first,*last);
      ++first;
    }
    return first;
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

    int rv, resultsFd;
    char *trainingData, *queryData, *resultsFile;
    unsigned int trainingFileSize, queryFileSize, resultsFileSize;

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

    Node *tree = buildTree(points, 0, nPoints-1, nDim, 0);

    resultsFileSize = 7*8 + nQueries*k*nDim*sizeof(float);
    resultsFile = writeFile(argv[4], &resultsFd, resultsFileSize);

    std::string results("RESULTS\0");
    results.copy(resultsFile, results.length());

    rv = munmap(trainingData, trainingFileSize); assert(rv == 0);
    rv = munmap(queryData, queryFileSize); assert(rv == 0);
    rv = munmap(resultsFile, resultsFileSize); assert(rv == 0);
    rv = close(resultsFd); assert(rv == 0);

    return 0;
}
