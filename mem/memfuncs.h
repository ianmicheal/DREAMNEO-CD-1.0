//==============================================================================
//  SH4 Memory Functions: Main Header
//==============================================================================
//
// This file provides function prototypes for memmove, memcpy, memset, and memcmp,
// which are required by GCC when used in a freestanding environment.
//
// Adapted from KNNSpeed's "AVX Memmove":
// https://github.com/KNNSpeed/AVX-Memmove
// V1.3875, 1/4/2020
//
// NOTE:
// If you need to move/copy memory between overlapping regions, use memmove instead of memcpy.
//

#ifndef __MEMFUNCS_H_
#define __MEMFUNCS_H_

#include "stddef.h"
#include "stdint.h"

//
// USAGE INFORMATION:
//
// The "len" argument is "# of x bytes to move," e.g. memmove_32bit needs to
// know "how many multiples of 32 bit (4 bytes) to move." All functions with len
// follow the same pattern, e.g. memset_16bit needs to know how many multiples
// of 2 bytes to set, so a len of 4 tells it to set 8 bytes.
//
// IMPORTANT NOTE: Please ensure that destination and source buffers are aligned
// appropriately (1, 2, 4, 8, or 32 bytes depending on the function). Your program
// may crash if not (and the 64-bit functions *will* crash unless you get lucky).
// So __attribute__((aligned(*size*))) is very useful here!
//

void * memmove (void *dest, const void *src, size_t len);
void * memmove_16bit(void *dest, const void *src, size_t len);
void * memmove_32bit(void *dest, const void *src, size_t len);
void * memmove_64bit(void *dest, const void *src, size_t len); // There is a way :)

void * memcpy (void *dest, const void *src, size_t len);
void * memcpy_16bit(void *dest, const void *src, size_t len);
void * memcpy_32bit(void *dest, const void *src, size_t len);
void * memcpy_32bit_16Bytes(void *dest, const void *src, unsigned int len); // 16 bytes, 4-byte alignmnet
void * memcpy_64bit(void *dest, const void *src, size_t len);
void * memcpy_64bit_32Bytes(void *dest, const void *src, size_t len); // 32 bytes, 8-byte alignment
// NOTE: Store Queues are better, though they require 32-byte alignment.

void * memset1 (void *dest, const uint8_t val, size_t len);
void * memset_16bit(void *dest, const uint16_t val, size_t len);
void * memset_32bit(void *dest, const uint32_t val, size_t len);
// NOTE: 64bit takes 32-bit input data and writes it two times simultaneously per write
void * memset_64bit_4B(void *dest, const uint32_t val, size_t len);
// Write 8 bytes of zeroes at a time
void * memset_zeroes_64bit(void *dest, size_t len);

// In standard comparison versions, return value of -1 = str1 is less, 0 = equal, 1 = str1 is greater.
// In equality-only versions, a return value of 0 means equal, -1 means unequal.
// The "count" argument for these functions works the same way as the "len"
// argument mentioned above.
int memcmp (const void *str1, const void *str2, size_t count);
int memcmp_eq (const void *str1, const void *str2, size_t count);

int memcmp_16bit(const void *str1, const void *str2, size_t count);
int memcmp_16bit_eq(const void *str1, const void *str2, size_t count);

int memcmp_32bit(const void *str1, const void *str2, size_t count);
int memcmp_32bit_eq(const void *str1, const void *str2, size_t count);

//
// SH4 has a cmp/str instruction, which checks for byte-corresponding inequality
// (e.g. ignoring null-termination, "ABCD" vs. "WXYZ" or 0x12345678 vs. 0x87654321,
// but not "BAND" vs. "CANS" or 0x55559999 vs. 0x66567777). It makes for a kind of
// an "inverse memcmp."
//

// If str1 and str2 have no corresponding bytes, return the total length that was
// compared, in bytes (= count * 4). Otherwise return the byte offset (array index)
// at which the first equality was found.
int inv_memcmp_32bit(const void *str1, const void *str2, size_t count);
// Equality-only version:
// Return value of 0 means the inputs have no corresponding bytes
// Return value of -1 means the inputs have at least one corresponding byte somewhere
int inv_memcmp_32bit_eq(const void *str1, const void *str2, size_t count);


#endif /* __MEMFUNCS_H_ */
