/*
 * Tanner Hoelzel
 */

#include <iostream>
#include <assert.h>
#include <sys/resource.h>

#include "io.h"
#include "train.h"

int
main(int argc, char **argv) {

    int rv, trainingFd, queryFd, resultsFd;
    char *trainingData, *queryData, *resultsData;
    unsigned int trainingFileSize, queryFileSize, resultsDataSize;

    trainingData = readFile(argv[2], &trainingFd, &trainingFileSize);
    queryData = readFile(argv[3], &queryFd, &queryFileSize);

    uint64_t trainingFileId = *(uint64_t *)(trainingData+8);
    uint64_t nPoints = *(uint64_t *)(trainingData+16);
    uint64_t nDim = *(uint64_t *)(trainingData+24);
    float *points = (float *)(trainingData+32);

    uint64_t queryFileId = *(uint64_t *)(queryData+8);
    uint64_t nQueries = *(uint64_t *)(queryData+16);
    uint64_t k = *(uint64_t *)(queryData+32);
    float *queries = (float *)(queryData+40);

    float *newPoints = new float[nPoints*nDim];
    for(int i = 0; i < nPoints*nDim; i++) {
        newPoints[i] = points[i];
    }

    float *newQueries = new float[nQueries*nDim];
    for(int i = 0; i < nQueries*nDim; i++) {
        newQueries[i] = queries[i];
    }

    rv = closeFile(trainingData, trainingFd, trainingFileSize);
    rv = closeFile(queryData, queryFd, queryFileSize);

    KDTree *tree = buildTree(newPoints, nPoints, nDim);

    resultsDataSize = 7*8 + nQueries*k*nDim*sizeof(float);
    resultsData= writeFile(argv[4], &resultsFd, resultsDataSize);

    std::string results("RESULTS\0");
    results.copy(resultsData, results.length());
    *(uint64_t *)(resultsData+8) = trainingFileId;
    *(uint64_t *)(resultsData+16) = queryFileId;
    *(uint64_t *)(resultsData+24) = getUniqueId();
    *(uint64_t *)(resultsData+32) = nQueries;
    *(uint64_t *)(resultsData+40) = nDim;
    *(uint64_t *)(resultsData+48) = k;

    //query((float *)(resultsData+48), newQueries, newPoints,

    rv = closeFile(resultsData, resultsFd, resultsDataSize);

    struct rusage ru;
    rv = getrusage(RUSAGE_SELF, &ru); assert(rv == 0);
    auto cv = [](const timeval &tv) {
        return double(tv.tv_sec) + double(tv.tv_usec)/1000000;
    };

    std::cerr << "Resource Usage:\n";
    std::cerr << "    User CPU Time: " << cv(ru.ru_utime) << '\n';
    std::cerr << "    Sys CPU Time: " << cv(ru.ru_stime) << '\n'; 
    std::cerr << "    Max Resident: " << ru.ru_maxrss << '\n';
    std::cerr << "    Page Faults: " << ru.ru_majflt << '\n';

    return 0;
}
