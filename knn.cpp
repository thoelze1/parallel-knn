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
    //assert(*(uint64_t *)queryData+3 == nDim);
    uint64_t k = *(uint64_t *)(queryData+32);
    float *queries = (float *)(queryData+40);

    std::cout << trainingFileId << std::endl;
    std::cout << nPoints << std::endl;
    std::cout << nDim << std::endl;
    std::cout << queryFileId << std::endl;
    std::cout << nQueries<< std::endl;
    std::cout << k << std::endl;

    rv = munmap(trainingData, trainingFileSize); assert(rv == 0);
}
