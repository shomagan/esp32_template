#ifndef TYPE_DEF_H
#define TYPE_DEF_H
#include "stdint.h"
#define     __O     volatile                  /*!< defines 'write only' permissions     */
#define     __IO    volatile                  /*!< defines 'read / write' permissions   */

#define MCU_PACK __attribute__((packed))
#define delay_until vTaskDelayUntil
typedef signed long long int s64;
typedef signed long int s32;
typedef signed short s16;
typedef signed char s8;
typedef signed char i8;

typedef volatile signed long long int vs64;
typedef volatile signed long int vs32;
typedef volatile signed short vs16;
typedef volatile signed char vs8;
typedef volatile signed char vi8;

typedef uint64_t u64;
typedef unsigned long int u32;
typedef unsigned int ui32;//for get rid of warning messege
typedef unsigned short u16;
typedef unsigned char u8;

typedef const unsigned long long int uc64;
typedef const unsigned long int uc32;
typedef const unsigned short uc16;
typedef const unsigned char uc8;

typedef volatile unsigned long int vu32;
typedef volatile unsigned short vu16;
typedef volatile unsigned char vu8;

typedef volatile unsigned long int const vuc32;
typedef volatile unsigned short const vuc16;
typedef volatile unsigned char const vuc8;

#ifndef TRUE
  #define TRUE 1
#endif
#ifndef FALSE
  #define FALSE 0
#endif
#define RES_OK    0
#define RES_ERROR (-1)
#define asm __asm

#if defined __MINGW32__
#define MCU_PACK __attribute__((packed, gcc_struct))
#else
#define MCU_PACK __attribute__((packed))
#endif

#define MCU_NAKED __attribute__((naked))
#define MCU_ALIGN(x) __attribute__((aligned (x)))
#define MCU_ALWAYS_INLINE __attribute__((always_inline))
#define MCU_NEVER_INLINE __attribute__((noinline))
#define FNCT_NO_RETURN __attribute__((noreturn))


#endif// TYPE_DEF_H
