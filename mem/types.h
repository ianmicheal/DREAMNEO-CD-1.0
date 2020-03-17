/* KallistiOS 2.1.0

   arch/dreamcast/include/types.h
   (c)2000-2001 Dan Potter

* This program is free software; you can redistribute it and/or modify
* it under the terms of the KOS License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* KOS License (DreamHAL note: it's in the LICENSE file) for more details.
*
* You should have received a copy of the KOS License along with this
* program; if not, please visit Cryptic Allusion Game Dev at:
*
*   http://gamedev.allusion.net/
*/

/** \file   arch/types.h
    \brief  Common integer types.

    This file contains typedefs for some common/useful integer types. These
    types include ones that tell you exactly how long they are, as well as some
    BSD-isms.

    \author Dan Potter
*/

// Note 1/4/2020:
// This file has been modified in order to work in a freestanding environment.
// --Moopthehedgehog

#ifndef __ARCH_TYPES_H
#define __ARCH_TYPES_H

#include "stddef.h"

/* Generic types */
typedef unsigned long long uint64;  /**< \brief 64-bit unsigned integer */
typedef unsigned long uint32;       /**< \brief 32-bit unsigned integer */
typedef unsigned short uint16;      /**< \brief 16-bit unsigned integer */
typedef unsigned char uint8;        /**< \brief 8-bit unsigned integer */
typedef long long int64;            /**< \brief 64-bit signed integer */
typedef long int32;                 /**< \brief 32-bit signed integer */
typedef short int16;                /**< \brief 16-bit signed integer */
typedef char int8;                  /**< \brief 8-bit signed integer */

/* Volatile types */
typedef volatile uint64 vuint64;    /**< \brief 64-bit volatile unsigned type */
typedef volatile uint32 vuint32;    /**< \brief 32-bit volatile unsigned type */
typedef volatile uint16 vuint16;    /**< \brief 16-bit volatile unsigned type */
typedef volatile uint8 vuint8;      /**< \brief 8-bit volatile unsigned type */
typedef volatile int64 vint64;      /**< \brief 64-bit volatile signed type */
typedef volatile int32 vint32;      /**< \brief 32-bit volatile signed type */
typedef volatile int16 vint16;      /**< \brief 16-bit volatile signed type */
typedef volatile int8 vint8;        /**< \brief 8-bit volatile signed type */

/* Pointer arithmetic types */
typedef uint32 ptr_t;               /**< \brief Pointer arithmetic type */

/* another format for type names */
#ifndef _BSDTYPES_DEFINED

/* \cond */
#define __u_char_defined
#define __u_short_defined
#define __u_int_defined
#define __u_long_defined
#define __ushort_defined
#define __uint_defined
/* \endcond */

typedef unsigned char   u_char;     /**< \brief BSD-style unsigned char */
typedef unsigned short  u_short;    /**< \brief BSD-style unsigned short */
typedef unsigned int    u_int;      /**< \brief BSD-style unsigned integer */
typedef unsigned long   u_long;     /**< \brief BSD-style unsigned long */
typedef unsigned short  ushort;     /**< \brief BSD-style unsigned short */
typedef unsigned int    uint;       /**< \brief BSD-style unsigned integer */

/* \cond */
#define _BSDTYPES_DEFINED
/* \endcond */

#endif  /* _BSDTYPES_DEFINED */

/* This type may be used for any generic handle type that is allowed
   to be negative (for errors) and has no specific bit count
   restraints. */
typedef int handle_t;               /**< \brief Generic "handle" type */

/* Thread and priority types */
typedef handle_t tid_t;             /**< \brief Thread ID type */
typedef handle_t prio_t;            /**< \brief Priority value type */

/** \brief  Endianness definition -- Little Endian */
#define BYTE_ORDER  LITTLE_ENDIAN

#endif  /* __ARCH_TYPES_H */
