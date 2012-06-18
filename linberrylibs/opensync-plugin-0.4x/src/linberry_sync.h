//
// \file	linberry_sync.h
//		Opensync module for the USB Blackberry handheld
//

/*
    Copyright (C) 2006-2009, Net Direct Inc. (http://www.netdirect.ca/)

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

#ifndef __LINBERRY_SYNC_H__
#define __LINBERRY_SYNC_H__

#include <linberry/linberry.h>
#include <string>

class LinberryEnvironment;

typedef char* (*GetData_t)(LinberryEnvironment *env, unsigned int dbId,
	Linberry::RecordStateTable::IndexType);
typedef bool (*CommitData_t)(LinberryEnvironment *env, unsigned int dbId,
	Linberry::RecordStateTable::IndexType StateIndex, uint32_t recordId,
	const char *data, bool add, std::string &errmsg);

#endif

