/*
 * Tanner Hoelzel
 */

#include <iostream>
#include <assert.h>
#include <sys/resource.h>

extern "C" {
#include "io.h"
}
#include "KDTree.h"

void printUsage(void) {
    struct rusage ru;
    int rv = getrusage(RUSAGE_SELF, &ru); assert(rv == 0);
    auto cv = [](const timeval &tv) {
        return double(tv.tv_sec) + double(tv.tv_usec)/1000000;
    };
    std::cerr << "Resource Usage:\n";
    std::cerr << "    User CPU Time: " << cv(ru.ru_utime) << '\n';
    std::cerr << "    Sys CPU Time: " << cv(ru.ru_stime) << '\n'; 
    std::cerr << "    Max Resident: " << ru.ru_maxrss << '\n';
    std::cerr << "    Page Faults: " << ru.ru_majflt << '\n';
}

int
main(int argc, char **argv) {

    int rv, trainingFd, queryFd, resultFd;
    unsigned int trainingFileSize, queryFileSize;

    int numCores = std::atoi(argv[1]);
    struct trainingHeader *tHeader = (struct trainingHeader *)readFile(argv[2], &trainingFd, &trainingFileSize);
    struct queryHeader *qHeader = (struct queryHeader *)readFile(argv[3], &queryFd, &queryFileSize);

    int resultFileSize = sizeof(struct resultHeader) + qHeader->nQueries*qHeader->k*qHeader->nDims*sizeof(float);
    struct resultHeader *rHeader = (struct resultHeader *)writeFile(argv[4], &resultFd, resultFileSize);

    std::string result("RESULT\0\0");
    result.copy((char *)&(rHeader->typeString), result.length());
    rHeader->trainingId = tHeader->id;
    rHeader->queryId = qHeader->id;
    rHeader->id = getUniqueId();
    rHeader->nQueries = qHeader->nQueries;
    rHeader->nDims = qHeader->nDims;
    rHeader->k = qHeader->k;

    KDTree tree((float *)(tHeader+1), tHeader->nPoints, tHeader->nDims);

    tree.train(numCores);
    tree.query((float *)(qHeader+1), qHeader->nQueries, qHeader->k, (float *)(rHeader+1), numCores);

    rv = closeFile((char *)tHeader, trainingFd, trainingFileSize);
    rv = closeFile((char *)qHeader, queryFd, queryFileSize);
    rv = closeFile((char *)rHeader, resultFd, resultFileSize);

    return 0;
}
