///
/// \file	vcard.h
///		Conversion routines for vcards
///

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

#ifndef __LINBERRY_SYNC_VCARD_H__
#define __LINBERRY_SYNC_VCARD_H__

#include <linberry/linberry.h>
#include <stdint.h>
#include <string>
#include "vbase.h"
#include "vformat.h"

// forward declarations
class LinberryEnvironment;


//
// vCard
//
/// Class for converting between RFC 2425/2426 vCard data format,
/// and the Linberry::Contact class.
///
class vCard : public vBase
{
	// data to pass to external requests
	char *m_gCardData;	// dynamic memory returned by vformat()... can
				// be used directly by the plugin, without
				// overmuch allocation and freeing (see Extract())
	std::string m_vCardData;// copy of m_gCardData, for C++ use
	Linberry::Contact m_LinberryContact;

protected:
	void AddAddress(const char *rfc_type, const Linberry::PostalAddress &addr);
	void AddCategories(const Linberry::CategoryList &categories);
	void AddPhoneCond(const std::string &phone);
	void AddPhoneCond(const char *rfc_type, const std::string &phone);

	void ParseAddress(vAttr &adr, Linberry::PostalAddress &address);
	void ParseCategories(vAttr &cat, Linberry::CategoryList &cats);

public:
	vCard();
	~vCard();

	const std::string&	ToVCard(const Linberry::Contact &con);
	const Linberry::Contact&	ToLinberry(const char *vcal, uint32_t RecordId);

	const std::string&	GetVCard() const { return m_vCardData; }
	const Linberry::Contact&	GetLinberryContact() const { return m_LinberryContact; }

	char* ExtractVCard();

	void Clear();
};


class VCardConverter
{
	char *m_Data;
	Linberry::Contact m_Contact;
	uint32_t m_RecordId;

public:
	VCardConverter();
	explicit VCardConverter(uint32_t newRecordId);
	~VCardConverter();

	// Transfers ownership of m_Data to the caller
	char* ExtractData();

	// Parses vevent data
	bool ParseData(const char *data);

	// Linberry storage operator
	void operator()(const Linberry::Contact &rec);

	// Linberry builder operator
	bool operator()(Linberry::Contact &rec, unsigned int dbId);

	// Handles calling of the Linberry::Controller to fetch a specific
	// record, indicated by index (into the RecordStateTable).
	// Returns a g_malloc'd string of data containing the vevent20
	// data.  It is the responsibility of the caller to free it.
	// This is intended to be passed into the GetChanges() function.
	static char* GetRecordData(LinberryEnvironment *env, unsigned int dbId,
		Linberry::RecordStateTable::IndexType index);

	// Handles either adding or overwriting a calendar record,
	// given vevent20 data in data, and the proper environmebnt,
	// dbId, StateIndex.  Set add to true if adding.
	static bool CommitRecordData(LinberryEnvironment *env, unsigned int dbId,
		Linberry::RecordStateTable::IndexType StateIndex, uint32_t recordId,
		const char *data, bool add, std::string &errmsg);
};


#endif

