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
#include <glib.h>


struct DatabaseSyncState
{
public:
	OSyncObjTypeSink *sink;

public:
	// device data
	unsigned int m_dbId;
	std::string m_dbName;
	Linberry::RecordStateTable m_Table;

	bool m_Sync;

private:
	std::string m_Desc;

public:
	DatabaseSyncState(OSyncPluginInfo *info, const char *description);
	~DatabaseSyncState();

	std::string Map2Uid(uint32_t recordId) const;
};


struct LinberryEnvironment
{
public:
	OSyncMember *member;
	OSyncPluginInfo *info;

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
	DatabaseSyncState m_CalendarSync, m_ContactsSync, m_JournalSync, m_TodoSync;

protected:
	void DoConnect();

public:
	LinberryEnvironment(OSyncPluginInfo *info);
	~LinberryEnvironment();

	void Connect(const Linberry::ProbeResult &result);
	void Reconnect();
	void Disconnect();
	bool isConnected();

	DatabaseSyncState* GetSyncObject(OSyncChange *change);

	void ClearDirtyFlags(Linberry::RecordStateTable &table, const std::string &dbname);
	void ClearCalendarDirtyFlags();
	void ClearContactsDirtyFlags();
	void ClearJournalDirtyFlags();
	void ClearTodoDirtyFlags();
};

#endif

