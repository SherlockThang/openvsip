//
// Copyright (c) 2005 - 2010 by CodeSourcery
// Copyright (c) 2013 Stefan Seefeld
// All rights reserved.
//
// This file is part of OpenVSIP. It is made available under the
// license contained in the accompanying LICENSE.BSD file.

/// Description
///   VSIPL++ Library: configuration items.

#ifndef VSIP_CORE_CONFIG_HPP
#define VSIP_CORE_CONFIG_HPP

#include <vsip/core/acconfig.hpp>

#ifdef  _MSC_VER

// Make sure we see the POSIX definitions from math.h.
# define _USE_MATH_DEFINES

#endif

#ifdef M_PI
# define VSIP_IMPL_PI M_PI
#else
# define VSIP_IMPL_PI 3.14159265358979323846
#endif

#if defined(_WIN32)
// IPP on Windows uses __stdcall for all functions.
# define VSIP_IMPL_IPP_CALL __stdcall
#else
# define VSIP_IMPL_IPP_CALL
#endif

// Remove macros that autoconf sometimes defines.
#ifdef SIZEOF_DOUBLE
#  undef SIZEOF_DOUBLE
#endif

#ifdef SIZEOF_LONG_DOUBLE
#  undef SIZEOF_LONG_DOUBLE
#endif

#ifdef SIZEOF_LONG
#  undef SIZEOF_LONG
#endif

#ifdef __GNUC__
#define ATTRIBUTE_UNUSED __attribute__((unused))
#else
#define ATTRIBUTE_UNUSED /* empty */
#endif

/***********************************************************************
  Parallel Configuration
***********************************************************************/

#if VSIP_IMPL_PAR_SERVICE == 1
// MPI

/// VSIP_DIST_LEVEL describes the implementations distribution support
/// level [dpp.distlevel]:
///
///  0: distribution of data is not support (not a parallel impl).
///  1: one dimension of data may be block distributed.
///  2: any and all dimensions of data may be block distributed.
///  3: any and all dimensions of data may be block-cyclic distributed.
#  define VSIP_DIST_LEVEL                3

#else
// Other (serial)

// In serial, Sourcery VSIPL++ supports block-cyclic distribution of data
// (across 1 processor).  While this support provides no additional
// functionality, it allows parallel programs to be compiled and run
// unchanged in serial.
#  define VSIP_DIST_LEVEL                3

#endif

#ifndef VSIP_IMPL_TUNE_MODE
/// Setting TUNE_MODE to 1 disables the tunable_thresholds.  This
/// allows benchmarks to be run without thresholds to determine
/// cross-over points, which can then be used as thresholds.
#  define VSIP_IMPL_TUNE_MODE 0
#endif

#endif // VSIP_CORE_CONFIG_HPP
