///
/// \file	r_message.h
///		Blackberry database record parser class for email records.
///

/*
    Copyright (C) 2005-2009, Net Direct Inc. (http://www.netdirect.ca/)

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

#ifndef __LINBERRY_RECORD_MESSAGE_H__
#define __LINBERRY_RECORD_MESSAGE_H__

#include "dll.h"
#include "r_message_base.h"

namespace Linberry {

//
// NOTE:  All classes here must be container-safe!  Perhaps add sorting
//        operators in the future.
//

/// \addtogroup RecordParserClasses
/// @{

class BXEXPORT Message : public MessageBase
{
public:
	// database name
	static const char * GetDBName() { return "Messages"; }
	static uint8_t GetDefaultRecType() { return 0; }
};

BXEXPORT inline std::ostream& operator<<(std::ostream &os, const Message &msg) {
	msg.Dump(os);
	return os;
}

/// @}

} // namespace Linberry

#endif

