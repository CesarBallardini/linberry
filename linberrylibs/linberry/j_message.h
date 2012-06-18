///
/// \file	j_message.h
///		JDWP message classes
///

/*
    Copyright (C) 2009, Nicolas VIVIEN

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

#ifndef __LINBERRYJDWP_MESSAGE_H__
#define __LINBERRYJDWP_MESSAGE_H__

#include "j_jdwp.h"

namespace Linberry { namespace JDWP {

class JDWMessage
{
private:
	int m_socket;

	JDWP::JDWP m_jdwp;

	void RawSend(Linberry::Data &send, int timeout = -1);
	bool RawReceive(Linberry::Data &receive, int timeout = -1);

protected:

public:
	JDWMessage(int socket);
	~JDWMessage();

	void Send(Linberry::Data &send, int timeout = -1);	// send only
	void Send(Linberry::Data &send, Linberry::Data &receive, int timeout = -1); // send+recv
	bool Receive(Linberry::Data &receive, int timeout = -1);
};

}} // namespace Linberry::JDWP

#endif

