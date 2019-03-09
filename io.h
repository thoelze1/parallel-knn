/*
 * Tanner Hoelzel
 */

#ifndef _IO_H_
#define _IO_H_

uint64_t
getUniqueId(void);

char *
writeFile(char *filename, int *retfd, unsigned int size);

char *
readFile(char *filename, int *retfd, unsigned int *size);

int
closeFile(char *mmap, int fd, unsigned int size);

#endif
