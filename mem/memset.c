// Compile with GCC -O3 for best performance

#include "memfuncs.h"
/*
void * memset (void *dest, const uint8_t val, size_t len)
{
  uint8_t *ptr = (uint8_t*)dest;

  while (len--)
  {
    *ptr++ = val;
  }

  return dest;
}
*/
///=============================================================================
/// LICENSING INFORMATION
///=============================================================================
//
// The code above this comment is in the public domain.
// The code below this comment is subject to the custom attribution license found
// here: https://github.com/KNNSpeed/AVX-Memmove/blob/master/LICENSE
//
// Adapted from KNNSpeed's "AVX Memmove":
// https://github.com/KNNSpeed/AVX-Memmove
// V1.3875, 1/4/2020
//
// Specifically these come from version 1.5 of memset.c
//

// 16-bit (2 bytes at a time)
// Len is (# of total bytes/2), so it's "# of 16-bits"

void * memset_16bit(void *dest, const uint16_t val, size_t len)
{
  uint16_t *ptr = (uint16_t*)dest;

  while (len--)
  {
    *ptr++ = val;
  }

  return dest;
}

// 32-bit (4 bytes at a time - 1 pixel in a 32-bit linear frame buffer)
// Len is (# of total bytes/4), so it's "# of 32-bits"

void * memset_32bit(void *dest, const uint32_t val, size_t len)
{
  uint32_t *ptr = (uint32_t*)dest;

  while (len--)
  {
    *ptr++ = val;
  }

  return dest;
}


