///
/// \file	version.cc
///		Provide access to library version information
///

/*
    Copyright (C) 2007-2009, Net Direct Inc. (http://www.netdirect.ca/)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the GNU General Public License in the COPYING file at the
    root directory of this project for more details.
*/

#include "version.h"
#include "config.h"

#ifdef WORDS_BIGENDIAN
#define LINBERRY_VERSION_STRING	"Linberry library version 0.16 (big endian)"
#else
#define LINBERRY_VERSION_STRING	"Linberry library version 0.16 (little endian)"
#endif

#define LINBERRY_VERSION_MAJOR	0
#define LINBERRY_VERSION_MINOR	16

namespace Linberry {

/// Fills major and minor with integer version numbers, and
/// returns a string containing human readable version
/// information in English.
const char* Version(int &major, int &minor)
{
	major = LINBERRY_VERSION_MAJOR;
	minor = LINBERRY_VERSION_MINOR;
	return LINBERRY_VERSION_STRING;
}

} // namespace Linberry

