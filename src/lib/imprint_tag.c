#include <imprint/tag.h>
#include <stddef.h>

uint64_t imprintTagFromString(char tag[8])
{
    uint64_t v = 0;
    for (size_t i=0; i<8; ++i) {
        v <<= 8;
        v |= (uint8_t) tag[i];
    }

    return v;
}
