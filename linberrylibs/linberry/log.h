///
/// \file	log.h
///		General header for the Linberry library
///

/*
    Copyright (C) 2008-2009, Net Direct Inc. (http://www.netdirect.ca/)

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

#ifndef __LINBERRY_LOG_H__
#define __LINBERRY_LOG_H__

#include "dll.h"
#include <iomanip>

namespace Linberry {

//
// LogLock
//
/// RAII locking class used to protect the logStream passed into
/// Linberry::Init() (common.h).  If the application uses the same stream for
/// its own logging, it should use this lock class, or use the macros
/// in log.h.
///
class BXEXPORT LogLock
{
public:
	LogLock();
	~LogLock();
};

BXEXPORT bool LogVerbose();
BXEXPORT std::ostream* GetLogStream();

} // namespace Linberry

#define linberrylog(x)	{ Linberry::LogLock lock; (*Linberry::GetLogStream()) << x << std::endl; }

// controlled by command line -v switch
#define linberryverbose(x)	if(Linberry::LogVerbose()) { Linberry::LogLock lock; (*Linberry::GetLogStream()) << x << std::endl; }

#endif // __LINBERRY_LOG_H__

