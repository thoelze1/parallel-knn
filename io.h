/*
 * Tanner Hoelzel
 */

#ifndef _IO_H_
#define _IO_H_

struct trainingHeader {
    char typeString[8];
    uint64_t id;
    uint64_t nPoints;
    uint64_t nDims;
};

struct queryHeader {
    char typeString[8];
    uint64_t id;
    uint64_t nQueries;
    uint64_t nDims;
    uint64_t k;
};

struct resultHeader {
    char typeString[8];
    uint64_t trainingId;
    uint64_t queryId;
    uint64_t id;
    uint64_t nQueries;
    uint64_t nDims;
    uint64_t k;
};

uint64_t
getUniqueId(void);

char *
writeFile(char *filename, int *retfd, unsigned int size);

char *
readFile(char *filename, int *retfd, unsigned int *size);

int
closeFile(char *mmap, int fd, unsigned int size);

#endif
