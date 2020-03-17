/*
######################################################################
#                 Dream neo cd for DreamCast  V 1.0                  #
######################################################################
# including :	Dreamneo cd v1.0 Beta                                #
#  Ported by Ian Micheal Based on   Neogeo cd SDL by fosters         #
#											                         #
#			2004 team and credits									 #
#	#code and co coders quzar black aura troy GPF blue crab		     #
######################################################################
#																	 #
#								   Makefile (c)2020 from Ian Micheal #
######################################################################
*/
//-- Include Files ----------------------------------------------------------- 
#ifndef _TYPES_H_
#define _TYPES_H_

#ifdef _arch_dreamcast
#include <arch/types.h>
#else
typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned long int uint32;

typedef signed char int8;
typedef signed short int int16;
typedef signed long int int32;
#endif


// target specific
///////////////////

#ifdef _arch_dreamcast
#ifndef __sh__
#define __sh__
#endif
#undef  __x86__
#else
#ifndef __x86__
#define __x86__
#endif
#undef  __sh__
#endif

// compiler specific
/////////////////////

#ifndef INLINE
#ifdef __x86__
#define INLINE      __inline
#else
#define INLINE      __inline
#endif
#endif

#ifndef FASTCALL
#ifdef __x86__
#define FASTCALL    __fastcall
#else
#define FASTCALL
#endif
#endif

// types definitions
/////////////////////

#ifndef NULL
#define NULL 0
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

typedef char __s8;
typedef unsigned char __u8;

typedef short __s16;
typedef unsigned short __u16;

typedef int __s32;
typedef unsigned int __u32;

typedef long long __s64;
typedef unsigned long long __u64;


typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef signed char S8;
typedef signed short S16;
typedef signed int S32;


#endif /* _TYPES_H_ */

