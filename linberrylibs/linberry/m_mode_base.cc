///
/// \file	m_mode_base.cc
///		Base for mode classes
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

#include "m_mode_base.h"

namespace Linberry { namespace Mode {

//////////////////////////////////////////////////////////////////////////////
// Mode base class

Mode::Mode(Controller &con, Controller::ModeType type)
	: m_con(con)
	, m_modetype(type)
	, m_ModeSocket(0)
{
}

Mode::~Mode()
{
}

//
// Open
//
/// Select device mode.  This is required before using any other mode-based
/// operations, such as GetDBDB() and LoadDatabase().
///
/// This function opens a socket to the device for communicating in Desktop
/// mode.  If the device requires it, specify the password with a const char*
/// string in password.  The password will not be stored in memory
/// inside this class, only a hash will be generated from it.  After
/// using the hash, the hash memory will be set to 0.  The application
/// is responsible for safely handling the raw password data.
///
/// You can retry the password by catching Linberry::BadPassword and
/// calling RetryPassword() with the new password.
///
/// \exception	Linberry::Error
///		Thrown on protocol error.
///
/// \exception	std::logic_error()
///		Thrown if unsupported mode is requested, or if socket
///		already open.
///
/// \exception	Linberry::BadPassword
///		Thrown when password is invalid or if not enough retries
///		left in the device.
///
void Mode::Open(const char *password)
{
	if( m_ModeSocket ) {
		m_socket->Close();
		m_socket.reset();
		m_ModeSocket = 0;
	}

	m_ModeSocket = m_con.SelectMode(m_modetype);
	RetryPassword(password);
}

//
// RetryPassword
//
/// Retry a failed password attempt from the first call to Open().
/// Only call this function in response to Linberry::BadPassword exceptions
/// that are thrown from Open().
///
/// \exception	Linberry::Error
///		Thrown on protocol error.
///
/// \exception	std::logic_error()
///		Thrown if in unsupported mode, or if socket already open.
///
/// \exception	Linberry::BadPassword
///		Thrown when password is invalid or if not enough retries
///		left in the device.
///
void Mode::RetryPassword(const char *password)
{
	if( m_socket.get() != 0 )
		throw std::logic_error("Socket alreay open in RetryPassword");

	m_socket = m_con.m_zero.Open(m_ModeSocket, password);

	// success... perform open-oriented setup
	OnOpen();
}


void Mode::OnOpen()
{
}


// FIXME - is this necessary?  and if it is, wouldn't it be better
// in the m_jvmdebug mode class?  I'm not convinced that applications
// should have to bother with socket-level details.
//void Mode::Close()
//{
//	if( m_ModeSocket ) {
//		m_socket->Close();
//		m_socket.reset();
//		m_ModeSocket = 0;
//	}
//}

}} // namespace Linberry::Mode

