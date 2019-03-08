/*
 * Tanner Hoelzel
 */

char *
writeFile(char *filename, int *retfd, unsigned int size);

char *
readFile(char *filename, int *retfd, unsigned int *size);

int
closeFile(char *mmap, int fd, unsigned int size);
