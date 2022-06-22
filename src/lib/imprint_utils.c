/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/clog.h>
#include <imprint/utils.h>
#include <tiny-libc/tiny_libc.h>

char* imprintSizeToString(char* buf, size_t bufSize, size_t size)
{
    const int KILOBYTE = 1024;
    const int MEGABYTE = 1024 * KILOBYTE;
    int factor;
    char* suffix;

    if (size >= MEGABYTE) {
        suffix = "Mb";
        factor = MEGABYTE;
    } else if (size >= KILOBYTE) {
        suffix = "Kb";
        factor = KILOBYTE;
    } else {
        suffix = "b";
        factor = 1;
    }

    int value = size / factor;

    tc_snprintf(buf, bufSize, "%d%s", value, suffix);

    return buf;
}

char* imprintSizeToStringEx(char* buf, size_t bufSize, size_t size, int flags)
{
    char* result = imprintSizeToString(buf, bufSize, size);
    size_t len = tc_strlen(result);
    int remaining = bufSize - len - 1;
    if (remaining < 10) {
        CLOG_ERROR("buffer is too small")
    }
    tc_snprintf(buf + len, remaining, " (%d)", size);

    return buf;
}

const char* imprintSizeToStringStatic(size_t size)
{
    static char buf[32];
    return imprintSizeToString(buf, 32, size);
}

const char* imprintSizeToStringExStatic(size_t size, int flags)
{
    static char buf[32];
    return imprintSizeToStringEx(buf, 32, size, flags);
}

char* imprintSizeAndPercentageToString(char* buf, size_t bufSize, size_t size, size_t maxSize)
{
    if (bufSize < 20) {
        return "";
    }
    size_t percentage = 100;
    if (maxSize != 0) {
        percentage = 100 * size / maxSize;
    }

    char internalBuf[32];
    tc_snprintf(buf, bufSize, "%s (%zu %%)", imprintSizeToString(internalBuf, 32, size), percentage);

    return buf;
}
