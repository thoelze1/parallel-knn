/*
 * Tanner Hoelzel
 */

#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

uint64_t
getUniqueId(void) {
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        int en = errno;
        fprintf(stderr, "Couldn't open /dev/urandom: %s\n", strerror(en));
        exit(2);
    }
    uint64_t uniqueId;
    int rv = read(fd, &uniqueId, sizeof(uniqueId)); assert(rv == sizeof(uniqueId));
    rv = close(fd); assert(rv == 0);
    return uniqueId;
}

int
closeFile(char *mmap, int fd, unsigned int size) {
    int rv;
    rv = munmap(mmap, size); assert(rv == 0);
    rv = close(fd); assert(rv == 0);
}

char *
writeFile(char *filename, int *retfd, unsigned int size) {

    int fd = open(filename, O_CREAT | O_RDWR, 0600);
    if (fd < 0) {
        int en = errno;
        fprintf(stderr, "Couldn't open %s: %s\n", filename, strerror(en));
        exit(2);
    }

    int rv = ftruncate(fd, size);
    if (rv < 0) {
        int en = errno;
        fprintf(stderr, "%s\n", strerror(en));
        exit(2);
    }

    // Use some flags that will hopefully improve performance.
    void *vp = mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, 0);
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
readFile(char *filename, int *retfd, unsigned int *size) {

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        int en = errno;
        fprintf(stderr, "Couldn't open %s: %s\n", filename, strerror(en));
        exit(2);
    }

    struct stat sb;
    int rv = fstat(fd, &sb); assert(rv == 0);

    // Use some flags that will hopefully improve performance.
    void *vp = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (vp == MAP_FAILED) {
        int en = errno;
        fprintf(stderr, "mmap() failed: %s\n", strerror(en));
        exit(3);
    }
    char *file_mem = (char *) vp;

    // Tell the kernel that it should evict the pages as soon as possible.
    rv = madvise(vp, sb.st_size, MADV_SEQUENTIAL|MADV_WILLNEED); assert(rv == 0);

    *retfd = fd;

    *size = sb.st_size;

    return file_mem;
}
