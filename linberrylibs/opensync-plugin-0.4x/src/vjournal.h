//
// \file	vjournal.h
//		Conversion routines for vjournals (VCALENDAR, etc)
//

/*
    Copyright (C) 2008-2009, Nicolas VIVIEN
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

#ifndef __LINBERRY_SYNC_VJOURNAL_H__
#define __LINBERRY_SYNC_VJOURNAL_H__

#include <linberry/linberry.h>
#include <stdint.h>
#include <string>
#include "vbase.h"
#include "vformat.h"


// forward declarations
class LinberryEnvironment;


//
// vJournal
//
/// Class for converting between RFC 2445 iCalendar data format,
/// and the Linberry::Memo class.
///
class vJournal : public vBase
{
	// data to pass to external requests
	char *m_gJournalData;	// dynamic memory returned by vformat()... can
				// be used directly by the plugin, without
				// overmuch allocation and freeing (see Extract())
	std::string m_vJournalData;	// copy of m_gJournalData, for C++ use
	Linberry::Memo m_LinberryMemo;

protected:
	bool HasMultipleVJournals() const;

public:
	vJournal();
	~vJournal();

	const std::string&	ToMemo(const Linberry::Memo &memo);
	const Linberry::Memo&	ToLinberry(const char *vjournal, uint32_t RecordId);

	char* ExtractVJournal();

	void Clear();
};


class VJournalConverter
{
	char *m_Data;
	Linberry::Memo m_Memo;
	uint32_t m_RecordId;

public:
	VJournalConverter();
	explicit VJournalConverter(uint32_t newRecordId);
	~VJournalConverter();

	// Transfers ownership of m_Data to the caller
	char* ExtractData();

	// Parses vjournal data
	bool ParseData(const char *data);

	// Linberry storage operator
	void operator()(const Linberry::Memo &rec);

	// Linberry builder operator
	bool operator()(Linberry::Memo &rec, unsigned int dbId);

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

