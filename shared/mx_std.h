/*
 *  This file is part of the Maxwell Word Processor application.
 *  Copyright (C) 1996, 1997, 1998 Andrew Haisley, David Miller, Tom Newton
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/*
 * MODULE/CLASS : mx_std.h
 *
 * AUTHOR : Andrew Haisley
 *
 *
 *
 *
 *
 *
 */
#ifndef MX_STD_H
#define MX_STD_H

#include <cstdint>
#include <math.h>

typedef uint32_t uint32;
typedef int32_t int32;
typedef int16_t int16;
typedef uint16_t uint16;
typedef uint8_t uint8;
typedef int8_t int8;

typedef unsigned long int LUT_VALUE;

// latest gcc (2.7.2) doesn't need this
// typedef int bool;

#ifndef TRUE
#define TRUE (bool)1
#define FALSE (bool)0
#endif
#define MAYBE (bool)2

#define MX_EFZ 0.000001
#define MX_FLOAT_EQ(x, y) (fabs((x) - (y)) < MX_EFZ)

#define MAX_FONTS 512

// this stuff taken from libc5 headers - it doesn't exist in the glibc headers
#ifndef M_E
#define M_E 2.7182818284590452354 /* e */
#endif
#ifndef M_LOG2E
#define M_LOG2E 1.4426950408889634074 /* log 2e */
#endif
#ifndef M_LOG10E
#define M_LOG10E 0.43429448190325182765 /* log 10e */
#endif
#ifndef M_LN2
#define M_LN2 0.69314718055994530942 /* log e2 */
#endif
#ifndef M_LN10
#define M_LN10 2.30258509299404568402 /* log e10 */
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923 /* pi/2 */
#endif
#ifndef M_1_PI
#define M_1_PI 0.31830988618379067154 /* 1/pi */
#endif
#ifndef M_PI_4
#define M_PI_4 0.78539816339744830962 /* pi/4 */
#endif
#ifndef M_2_PI
#define M_2_PI 0.63661977236758134308 /* 2/pi */
#endif
#ifndef M_2_SQRTPI
#define M_2_SQRTPI 1.12837916709551257390 /* 2/sqrt(pi) */
#endif
#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880 /* sqrt(2) */
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2 0.70710678118654752440 /* 1/sqrt(2) */
#endif

#ifndef PI /* as in stroustrup */
#define PI M_PI
#endif
#ifndef PI2
#define PI2 M_PI_2
#endif

#include <limits.h>

// maximum number of printers
#define MX_MAX_PRINTERS 50

#endif
