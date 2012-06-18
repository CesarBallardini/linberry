///
/// \file	ConfigDlg.cc
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

#include "ConfigDlg.h"
#include "DatabaseSelectDlg.h"
#include "util.h"
#include <linberry/linberry.h>

ConfigDlg::ConfigDlg(const Linberry::DatabaseDatabase &dbdb,
		     const ConfigFile &config)
	: m_dbdb(dbdb)
	, m_backupList(config.GetBackupList())
	, m_restoreList(config.GetRestoreList())
	, m_backupPath(config.GetPath())
{
	Glib::RefPtr<Gnome::Glade::Xml> xml = LoadXml("ConfigDlg.glade");

	Gtk::Dialog *pD = 0;
	xml->get_widget("ConfigDlg", pD);
	m_pDialog.reset(pD);

	xml->get_widget("DeviceName", m_pDeviceNameEntry);
	m_pDeviceNameEntry->set_text(config.GetDeviceName());

	xml->get_widget("BackupPath", m_pBackupPath);
	m_pBackupPath->set_filename(config.GetPath());

	xml->get_widget("PromptBackupLabel", m_pPromptBackupLabelCheck);
	m_pPromptBackupLabelCheck->set_active(config.PromptBackupLabel());

	Gtk::Button *pButton = 0;
	xml->get_widget("configure_backup", pButton);
	pButton->signal_clicked().connect(
		sigc::mem_fun(*this, &ConfigDlg::on_configure_backup));

	pButton = 0;
	xml->get_widget("configure_restore", pButton);
	pButton->signal_clicked().connect(
		sigc::mem_fun(*this, &ConfigDlg::on_configure_restore));
}

ConfigDlg::~ConfigDlg()
{
}

int ConfigDlg::run()
{
	int r_val = m_pDialog->run();
	m_deviceName = m_pDeviceNameEntry->get_text();
	m_backupPath = m_pBackupPath->get_filename();
	m_promptBackupLabel = m_pPromptBackupLabelCheck->get_active();
	return r_val;
}

void ConfigDlg::on_configure_backup()
{
	DatabaseSelectDlg dlg(m_dbdb, m_backupList,
		"Marque las bases de datos RIM que desea sean respaldadas en el archivo de backup. (Se recomienda que marque todas)");
	if( dlg.run() == Gtk::RESPONSE_OK ) {
		m_backupList = dlg.GetSelections();
	}
}

void ConfigDlg::on_configure_restore()
{
	DatabaseSelectDlg dlg(m_dbdb, m_restoreList,
		"Marque las bases de datos que desea recuperar del archivo de backup.  ");
	if( dlg.run() == Gtk::RESPONSE_OK ) {
		m_restoreList = dlg.GetSelections();
	}
}

