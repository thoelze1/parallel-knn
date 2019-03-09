/*
 * Tanner Hoelzel
 */

#include <iostream>
#include <assert.h>

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

    Node *tree = buildTree(newPoints, 0, nPoints, nDim, 0);

    resultsDataSize = 7*8 + nQueries*k*nDim*sizeof(float);
    resultsData= writeFile(argv[4], &resultsFd, resultsDataSize);

    std::string results("RESULTS\0");
    results.copy(resultsData, results.length());
    *(uint64_t *)(resultsData+8) = trainingFileId;
    *(uint64_t *)(resultsData+16) = queryFileId;
    //*(uint64_t *)(resultsData+24) = from /dev/urandom
    *(uint64_t *)(resultsData+32) = nQueries;
    *(uint64_t *)(resultsData+40) = nDim;
    *(uint64_t *)(resultsData+48) = k;

    rv = closeFile(resultsData, resultsFd, resultsDataSize);

    return 0;
}
