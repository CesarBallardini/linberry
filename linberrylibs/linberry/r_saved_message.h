///
/// \file	r_save_message.h
///		Blackberry database record parser class for saved email
///		message records.
///

/*
    Copyright (C) 2005-2009, Net Direct Inc. (http://www.netdirect.ca/)
    Copyright (C) 2007, Brian Edginton (edge@edginton.net)

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

#ifndef __LINBERRY_RECORD_SAVED_MESSAGE_H__
#define __LINBERRY_RECORD_SAVED_MESSAGE_H__

#include "dll.h"
#include "r_message_base.h"

namespace Linberry {

//
// NOTE:  All classes here must be container-safe!  Perhaps add sorting
//        operators in the future.
//

/// \addtogroup RecordParserClasses
/// @{

class BXEXPORT SavedMessage : public MessageBase
{
public:
	// database name
	static const char * GetDBName() { return "Saved Email Messages"; }
	static uint8_t GetDefaultRecType() { return 3; }
};

BXEXPORT inline std::ostream& operator<<(std::ostream &os, const SavedMessage &msg) {
	msg.Dump(os);
	return os;
}

/// @}

} // namespace Linberry

#endif // __LINBERRY_RECORD_SAVED_MESSAGE_H__


