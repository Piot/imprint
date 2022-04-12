#ifndef IMPRINT_UTILS_H
#define IMPRINT_UTILS_H

#include <stddef.h>

char* imprintSizeToString(char *buf, size_t bufSize, size_t size);
char* imprintSizeAndPercentageToString(char *buf, size_t bufSize, size_t size, size_t maximumSize);

#endif
