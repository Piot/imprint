/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/imprint
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#ifndef IMPRINT_DEBUG_H
#define IMPRINT_DEBUG_H

#include <stddef.h>

const char* imprintDebugFormatFileLink(
    char* buf, size_t maxBuf, const char* absolutePath, size_t line, const char* description);

#endif //IMPRINT_DEBUG_H
