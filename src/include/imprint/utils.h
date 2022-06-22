/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef IMPRINT_UTILS_H
#define IMPRINT_UTILS_H

#include <stddef.h>

char* imprintSizeToString(char* buf, size_t bufSize, size_t size);
const char* imprintSizeToStringStatic(size_t size);
char* imprintSizeToStringEx(char* buf, size_t bufSize, size_t size, int flags);
const char* imprintSizeToStringExStatic(size_t size, int flags);
char* imprintSizeAndPercentageToString(char* buf, size_t bufSize, size_t size, size_t maximumSize);

#endif
