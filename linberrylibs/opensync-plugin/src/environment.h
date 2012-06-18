//
// \file	environment.h
//		Container / environment class for the sync module.
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

#ifndef __LINBERRY_SYNC_ENVIRONMENT_H__
#define __LINBERRY_SYNC_ENVIRONMENT_H__

#include <opensync/opensync.h>
#include <linberry/linberry.h>
#include <string>
#include "idmap.h"


struct DatabaseSyncState
{
public:
	// cache is a map of record ID to bool... the bool doesn't mean
	// anything... the mere existence of the ID means it belongs
	// in the cache
	typedef std::map<uint32_t, bool>			cache_type;

public:
	// cache data
	std::string m_CacheFilename;
	cache_type m_Cache;

	// id map data
	std::string m_MapFilename;
	idmap m_IdMap;

	// device data
	unsigned int m_dbId;
	std::string m_dbName;
	Linberry::RecordStateTable m_Table;

	bool m_Sync;

private:
	std::string m_Desc;

public:
	DatabaseSyncState(OSyncMember *pm, const char *description);
	~DatabaseSyncState();

	bool LoadCache();
	bool SaveCache();

	bool LoadMap();
	bool SaveMap();

	void CleanupMap();
	void ClearDirtyFlags();

	std::string Map2Uid(uint32_t recordId) const;
	unsigned long GetMappedRecordId(const std::string &uid);
};


struct LinberryEnvironment
{
public:
	OSyncMember *member;

	// user config data
	std::string m_ConfigData;
	uint32_t m_pin;
	bool m_DebugMode;
	std::string m_password;

	// device communication
	Linberry::IConverter m_IConverter;
	Linberry::ProbeResult m_ProbeResult;
	Linberry::Controller *m_pCon;
	Linberry::Mode::Desktop *m_pDesktop;

	// sync data
	DatabaseSyncState m_CalendarSync, m_ContactsSync;

protected:
	void DoConnect();

public:
	LinberryEnvironment(OSyncMember *pm);
	~LinberryEnvironment();

	void Connect(const Linberry::ProbeResult &result);
	void Reconnect();
	void Disconnect();

	DatabaseSyncState* GetSyncObject(OSyncChange *change);

	void ParseConfig(const char *data, int size);
	void BuildConfig();

	void ClearDirtyFlags(Linberry::RecordStateTable &table, const std::string &dbname);
	void ClearCalendarDirtyFlags();
	void ClearContactsDirtyFlags();
};

#endif

