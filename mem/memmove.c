// Compile with GCC -O3 for best performance

// Handles overlapping memory areas, too!

#include "memfuncs.h"

// Default (8-bit, 1 byte at a time)
void * memmove (void *dest, const void *src, size_t len)
{
  const char *s = (char *)src;
  char *d = (char *)dest;

  const char *nexts = s + len;
  char *nextd = d + len;

  if (d < s)
  {
    while (d != nextd)
    {
      *d++ = *s++;
    }
  }
  else
  {
    while (nextd != d)
    {
      *--nextd = *--nexts;
    }
  }
  return dest;
}

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
// Specifically these come from version 1.45 of memmove.c
//

// 16-bit (2 bytes at a time)
// Len is (# of total bytes/2), so it's "# of 16-bits"

void * memmove_16bit(void *dest, const void *src, size_t len)
{
  const uint16_t* s = (uint16_t*)src;
  uint16_t* d = (uint16_t*)dest;

  const uint16_t *nexts = s + len;
  uint16_t *nextd = d + len;

  if (d < s)
  {
    while (d != nextd)
    {
      *d++ = *s++;
    }
  }
  else
  {
    while (nextd != d)
    {
      *--nextd = *--nexts;
    }
  }
  return dest;
}

// 32-bit (4 bytes at a time - 1 pixel in a 32-bit linear frame buffer)
// Len is (# of total bytes/4), so it's "# of 32-bits"

void * memmove_32bit(void *dest, const void *src, size_t len)
{
  const uint32_t* s = (uint32_t*)src;
  uint32_t* d = (uint32_t*)dest;

  const uint32_t *nexts = s + len;
  uint32_t *nextd = d + len;

  if (d < s)
  {
    while (d != nextd)
    {
      *d++ = *s++;
    }
  }
  else
  {
    while (nextd != d)
    {
      *--nextd = *--nexts;
    }
  }
  return dest;
}
