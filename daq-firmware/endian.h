#ifndef _ENDIAN_H_
#define _ENDIAN_H_

#include <stdint.h>

#ifndef __MY_BYTE_ORDER
#  ifndef __MY_LITTLE_ENDIAN
#    define __MY_LITTLE_ENDIAN     1234
#  endif /* __MY_LITTLE_ENDIAN */
#  ifndef __MY_BIG_ENDIAN
#    define __MY_BIG_ENDIAN        4321
#  endif /* __MY_BIG_ENDIAN */
#  if defined (i386) || defined (__i386__) || defined (_M_IX86) || \
      defined (vax) || defined (__alpha) || defined (__x86_64__) || \
      defined (ARDUINO_ARCH_AVR) || \
      (defined (__ARM_ARCH) && !defined(__ARM_BIG_ENDIAN))
#    define __MY_BYTE_ORDER __MY_LITTLE_ENDIAN
#  else
#    define __MY_BYTE_ORDER __MY_BIG_ENDIAN
#  endif /* defined ... */
#endif /* __MY_BYTE_ORDER */


/*
 * Const versions should not be used with expressions that have side effects
 * (e.g. i++, i--, or functions that have side effects) or that aren't thread
 * safe. The const verion is still useful for initializaiton of static or
 * const variables that already have such a restriction themselves, though.
 *
 * There is currently no const float/double version, but this is just because
 * it was not needed when the safe/const split was made.
 */


#if __MY_BYTE_ORDER == __MY_BIG_ENDIAN
#define endianS16_BtoN(x)   (x)
#define endianS16_NtoB(x)   (x)
#define endianU16_BtoN(x)   (x)
#define endianU16_NtoB(x)   (x)
#define endianS32_BtoN(x)   (x)
#define endianS32_NtoB(x)   (x)
#define endianU32_BtoN(x)   (x)
#define endianU32_NtoB(x)   (x)
#define endianS64_BtoN(x)   (x)
#define endianS64_NtoB(x)   (x)
#define endianU64_BtoN(x)   (x)
#define endianU64_NtoB(x)   (x)
#define endianF_BtoN(x)     (x)
#define endianF_NtoB(x)     (x)
#define endianD_BtoN(x)     (x)
#define endianD_NtoB(x)     (x)
#define endianS16_LtoN(x)   endianS16_LtoB(x)
#define endianS16_NtoL(x)   endianS16_BtoL(x)
#define endianU16_LtoN(x)   endianU16_LtoB(x)
#define endianU16_NtoL(x)   endianU16_BtoL(x)
#define endianS32_LtoN(x)   endianS32_LtoB(x)
#define endianS32_NtoL(x)   endianS32_BtoL(x)
#define endianU32_LtoN(x)   endianU32_LtoB(x)
#define endianU32_NtoL(x)   endianU32_BtoL(x)
#define endianS64_LtoN(x)   endianS64_LtoB(x)
#define endianS64_NtoL(x)   endianS64_BtoL(x)
#define endianU64_LtoN(x)   endianU64_LtoB(x)
#define endianU64_NtoL(x)   endianU64_BtoL(x)
#define endianF_LtoN(x)     endianF_LtoB(x)
#define endianF_NtoL(x)     endianF_BtoL(x)
#define endianD_LtoN(x)     endianD_LtoB(x)
#define endianD_NtoL(x)     endianD_BtoL(x)

#define endianS16_BtoN_const(x)   (x)
#define endianS16_NtoB_const(x)   (x)
#define endianU16_BtoN_const(x)   (x)
#define endianU16_NtoB_const(x)   (x)
#define endianS32_BtoN_const(x)   (x)
#define endianS32_NtoB_const(x)   (x)
#define endianU32_BtoN_const(x)   (x)
#define endianU32_NtoB_const(x)   (x)
#define endianS64_BtoN_const(x)   (x)
#define endianS64_NtoB_const(x)   (x)
#define endianU64_BtoN_const(x)   (x)
#define endianU64_NtoB_const(x)   (x)
#define endianS16_LtoN_const(x)   endianS16_LtoB_const(x)
#define endianS16_NtoL_const(x)   endianS16_BtoL_const(x)
#define endianU16_LtoN_const(x)   endianU16_LtoB_const(x)
#define endianU16_NtoL_const(x)   endianU16_BtoL_const(x)
#define endianS32_LtoN_const(x)   endianS32_LtoB_const(x)
#define endianS32_NtoL_const(x)   endianS32_BtoL_const(x)
#define endianU32_LtoN_const(x)   endianU32_LtoB_const(x)
#define endianU32_NtoL_const(x)   endianU32_BtoL_const(x)
#define endianS64_LtoN_const(x)   endianS64_LtoB_const(x)
#define endianS64_NtoL_const(x)   endianS64_BtoL_const(x)
#define endianU64_LtoN_const(x)   endianU64_LtoB_const(x)
#define endianU64_NtoL_const(x)   endianU64_BtoL_const(x)

#else

#define endianS16_BtoN(x)   endianS16_BtoL(x)
#define endianS16_NtoB(x)   endianS16_LtoB(x)
#define endianU16_BtoN(x)   endianU16_BtoL(x)
#define endianU16_NtoB(x)   endianU16_LtoB(x)
#define endianS32_BtoN(x)   endianS32_BtoL(x)
#define endianS32_NtoB(x)   endianS32_LtoB(x)
#define endianU32_BtoN(x)   endianU32_BtoL(x)
#define endianU32_NtoB(x)   endianU32_LtoB(x)
#define endianS64_BtoN(x)   endianS64_BtoL(x)
#define endianS64_NtoB(x)   endianS64_LtoB(x)
#define endianU64_BtoN(x)   endianU64_LtoB(x)
#define endianU64_NtoB(x)   endianU64_BtoL(x)
#define endianF_BtoN(x)     endianF_BtoL(x)
#define endianF_NtoB(x)     endianF_LtoB(x)
#define endianD_BtoN(x)     endianD_BtoL(x)
#define endianD_NtoB(x)     endianD_LtoB(x)
#define endianS16_LtoN(x)   (x)
#define endianS16_NtoL(x)   (x)
#define endianU16_LtoN(x)   (x)
#define endianU16_NtoL(x)   (x)
#define endianS32_LtoN(x)   (x)
#define endianS32_NtoL(x)   (x)
#define endianU32_LtoN(x)   (x)
#define endianU32_NtoL(x)   (x)
#define endianS64_LtoN(x)   (x)
#define endianS64_NtoL(x)   (x)
#define endianU64_LtoN(x)   (x)
#define endianU64_NtoL(x)   (x)
#define endianF_LtoN(x)     (x)
#define endianF_NtoL(x)     (x)
#define endianD_LtoN(x)     (x)
#define endianD_NtoL(x)     (x)

#define endianS16_BtoN_const(x)   endianS16_BtoL_const(x)
#define endianS16_NtoB_const(x)   endianS16_LtoB_const(x)
#define endianU16_BtoN_const(x)   endianU16_BtoL_const(x)
#define endianU16_NtoB_const(x)   endianU16_LtoB_const(x)
#define endianS32_BtoN_const(x)   endianS32_BtoL_const(x)
#define endianS32_NtoB_const(x)   endianS32_LtoB_const(x)
#define endianU32_BtoN_const(x)   endianU32_BtoL_const(x)
#define endianU32_NtoB_const(x)   endianU32_LtoB_const(x)
#define endianS64_BtoN_const(x)   endianS64_BtoL_const(x)
#define endianS64_NtoB_const(x)   endianS64_LtoB_const(x)
#define endianU64_BtoN_const(x)   endianU64_LtoB_const(x)
#define endianU64_NtoB_const(x)   endianU64_BtoL_const(x)
#define endianS16_LtoN_const(x)   (x)
#define endianS16_NtoL_const(x)   (x)
#define endianU16_LtoN_const(x)   (x)
#define endianU16_NtoL_const(x)   (x)
#define endianS32_LtoN_const(x)   (x)
#define endianS32_NtoL_const(x)   (x)
#define endianU32_LtoN_const(x)   (x)
#define endianU32_NtoL_const(x)   (x)
#define endianS64_LtoN_const(x)   (x)
#define endianS64_NtoL_const(x)   (x)
#define endianU64_LtoN_const(x)   (x)
#define endianU64_NtoL_const(x)   (x)

#endif /* __MY_BYTE_ORDER == __MY_BIG_ENDIAN */

/* Implement LtoB and BtoL */

#define endianS16_LtoB(x)   ((int16_t)endian16_swap(x))
#define endianS16_BtoL(x)   ((int16_t)endian16_swap(x))
#define endianU16_LtoB(x)   ((uint16_t)endian16_swap(x))
#define endianU16_BtoL(x)   ((uint16_t)endian16_swap(x))
#define endianS32_LtoB(x)   ((int32_t)endian32_swap(x))
#define endianS32_BtoL(x)   ((int32_t)endian32_swap(x))
#define endianU32_LtoB(x)   ((uint32_t)endian32_swap(x))
#define endianU32_BtoL(x)   ((uint32_t)endian32_swap(x))
#define endianS64_LtoB(x)   ((int64_t)endian64_swap(x))
#define endianS64_BtoL(x)   ((int64_t)endian64_swap(x))
#define endianU64_LtoB(x)   ((uint64_t)endian64_swap(x))
#define endianU64_BtoL(x)   ((uint64_t)endian64_swap(x))
#define endianF_LtoB(x)     endianFloat_swap(x)
#define endianF_BtoL(x)     endianFloat_swap(x)
#define endianD_LtoB(x)     endianDouble_swap(x)
#define endianD_BtoL(x)     endianDouble_swap(x)

#define endianS16_LtoB_const(x)   ((int16_t)endian16_swap_const(x))
#define endianS16_BtoL_const(x)   ((int16_t)endian16_swap_const(x))
#define endianU16_LtoB_const(x)   ((uint16_t)endian16_swap_const(x))
#define endianU16_BtoL_const(x)   ((uint16_t)endian16_swap_const(x))
#define endianS32_LtoB_const(x)   ((int32_t)endian32_swap_const(x))
#define endianS32_BtoL_const(x)   ((int32_t)endian32_swap_const(x))
#define endianU32_LtoB_const(x)   ((uint32_t)endian32_swap_const(x))
#define endianU32_BtoL_const(x)   ((uint32_t)endian32_swap_const(x))
#define endianS64_LtoB_const(x)   ((int64_t)endian64_swap_const(x))
#define endianS64_BtoL_const(x)   ((int64_t)endian64_swap_const(x))
#define endianU64_LtoB_const(x)   ((uint64_t)endian64_swap_const(x))
#define endianU64_BtoL_const(x)   ((uint64_t)endian64_swap_const(x))

/* Macros that always swap */
#define endian16_swap_const(x) \
    ((((uint16_t)((x) & 0x00ff)) << 8) | \
     (((uint16_t)((x) & 0xff00)) >> 8))

#define endian16_swap(x) ({ uint16_t __i; __i = (x); __i = endian16_swap_const(__i); __i; })

#define endian32_swap_const(x) \
    ((((uint32_t)((x) & 0x000000ff)) << 24) | \
     (((uint32_t)((x) & 0x0000ff00)) << 8) | \
     (((uint32_t)((x) & 0x00ff0000)) >> 8) | \
     (((uint32_t)((x) & 0xff000000)) >> 24))

#define endian32_swap(x) ({ uint32_t __i; __i = (x); __i = endian32_swap_const(__i); __i; })

#define endian64_swap_const(x) \
    (((((uint64_t)(x))<<56) & 0xff00000000000000ULL)  | \
     ((((uint64_t)(x))<<40) & 0x00ff000000000000ULL)  | \
     ((((uint64_t)(x))<<24) & 0x0000ff0000000000ULL)  | \
     ((((uint64_t)(x))<< 8) & 0x000000ff00000000ULL)  | \
     ((((uint64_t)(x))>> 8) & 0x00000000ff000000ULL)  | \
     ((((uint64_t)(x))>>24) & 0x0000000000ff0000ULL)  | \
     ((((uint64_t)(x))>>40) & 0x000000000000ff00ULL)  | \
     ((((uint64_t)(x))>>56) & 0x00000000000000ffULL))

#define endian64_swap(x) ({uint64_t __i; __i = (x); __i = endian64_swap_const(__i); __i;})
#define endianFloat_swap(x) ({ union { float f; uint32_t i; } __c; __c.f = (x); __c.i = endian32_swap_const(__c.i); __c.f; })
#define endianDouble_swap(x) ({ union { double d; uint64_t i; } __c; __c.d = (x); __c.i = endian64_swap_const(__c.i); __c.d; })

#endif /* _ENDIAN_H_ */
