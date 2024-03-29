///
/// \file	ConfigDlg.h
///		Dialog wrapper class for user selection of device databases
///

/*
    Copyright (C) 2007-2009, Net Direct Inc. (http://www.netdirect.ca/)

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

#ifndef __LINBERRYBACKUP_CONFIGDLG_H__
#define __LINBERRYBACKUP_CONFIGDLG_H__

#include <gtkmm.h>
#include <memory>
#include "ConfigFile.h"

namespace Linberry {
	class DatabaseDatabase;
}

class ConfigDlg
{
	// external data
	const Linberry::DatabaseDatabase &m_dbdb;

	// Widgets
	std::auto_ptr<Gtk::Dialog> m_pDialog;
	Gtk::Entry *m_pDeviceNameEntry;
	Gtk::CheckButton *m_pPromptBackupLabelCheck;
	Gtk::FileChooserButton *m_pBackupPath;

	// config data
	ConfigFile::DBListType m_backupList;
	ConfigFile::DBListType m_restoreList;
	std::string m_deviceName;
	std::string m_backupPath;
	bool m_promptBackupLabel;

public:
	ConfigDlg(const Linberry::DatabaseDatabase &dbdb,
		const ConfigFile &config);
	~ConfigDlg();

	const ConfigFile::DBListType& GetBackupList() const { return m_backupList; }
	const ConfigFile::DBListType& GetRestoreList() const { return m_restoreList; }
	const std::string& GetDeviceName() const { return m_deviceName; }
	const std::string& GetBackupPath() const { return m_backupPath; }
	bool GetPromptBackupLabel() const { return m_promptBackupLabel; }

	int run();

	// signals
	void on_configure_backup();
	void on_configure_restore();
};

#endif

