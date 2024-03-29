///
/// \file	error.h
///		Common exception classes for the Linberry library
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

#ifndef __LINBERRY_ERROR_H__
#define __LINBERRY_ERROR_H__

#include "dll.h"
#include <stdexcept>
#include <stdint.h>

namespace Linberry {

/// \addtogroup exceptions
/// @{

//
// Error class
//
/// The base class for any future derived exceptions.
/// Can be thrown on any protocol error.
///
class BXEXPORT Error : public std::runtime_error
{
public:
	Error(const std::string &str) : std::runtime_error(str) {}
};


//
// BadPassword
//
/// A bad or unknown password when talking to the device.
/// Can be thrown in the following instances:
///
///	- no password provided and the device requests one
///	- device rejected the available password
///	- too few remaining tries left... Linberry will refuse to keep
///		trying passwords if there are fewer than
///		LINBERRY_MIN_PASSWORD_TRIES tries remaining.  In this case,
///		out_of_tries() will return true.
///
///
class BXEXPORT BadPassword : public Linberry::Error
{
	int m_remaining_tries;
	bool m_out_of_tries;

public:
	BadPassword(const std::string &str, int remaining_tries,
		bool out_of_tries)
		: Linberry::Error(str),
		m_remaining_tries(remaining_tries),
		m_out_of_tries(out_of_tries)
		{}
	int remaining_tries() const { return m_remaining_tries; }
	bool out_of_tries() const { return m_out_of_tries; }
};

//
// BadData
//
/// Thrown by record classes if their data is invalid and cannot be
/// uploaded to the Blackberry.
///
class BXEXPORT BadData : public Linberry::Error
{
public:
	BadData(const std::string &str)
		: Linberry::Error(str)
		{}
};

//
// BadSize
//
/// Unexpected packet size, or not enough data.
///
class BXEXPORT BadSize : public Linberry::Error
{
	unsigned int m_packet_size,
		m_data_buf_size,
		m_required_size;

	BXLOCAL static std::string GetMsg(const char *msg, unsigned int d, unsigned int r);
	BXLOCAL static std::string GetMsg(unsigned int p, unsigned int d, unsigned int r);

public:
	BadSize(const char *msg, unsigned int data_size, unsigned int required_size)
		: Linberry::Error(GetMsg(msg, data_size, required_size))
		, m_packet_size(0)
		, m_data_buf_size(data_size)
		, m_required_size(required_size)
		{}
	BadSize(unsigned int packet_size,
		unsigned int data_buf_size,
		unsigned int required_size)
		: Linberry::Error(GetMsg(packet_size, data_buf_size, required_size))
		, m_packet_size(packet_size)
		, m_data_buf_size(data_buf_size)
		, m_required_size(required_size)
		{}
	unsigned int packet_size() const { return m_packet_size; }
	unsigned int data_buf_size() const { return m_data_buf_size; }
	unsigned int required_size() const { return m_required_size; }
};

//
// ErrnoError
//
/// System error that provides an errno error code.
///
class BXEXPORT ErrnoError : public Linberry::Error
{
	int m_errno;

	static std::string GetMsg(const std::string &msg, int err);

public:
	ErrnoError(const std::string &msg, int err)
		: Linberry::Error(GetMsg(msg, err))
		, m_errno(err)
		{}

	int error_code() const { return m_errno; }
};

//
// BadPackedFormat
//
/// Thrown by record classes that don't recognize a given packed format code.
/// This exception is mostly handled internally, but is published here
/// just in case it escapes.
///
class BXEXPORT BadPackedFormat : public Linberry::Error
{
	uint8_t m_format;

public:
	BadPackedFormat(uint8_t format)
		: Linberry::Error("Bad packed format - internal exception")
		, m_format(format)
		{}

	uint8_t format() const { return m_format; }
};

//
// BadPacket
//
/// Thrown by the socket class if a packet command's response indicates
/// an error.  Some commands may be able to recover inside the library,
/// so a special exception is used, that includes the response code.
///
class BXEXPORT BadPacket : public Linberry::Error
{
	uint8_t m_response;

public:
	BadPacket(uint8_t response, const std::string &msg)
		: Linberry::Error(msg)
		, m_response(response)
		{}

	uint8_t response() const { return m_response; }
};

/// @}

} // namespace Linberry

#endif

