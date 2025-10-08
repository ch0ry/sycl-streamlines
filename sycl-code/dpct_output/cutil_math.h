    /*
 * Copyright 1993-2007 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

/*
    This file implements common mathematical operations on vector types
    (float3, float4 etc.) since these are not provided as standard by CUDA.

    The syntax is modelled on the Cg standard library.
*/

#ifndef CUTIL_MATH_H
#define CUTIL_MATH_H

#include <sycl/sycl.hpp>
#include <dpct/dpct.hpp>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////
typedef unsigned int uint;
typedef unsigned short ushort;

#ifndef SYCL_LANGUAGE_VERSION
#include <math.h>

inline float fminf(float a, float b)
{
  return a < b ? a : b;
}

inline float fmaxf(float a, float b)
{
  return a < b ? a : b;
}

inline int max(int a, int b)
{
  return a > b ? a : b;
}

inline int min(int a, int b)
{
  return a < b ? a : b;
}
#endif

// float functions
////////////////////////////////////////////////////////////////////////////////

// lerp
inline float lerp(float a, float b, float t)
{
    return a + t*(b-a);
}

// clamp
inline float clamp(float f, float a, float b)
{
    return sycl::fmax(a, sycl::fmin(f, b));
}

// int2 functions
////////////////////////////////////////////////////////////////////////////////

// negate
/*
DPCT1011:8: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::int2 operator-(sycl::int2 &a)
{
    return sycl::int2(-a.x(), -a.y());
}
} // namespace dpct_operator_overloading

// addition
/*
DPCT1011:9: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::int2 operator+(sycl::int2 a, sycl::int2 b)
{
    return sycl::int2(a.x() + b.x(), a.y() + b.y());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:10: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator+=(sycl::int2 &a, sycl::int2 b)
{
    a.x() += b.x(); a.y() += b.y();
}
} // namespace dpct_operator_overloading

// subtract
/*
DPCT1011:11: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::int2 operator-(sycl::int2 a, sycl::int2 b)
{
    return sycl::int2(a.x() - b.x(), a.y() - b.y());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:12: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator-=(sycl::int2 &a, sycl::int2 b)
{
    a.x() -= b.x(); a.y() -= b.y();
}
} // namespace dpct_operator_overloading

// multiply
/*
DPCT1011:13: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::int2 operator*(sycl::int2 a, sycl::int2 b)
{
    return sycl::int2(a.x() * b.x(), a.y() * b.y());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:14: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::int2 operator*(sycl::int2 a, int s)
{
    return sycl::int2(a.x() * s, a.y() * s);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:15: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::int2 operator*(int s, sycl::int2 a)
{
    return sycl::int2(a.x() * s, a.y() * s);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:16: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator*=(sycl::int2 &a, int s)
{
    a.x() *= s; a.y() *= s;
}
} // namespace dpct_operator_overloading

// float2 functions
////////////////////////////////////////////////////////////////////////////////

// additional constructors
inline sycl::float2 make_float2(float s)
{
    return sycl::float2(s, s);
}
inline sycl::float2 make_float2(sycl::int2 a)
{
    return sycl::float2(float(a.x()), float(a.y()));
}

// negate
/*
DPCT1011:17: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float2 operator-(sycl::float2 &a)
{
    return sycl::float2(-a.x(), -a.y());
}
} // namespace dpct_operator_overloading

// addition
/*
DPCT1011:18: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float2 operator+(sycl::float2 a, sycl::float2 b)
{
    return sycl::float2(a.x() + b.x(), a.y() + b.y());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:19: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator+=(sycl::float2 &a, sycl::float2 b)
{
    a.x() += b.x(); a.y() += b.y();
}
} // namespace dpct_operator_overloading

// subtract
/*
DPCT1011:20: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float2 operator-(sycl::float2 a, sycl::float2 b)
{
    return sycl::float2(a.x() - b.x(), a.y() - b.y());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:21: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator-=(sycl::float2 &a, sycl::float2 b)
{
    a.x() -= b.x(); a.y() -= b.y();
}
} // namespace dpct_operator_overloading

// multiply
/*
DPCT1011:22: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float2 operator*(sycl::float2 a, sycl::float2 b)
{
    return sycl::float2(a.x() * b.x(), a.y() * b.y());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:23: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float2 operator*(sycl::float2 a, float s)
{
    return sycl::float2(a.x() * s, a.y() * s);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:24: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float2 operator*(float s, sycl::float2 a)
{
    return sycl::float2(a.x() * s, a.y() * s);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:25: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator*=(sycl::float2 &a, float s)
{
    a.x() *= s; a.y() *= s;
}
} // namespace dpct_operator_overloading

// divide
/*
DPCT1011:26: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float2 operator/(sycl::float2 a, sycl::float2 b)
{
    return sycl::float2(a.x() / b.x(), a.y() / b.y());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:27: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float2 operator/(sycl::float2 a, float s)
{
    float inv = 1.0f / s;
    return dpct_operator_overloading::operator*(a, inv);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:28: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float2 operator/(float s, sycl::float2 a)
{
    float inv = 1.0f / s;
    return dpct_operator_overloading::operator*(a, inv);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:29: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator/=(sycl::float2 &a, float s)
{
    float inv = 1.0f / s;
    dpct_operator_overloading::operator*=(a, inv);
}
} // namespace dpct_operator_overloading

// lerp
inline sycl::float2 lerp(sycl::float2 a, sycl::float2 b, float t)
{
    return dpct_operator_overloading::operator+(
        a, dpct_operator_overloading::operator*(
               t, (dpct_operator_overloading::operator-(b, a))));
}

// clamp
inline sycl::float2 clamp(sycl::float2 v, float a, float b)
{
    return sycl::float2(clamp(v.x(), a, b), clamp(v.y(), a, b));
}

inline sycl::float2 clamp(sycl::float2 v, sycl::float2 a, sycl::float2 b)
{
    return sycl::float2(clamp(v.x(), a.x(), b.x()), clamp(v.y(), a.y(), b.y()));
}

// dot product
inline float dot(sycl::float2 a, sycl::float2 b)
{
    return a.x() * b.x() + a.y() * b.y();
}

// length
inline float length(sycl::float2 v)
{
    return sycl::sqrt(dot(v, v));
}

// normalize
inline sycl::float2 normalize(sycl::float2 v)
{
    float invLen = 1.0f / sycl::sqrt(dot(v, v));
    return dpct_operator_overloading::operator*(v, invLen);
}

// floor
inline sycl::float2 floor(const sycl::float2 v)
{
    return sycl::float2(sycl::floor((float)(v.x())), sycl::floor((float)(v.y())));
}

// reflect
inline sycl::float2 reflect(sycl::float2 i, sycl::float2 n)
{
        return dpct_operator_overloading::operator-(
            i, dpct_operator_overloading::operator*(
                   dpct_operator_overloading::operator*(2.0f, n), dot(n, i)));
}

// float3 functions
////////////////////////////////////////////////////////////////////////////////

// additional constructors
inline sycl::float3 make_float3(float s)
{
    return sycl::float3(s, s, s);
}
inline sycl::float3 make_float3(sycl::float2 a)
{
    return sycl::float3(a.x(), a.y(), 0.0f);
}
inline sycl::float3 make_float3(sycl::float2 a, float s)
{
    return sycl::float3(a.x(), a.y(), s);
}
inline sycl::float3 make_float3(sycl::float4 a)
{
    return sycl::float3(a.x(), a.y(), a.z()); // discards w
}
inline sycl::float3 make_float3(sycl::int3 a)
{
    return sycl::float3(float(a.x()), float(a.y()), float(a.z()));
}

// negate
/*
DPCT1011:30: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float3 operator-(sycl::float3 &a)
{
    return sycl::float3(-a.x(), -a.y(), -a.z());
}
} // namespace dpct_operator_overloading

// min
static __inline__ sycl::float3 fminf(sycl::float3 a, sycl::float3 b)
{
        return sycl::float3(sycl::fmin(a.x(), b.x()), sycl::fmin(a.y(), b.y()),
                            sycl::fmin(a.z(), b.z()));
}

// max
static __inline__ sycl::float3 fmaxf(sycl::float3 a, sycl::float3 b)
{
        return sycl::float3(sycl::fmax(a.x(), b.x()), sycl::fmax(a.y(), b.y()),
                            sycl::fmax(a.z(), b.z()));
}

// addition
/*
DPCT1011:31: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float3 operator+(sycl::float3 a, sycl::float3 b)
{
    return sycl::float3(a.x() + b.x(), a.y() + b.y(), a.z() + b.z());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:32: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float3 operator+(sycl::float3 a, float b)
{
    return sycl::float3(a.x() + b, a.y() + b, a.z() + b);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:33: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator+=(sycl::float3 &a, sycl::float3 b)
{
    a.x() += b.x(); a.y() += b.y(); a.z() += b.z();
}
} // namespace dpct_operator_overloading

// subtract
/*
DPCT1011:34: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float3 operator-(sycl::float3 a, sycl::float3 b)
{
    return sycl::float3(a.x() - b.x(), a.y() - b.y(), a.z() - b.z());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:35: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float3 operator-(sycl::float3 a, float b)
{
    return sycl::float3(a.x() - b, a.y() - b, a.z() - b);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:36: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator-=(sycl::float3 &a, sycl::float3 b)
{
    a.x() -= b.x(); a.y() -= b.y(); a.z() -= b.z();
}
} // namespace dpct_operator_overloading

// multiply
/*
DPCT1011:37: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float3 operator*(sycl::float3 a, sycl::float3 b)
{
    return sycl::float3(a.x() * b.x(), a.y() * b.y(), a.z() * b.z());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:38: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float3 operator*(sycl::float3 a, float s)
{
    return sycl::float3(a.x() * s, a.y() * s, a.z() * s);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:39: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float3 operator*(float s, sycl::float3 a)
{
    return sycl::float3(a.x() * s, a.y() * s, a.z() * s);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:40: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator*=(sycl::float3 &a, float s)
{
    a.x() *= s; a.y() *= s; a.z() *= s;
}
} // namespace dpct_operator_overloading

// divide
/*
DPCT1011:41: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float3 operator/(sycl::float3 a, sycl::float3 b)
{
    return sycl::float3(a.x() / b.x(), a.y() / b.y(), a.z() / b.z());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:42: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float3 operator/(sycl::float3 a, float s)
{
    float inv = 1.0f / s;
    return dpct_operator_overloading::operator*(a, inv);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:43: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float3 operator/(float s, sycl::float3 a)
{
    float inv = 1.0f / s;
    return dpct_operator_overloading::operator*(a, inv);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:44: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator/=(sycl::float3 &a, float s)
{
    float inv = 1.0f / s;
    dpct_operator_overloading::operator*=(a, inv);
}
} // namespace dpct_operator_overloading

// lerp
inline sycl::float3 lerp(sycl::float3 a, sycl::float3 b, float t)
{
    return dpct_operator_overloading::operator+(
        a, dpct_operator_overloading::operator*(
               t, (dpct_operator_overloading::operator-(b, a))));
}

// clamp
inline sycl::float3 clamp(sycl::float3 v, float a, float b)
{
    return sycl::float3(clamp(v.x(), a, b), clamp(v.y(), a, b),
                        clamp(v.z(), a, b));
}

inline sycl::float3 clamp(sycl::float3 v, sycl::float3 a, sycl::float3 b)
{
    return sycl::float3(clamp(v.x(), a.x(), b.x()), clamp(v.y(), a.y(), b.y()),
                        clamp(v.z(), a.z(), b.z()));
}

// dot product
inline float dot(sycl::float3 a, sycl::float3 b)
{
    return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
}

// cross product
inline sycl::float3 cross(sycl::float3 a, sycl::float3 b)
{
    return sycl::float3(a.y() * b.z() - a.z() * b.y(),
                        a.z() * b.x() - a.x() * b.z(),
                        a.x() * b.y() - a.y() * b.x());
}

// length
inline float length(sycl::float3 v)
{
    return sycl::sqrt(dot(v, v));
}

// normalize
inline sycl::float3 normalize(sycl::float3 v)
{
    float invLen = 1.0f / sycl::sqrt(dot(v, v));
    return dpct_operator_overloading::operator*(v, invLen);
}

// floor
inline sycl::float3 floor(const sycl::float3 v)
{
    return sycl::float3(sycl::floor((float)(v.x())),
                        sycl::floor((float)(v.y())),
                        sycl::floor((float)(v.z())));
}

// reflect
inline sycl::float3 reflect(sycl::float3 i, sycl::float3 n)
{
        return dpct_operator_overloading::operator-(
            i, dpct_operator_overloading::operator*(
                   dpct_operator_overloading::operator*(2.0f, n), dot(n, i)));
}

// float4 functions
////////////////////////////////////////////////////////////////////////////////

// additional constructors
inline sycl::float4 make_float4(float s)
{
    return sycl::float4(s, s, s, s);
}
inline sycl::float4 make_float4(sycl::float3 a)
{
    return sycl::float4(a.x(), a.y(), a.z(), 0.0f);
}
inline sycl::float4 make_float4(sycl::float3 a, float w)
{
    return sycl::float4(a.x(), a.y(), a.z(), w);
}
inline sycl::float4 make_float4(sycl::int4 a)
{
    return sycl::float4(float(a.x()), float(a.y()), float(a.z()), float(a.w()));
}

// negate
/*
DPCT1011:45: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float4 operator-(sycl::float4 &a)
{
    return sycl::float4(-a.x(), -a.y(), -a.z(), -a.w());
}
} // namespace dpct_operator_overloading

// min
static __inline__ sycl::float4 fminf(sycl::float4 a, sycl::float4 b)
{
        return sycl::float4(sycl::fmin(a.x(), b.x()), sycl::fmin(a.y(), b.y()),
                            sycl::fmin(a.z(), b.z()), sycl::fmin(a.w(), b.w()));
}

// max
static __inline__ sycl::float4 fmaxf(sycl::float4 a, sycl::float4 b)
{
        return sycl::float4(sycl::fmax(a.x(), b.x()), sycl::fmax(a.y(), b.y()),
                            sycl::fmax(a.z(), b.z()), sycl::fmax(a.w(), b.w()));
}

// addition
/*
DPCT1011:46: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float4 operator+(sycl::float4 a, sycl::float4 b)
{
    return sycl::float4(a.x() + b.x(), a.y() + b.y(), a.z() + b.z(),
                        a.w() + b.w());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:47: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator+=(sycl::float4 &a, sycl::float4 b)
{
    a.x() += b.x(); a.y() += b.y(); a.z() += b.z(); a.w() += b.w();
}
} // namespace dpct_operator_overloading

// subtract
/*
DPCT1011:48: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float4 operator-(sycl::float4 a, sycl::float4 b)
{
    return sycl::float4(a.x() - b.x(), a.y() - b.y(), a.z() - b.z(),
                        a.w() - b.w());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:49: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator-=(sycl::float4 &a, sycl::float4 b)
{
    a.x() -= b.x(); a.y() -= b.y(); a.z() -= b.z(); a.w() -= b.w();
}
} // namespace dpct_operator_overloading

// multiply
/*
DPCT1011:50: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float4 operator*(sycl::float4 a, float s)
{
    return sycl::float4(a.x() * s, a.y() * s, a.z() * s, a.w() * s);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:51: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float4 operator*(float s, sycl::float4 a)
{
    return sycl::float4(a.x() * s, a.y() * s, a.z() * s, a.w() * s);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:52: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator*=(sycl::float4 &a, float s)
{
    a.x() *= s; a.y() *= s; a.z() *= s; a.w() *= s;
}
} // namespace dpct_operator_overloading

// divide
/*
DPCT1011:53: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float4 operator/(sycl::float4 a, sycl::float4 b)
{
    return sycl::float4(a.x() / b.x(), a.y() / b.y(), a.z() / b.z(),
                        a.w() / b.w());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:54: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float4 operator/(sycl::float4 a, float s)
{
    float inv = 1.0f / s;
    return dpct_operator_overloading::operator*(a, inv);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:55: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::float4 operator/(float s, sycl::float4 a)
{
    float inv = 1.0f / s;
    return dpct_operator_overloading::operator*(a, inv);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:56: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator/=(sycl::float4 &a, float s)
{
    float inv = 1.0f / s;
    dpct_operator_overloading::operator*=(a, inv);
}
} // namespace dpct_operator_overloading

// lerp
inline sycl::float4 lerp(sycl::float4 a, sycl::float4 b, float t)
{
    return dpct_operator_overloading::operator+(
        a, dpct_operator_overloading::operator*(
               t, (dpct_operator_overloading::operator-(b, a))));
}

// clamp
inline sycl::float4 clamp(sycl::float4 v, float a, float b)
{
    return sycl::float4(clamp(v.x(), a, b), clamp(v.y(), a, b),
                        clamp(v.z(), a, b), clamp(v.w(), a, b));
}

inline sycl::float4 clamp(sycl::float4 v, sycl::float4 a, sycl::float4 b)
{
    return sycl::float4(clamp(v.x(), a.x(), b.x()), clamp(v.y(), a.y(), b.y()),
                        clamp(v.z(), a.z(), b.z()), clamp(v.w(), a.w(), b.w()));
}

// dot product
inline float dot(sycl::float4 a, sycl::float4 b)
{
    return a.x() * b.x() + a.y() * b.y() + a.z() * b.z() + a.w() * b.w();
}

// length
inline float length(sycl::float4 r)
{
    return sycl::sqrt(dot(r, r));
}

// normalize
inline sycl::float4 normalize(sycl::float4 v)
{
    float invLen = 1.0f / sycl::sqrt(dot(v, v));
    return dpct_operator_overloading::operator*(v, invLen);
}

// floor
inline sycl::float4 floor(const sycl::float4 v)
{
    return sycl::float4(
        sycl::floor((float)(v.x())), sycl::floor((float)(v.y())),
        sycl::floor((float)(v.z())), sycl::floor((float)(v.w())));
}

// int3 functions
////////////////////////////////////////////////////////////////////////////////

// additional constructors
inline sycl::int3 make_int3(int s)
{
    return sycl::int3(s, s, s);
}
inline sycl::int3 make_int3(sycl::float3 a)
{
    return sycl::int3(int(a.x()), int(a.y()), int(a.z()));
}

// negate
/*
DPCT1011:57: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::int3 operator-(sycl::int3 &a)
{
    return sycl::int3(-a.x(), -a.y(), -a.z());
}
} // namespace dpct_operator_overloading

// min
inline sycl::int3 min(sycl::int3 a, sycl::int3 b)
{
    return sycl::int3(sycl::min(a.x(), b.x()), sycl::min(a.y(), b.y()),
                      sycl::min(a.z(), b.z()));
}

// max
inline sycl::int3 max(sycl::int3 a, sycl::int3 b)
{
    return sycl::int3(sycl::max(a.x(), b.x()), sycl::max(a.y(), b.y()),
                      sycl::max(a.z(), b.z()));
}

// addition
/*
DPCT1011:58: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::int3 operator+(sycl::int3 a, sycl::int3 b)
{
    return sycl::int3(a.x() + b.x(), a.y() + b.y(), a.z() + b.z());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:59: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator+=(sycl::int3 &a, sycl::int3 b)
{
    a.x() += b.x(); a.y() += b.y(); a.z() += b.z();
}
} // namespace dpct_operator_overloading

// subtract
/*
DPCT1011:60: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::int3 operator-(sycl::int3 a, sycl::int3 b)
{
    return sycl::int3(a.x() - b.x(), a.y() - b.y(), a.z() - b.z());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:61: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator-=(sycl::int3 &a, sycl::int3 b)
{
    a.x() -= b.x(); a.y() -= b.y(); a.z() -= b.z();
}
} // namespace dpct_operator_overloading

// multiply
/*
DPCT1011:62: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::int3 operator*(sycl::int3 a, sycl::int3 b)
{
    return sycl::int3(a.x() * b.x(), a.y() * b.y(), a.z() * b.z());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:63: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::int3 operator*(sycl::int3 a, int s)
{
    return sycl::int3(a.x() * s, a.y() * s, a.z() * s);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:64: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::int3 operator*(int s, sycl::int3 a)
{
    return sycl::int3(a.x() * s, a.y() * s, a.z() * s);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:65: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator*=(sycl::int3 &a, int s)
{
    a.x() *= s; a.y() *= s; a.z() *= s;
}
} // namespace dpct_operator_overloading

// divide
/*
DPCT1011:66: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::int3 operator/(sycl::int3 a, sycl::int3 b)
{
    return sycl::int3(a.x() / b.x(), a.y() / b.y(), a.z() / b.z());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:67: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::int3 operator/(sycl::int3 a, int s)
{
    return sycl::int3(a.x() / s, a.y() / s, a.z() / s);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:68: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::int3 operator/(int s, sycl::int3 a)
{
    return sycl::int3(a.x() / s, a.y() / s, a.z() / s);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:69: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator/=(sycl::int3 &a, int s)
{
    a.x() /= s; a.y() /= s; a.z() /= s;
}
} // namespace dpct_operator_overloading

// clamp
inline int clamp(int f, int a, int b)
{
    return sycl::max(a, sycl::min(f, b));
}

inline sycl::int3 clamp(sycl::int3 v, int a, int b)
{
    return sycl::int3(clamp(v.x(), a, b), clamp(v.y(), a, b), clamp(v.z(), a, b));
}

inline sycl::int3 clamp(sycl::int3 v, sycl::int3 a, sycl::int3 b)
{
    return sycl::int3(clamp(v.x(), a.x(), b.x()), clamp(v.y(), a.y(), b.y()),
                      clamp(v.z(), a.z(), b.z()));
}


// uint3 functions
////////////////////////////////////////////////////////////////////////////////

// additional constructors
inline sycl::uint3 make_uint3(uint s)
{
    return sycl::uint3(s, s, s);
}
inline sycl::uint3 make_uint3(sycl::float3 a)
{
    return sycl::uint3(uint(a.x()), uint(a.y()), uint(a.z()));
}

// min
inline sycl::uint3 min(sycl::uint3 a, sycl::uint3 b)
{
    return sycl::uint3(sycl::min(a.x(), b.x()), sycl::min(a.y(), b.y()),
                       sycl::min(a.z(), b.z()));
}

// max
inline sycl::uint3 max(sycl::uint3 a, sycl::uint3 b)
{
    return sycl::uint3(sycl::max(a.x(), b.x()), sycl::max(a.y(), b.y()),
                       sycl::max(a.z(), b.z()));
}

// addition
/*
DPCT1011:70: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::uint3 operator+(sycl::uint3 a, sycl::uint3 b)
{
    return sycl::uint3(a.x() + b.x(), a.y() + b.y(), a.z() + b.z());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:71: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator+=(sycl::uint3 &a, sycl::uint3 b)
{
    a.x() += b.x(); a.y() += b.y(); a.z() += b.z();
}
} // namespace dpct_operator_overloading

// subtract
/*
DPCT1011:72: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::uint3 operator-(sycl::uint3 a, sycl::uint3 b)
{
    return sycl::uint3(a.x() - b.x(), a.y() - b.y(), a.z() - b.z());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:73: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator-=(sycl::uint3 &a, sycl::uint3 b)
{
    a.x() -= b.x(); a.y() -= b.y(); a.z() -= b.z();
}
} // namespace dpct_operator_overloading

// multiply
/*
DPCT1011:74: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::uint3 operator*(sycl::uint3 a, sycl::uint3 b)
{
    return sycl::uint3(a.x() * b.x(), a.y() * b.y(), a.z() * b.z());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:75: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::uint3 operator*(sycl::uint3 a, uint s)
{
    return sycl::uint3(a.x() * s, a.y() * s, a.z() * s);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:76: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::uint3 operator*(uint s, sycl::uint3 a)
{
    return sycl::uint3(a.x() * s, a.y() * s, a.z() * s);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:77: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator*=(sycl::uint3 &a, uint s)
{
    a.x() *= s; a.y() *= s; a.z() *= s;
}
} // namespace dpct_operator_overloading

// divide
/*
DPCT1011:78: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::uint3 operator/(sycl::uint3 a, sycl::uint3 b)
{
    return sycl::uint3(a.x() / b.x(), a.y() / b.y(), a.z() / b.z());
}
} // namespace dpct_operator_overloading

/*
DPCT1011:79: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::uint3 operator/(sycl::uint3 a, uint s)
{
    return sycl::uint3(a.x() / s, a.y() / s, a.z() / s);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:80: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline sycl::uint3 operator/(uint s, sycl::uint3 a)
{
    return sycl::uint3(a.x() / s, a.y() / s, a.z() / s);
}
} // namespace dpct_operator_overloading

/*
DPCT1011:81: The tool detected overloaded operators for built-in vector types,
which may conflict with the SYCL 2020 standard operators (see 4.14.2.1 Vec
interface). The tool inserted a namespace to avoid the conflict. Use SYCL 2020
standard operators instead.
*/
namespace dpct_operator_overloading {

inline void operator/=(sycl::uint3 &a, uint s)
{
    a.x() /= s; a.y() /= s; a.z() /= s;
}
} // namespace dpct_operator_overloading

// clamp
inline uint clamp(uint f, uint a, uint b)
{
    return dpct::max(a, sycl::min(f, b));
}

inline sycl::uint3 clamp(sycl::uint3 v, uint a, uint b)
{
    return sycl::uint3(clamp(v.x(), a, b), clamp(v.y(), a, b),
                       clamp(v.z(), a, b));
}

inline sycl::uint3 clamp(sycl::uint3 v, sycl::uint3 a, sycl::uint3 b)
{
    return sycl::uint3(clamp(v.x(), a.x(), b.x()), clamp(v.y(), a.y(), b.y()),
                       clamp(v.z(), a.z(), b.z()));
}

#endif
