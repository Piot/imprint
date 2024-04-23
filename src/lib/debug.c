/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/imprint
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#include <imprint/debug.h>
#include <tiny-libc/tiny_libc.h>

const char* imprintDebugFormatFileLink(
    char* buf, size_t maxBuf, const char* absolutePath, size_t line, const char* description)
{
    tc_snprintf(buf, maxBuf, "file://%s:%zu '%s'", absolutePath, line, description);

    return buf;
}
