#include <imprint/utils.h>
#include <tiny-libc/tiny_libc.h>

char *imprintSizeToString(char *buf, size_t bufSize, size_t size) {
  const int KILOBYTE = 1024;
  const int MEGABYTE = 1024 * KILOBYTE;
  int factor;
  char *suffix;

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

  tc_snprintf(buf, bufSize, "%d %s", value, suffix);

  return buf;
}

char *imprintSizeAndPercentageToString(char *buf, size_t bufSize, size_t size,
                                       size_t maxSize) {
  if (bufSize < 20) {
    return "";
  }
  size_t percentage = 100;
  if (maxSize != 0) {
    percentage = 100 * size / maxSize;
  }

  char internalBuf[32];
  tc_snprintf(buf, bufSize, "%s (%zu %%)",
              imprintSizeToString(internalBuf, 32, size), percentage);

  return buf;
}
