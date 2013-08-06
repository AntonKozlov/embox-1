/**
 * @file
 * @brief
 *
 * @date 16.11.12
 * @author Anton Bondarev
 */

#ifndef LIMITS_H_
#define LIMITS_H_

#include <asm/limits.h>

#define CHAR_BIT 	8
#define CHAR_MIN 	SCHAR_MIN
#define CHAR_MAX 	SCHAR_MAX
#define SCHAR_MIN 	-127
#define SCHAR_MAX 	128
#define UCHAR_MAX 	255

#define WORD_BIT 	32
#define INT_MIN		(-INT_MAX - 1)
#define INT_MAX		2147483647
#define UINT_MAX	4294967295U

#define SHRT_MIN	(-32768)
#define SHRT_MAX	32767
#define USHRT_MAX	65535

#define LONG_BIT	32
#define LONG_MIN	(-LONG_MAX - 1)
#define LONG_MAX	2147483647L
#define ULONG_MAX	4294967295UL

#define LLONG_MIN	(-LLONG_MAX - 1)
#define LLONG_MAX	9223372036854775807LL

#define ULLONG_MAX	18446744073709551615ULL

/**
 * Pathname Variable Values
 */

/* Maximum number of bytes in a filename (not including \0) */
#define NAME_MAX 32

/* Maximum number of bytes in a pathname, including \0 character */
#define PATH_MAX 256


#endif /* LIMITS_H_ */
