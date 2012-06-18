///
/// \file	BackupWindow.cc
///		GUI window class
///


#include "BackupWindow.h"
#include "PasswordDlg.h"
#include "PromptDlg.h"
#include "ConfigDlg.h"
#include "util.h"
#include <gtkmm/aboutdialog.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <unistd.h>

BackupWindow::BackupWindow(BaseObjectType *cobject,
			   const Glib::RefPtr<Gnome::Glade::Xml> &xml)
	: Gtk::Window(cobject)
	, m_xml(xml)
	, m_pStatusbar(0)
	, m_pBackupButton(0)
	, m_pcmdCerrar(0)
	, m_pRestoreButton(0)
	, m_pDisconnectButton(0)
	, m_pReloadButton(0)
	, m_pDeviceLabel(0)
	, m_pDeviceList(0)
	, m_device_count(0)
	, m_pActive(0)
	, m_scanned(false)
	//AQUI	
	, m_pProgressBar(0)
	, m_last_status_id(0)
{
	m_signal_update.connect(
		sigc::mem_fun(*this, &BackupWindow::treeview_update));


	// setup menu signals
	

	Gtk::MenuItem *pItem = 0;
	m_xml->get_widget("menu_file_quit", pItem);
	pItem->signal_activate().connect(
		sigc::mem_fun(*this, &BackupWindow::on_file_quit));

	m_xml->get_widget("menu_help_about", pItem);
	pItem->signal_activate().connect(
		sigc::mem_fun(*this, &BackupWindow::on_help_about));

	// get various widget pointers we will use later
	m_xml->get_widget("DeviceLabel", m_pDeviceLabel);
	m_xml->get_widget("DeviceList", m_pDeviceList);
	m_xml->get_widget("BackupButton", m_pBackupButton);
	m_xml->get_widget("cmdCerrar", m_pcmdCerrar);
	m_xml->get_widget("RestoreButton", m_pRestoreButton);
	m_xml->get_widget("ConfigButton", m_pConfigButton);
	m_xml->get_widget("DisconnectButton", m_pDisconnectButton);
	m_xml->get_widget("DisconnectAllButton", m_pDisconnectAllButton);
	m_xml->get_widget("ReloadButton", m_pReloadButton);
	m_xml->get_widget("Statusbar", m_pStatusbar);
	//AQUI
	m_xml->get_widget("ProgressBar", m_pProgressBar);

	// set up device list
	m_pListStore = Gtk::ListStore::create(m_columns);
	m_pDeviceList->set_model(m_pListStore);

	m_pDeviceList->append_column("PIN", m_columns.m_pin);
	m_pDeviceList->append_column("Name", m_columns.m_name);
	m_pDeviceList->append_column("Status", m_columns.m_status);
	Gtk::CellRendererProgress* cell = new Gtk::CellRendererProgress;
	m_pDeviceList->append_column("Progress", *cell);
	Gtk::TreeViewColumn* pColumn = m_pDeviceList->get_column(3);
	pColumn->add_attribute(cell->property_value(), m_columns.m_percentage);

	m_pDeviceList->get_column(0)->set_min_width(60);
	m_pDeviceList->get_column(1)->set_min_width(100);
	m_pDeviceList->get_column(2)->set_min_width(75);

	for( unsigned int i = 0; i < 4; ++i )
		m_pDeviceList->get_column(i)->set_resizable();

	// set up device list selection
	m_pDeviceSelection = m_pDeviceList->get_selection();

	// setup widget signals
	m_pBackupButton->signal_clicked().connect(
		sigc::mem_fun(*this, &BackupWindow::on_backup));
	m_pcmdCerrar->signal_clicked().connect(
		sigc::mem_fun(*this, &BackupWindow::on_cerrar));
	m_pRestoreButton->signal_clicked().connect(
		sigc::mem_fun(*this, &BackupWindow::on_restore));
	m_pConfigButton->signal_clicked().connect(
		sigc::mem_fun(*this, &BackupWindow::on_config));
	m_pDisconnectButton->signal_clicked().connect(
		sigc::mem_fun(*this, &BackupWindow::on_disconnect));
	m_pDisconnectAllButton->signal_clicked().connect(
		sigc::mem_fun(*this, &BackupWindow::on_disconnect_all));
	m_pReloadButton->signal_clicked().connect(
		sigc::mem_fun(*this, &BackupWindow::on_reload));
	m_pDeviceSelection->signal_changed().connect(
		sigc::mem_fun(*this, &BackupWindow::on_device_change));

	// setup startup device scan
	Glib::signal_timeout().connect(
		sigc::mem_fun(*this, &BackupWindow::on_startup), 500);

	// workaround: normally this should say "Ready" but since
	// the initial Scan() happens right away, and the statusbar
	// doesn't seem to update the screen until the handler is
	// finished, we update the status bar here instead
	StatusbarSet("Buscando dispositivo...");
	m_pProgressBar->set_fraction(0.00);

	// do this last so that any exceptions in the constructor
	// won't cause a connected signal handler to a non-object
	// (i.e. ~BackupWindow() won't get called if constructor throws)
	m_signal_handler_connection = Glib::add_exception_handler(
		sigc::mem_fun(*this, &BackupWindow::signal_exception_handler) );
	
}

BackupWindow::~BackupWindow()
{
	// disconnect the signal, as we're going out of business
	m_signal_handler_connection.disconnect();
}

//Agregado por israel  Marrero 28/11/2010 para cerrar aplicacion

void BackupWindow::on_cerrar()
{
	if( CheckWorking() ) {
		gtk_main_quit();
	}
	//	return false;	// allow closing of window via window manager
	//else
	//	return true;	// stop the close

		
}



void BackupWindow::Scan()
{

	StatusbarSet("Buscando dispositivo...");

	m_pListStore->clear();
	m_threads.clear();

	m_bus.Probe();
	m_device_count = m_bus.ProbeCount();

	if( m_device_count == 0 )
		m_pDeviceLabel->set_label("Sin conección!!!!!");
	else if( m_device_count == 1 )
		m_pDeviceLabel->set_label("Dispositivo Preparado...");
	else
	{
		std::ostringstream oss;
		oss << m_device_count << " dispositivos:";
		m_pDeviceLabel->set_label(oss.str());
	}

	m_threads.resize(m_device_count);
	for( unsigned int id = 0; id < m_device_count; ++id ) {
		Device dev = m_bus.Get(id);
		Gtk::TreeModel::iterator row = m_pListStore->append();
		(*row)[m_columns.m_id] = id;

		m_threads[id].reset(new Thread(dev, &m_signal_update));
	}

	// all devices loaded
	m_scanned = true;

	StatusbarSet("Dispositivo listo.");

	// if one or more device plugged in,
	// activate the first one
	Gtk::TreeModel::iterator iter = m_pListStore->children().begin();
	if( iter )
		m_pDeviceSelection->select(iter);
}

bool BackupWindow::Connect(Thread *thread)
{
	if( thread->Connected() )
		return true;

	StatusbarSet("Conectando con dispositivo...");
	static int tries(0);

	CheckDeviceName(thread);


	if( !thread->Connect() ) {
		if( thread->PasswordRequired() ) {
			bool connected = false;
			while( !connected && !thread->PasswordOutOfTries() ) {
				PasswordDlg dlg(thread->PasswordRemainingTries());
				if( dlg.run() == Gtk::RESPONSE_OK )
					connected = thread->Connect(dlg.GetPassword());
				else { // user cancelled
					thread->Reset();
					StatusbarSet("Conexión cancelada.");
					return false;
				}
			}
			if( thread->PasswordOutOfTries() )
			{
				//Gtk::MessageDialog msg(thread->BadPasswordError());
				//msg.run();
				//StatusbarSet("Imposible conectarse a " + thread->GetFullname() + ".");
				std::cout << "ERROR:password\n";
				return false;
			}
		}
		else if( thread->BadSize() ) {
			++tries;
			if( tries < 3 ) {
				std::cerr << thread->BadSizeError() << std::endl;
				thread->Reset();
				sleep(2);
				return Connect(thread);
			}
			else {
				Gtk::MessageDialog msg(thread->BadSizeError());
				//msg.run();
				std::cout << "ERROR:no conectado" << std::endl;
				StatusbarSet("Imposible conectarse a " + thread->GetFullname() + ".");
				return false;
			}
		}
		else {
			Gtk::MessageDialog msg(thread->LastInterfaceError());
			//msg.run();
			std::cout << "ERROR:no conectado2" << std::endl;
			StatusbarSet("Imposible conectarse a " + thread->GetFullname() + ".");
			return false;
		}
	}
	tries = 0;
	//StatusbarSet("Conectado a " + thread->GetFullname() + ".");
	StatusbarSet("Conectado...");

//agregado por israel para que haga el respaldo directamente...

	//std::cout << "\n\npruebita" << thread->GetRestoreFile() << "\n\n\n" ;
		
		on_restore();

//on_backup();
	return true;
}

void BackupWindow::Disconnect(Thread *thread)
{
	if( thread->Working() ) {
		Gtk::MessageDialog dialog(*this, thread->GetFullname() + " is working, "
			"disconnecting from it may cause data corruption, are you sure to proceed?",
			false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
		if( dialog.run() == Gtk::RESPONSE_CANCEL )
			return;
	}

	if( thread->Connected() ) {
		thread->Disconnect();
		StatusbarSet("Disconnected from " + thread->GetFullname() + ".");
	}
	else {
		StatusbarSet("sin conexión.");
	}
}

void BackupWindow::CheckDeviceName(Thread *thread)
{
		
	if( !thread->HasDeviceName() ) {
		//Modificaciones hechas por Israel
		//PromptDlg dlg;
		//dlg.SetPrompt("Unnamed device found (PIN: " + thread->GetPIN().str() + ").\r\nPlease enter a name for it:");
		//if( dlg.run() == Gtk::RESPONSE_OK ) {
		//	thread->SetDeviceName(dlg.GetAnswer());
		//}
		//else {
			thread->SetDeviceName(" ");

		//}
		if( !thread->Save() ) {
			Gtk::MessageDialog msg("Imposible salvar configuración: " +
				thread->LastConfigError());
			//msg.run();
			std::cout << "ERROR:save config" << std::endl;
		}
		// Agregé esta linea para que pida la configuración la primera vez
		on_config();

	}
}

Thread *BackupWindow::GetActive()
{
	Gtk::TreeModel::iterator row = m_pDeviceSelection->get_selected();
	if( row ) {
		unsigned int id = (*row)[m_columns.m_id];
		return m_threads[id].get();
	}
	else {
		return 0;
	}
}

void BackupWindow::StatusbarSet(const Glib::ustring& text)
{
	guint remove_id = m_last_status_id;
	if( m_pStatusbar ) {
		m_last_status_id = m_pStatusbar->push(text);
		if( remove_id )
			m_pStatusbar->remove_message(remove_id);
	}
}

/// Returns true if ok to proceed (either nothing is currently 'working',
/// or the user confirmed to do it anyway)
bool BackupWindow::CheckWorking()
{
	bool working(false);
	for( unsigned int i = 0; i < m_device_count; ++i) {
		if( m_threads[i]->Working() ) {
			working = true;
			break;
		}
	}

	if( working ) {
		//Gtk::MessageDialog dialog(*this, "Actualmente hay un proceso en ejecución, por favor espere... ",
		//	false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
		//if( dialog.run() != Gtk::RESPONSE_OK )
		//	return false;
		//Gtk::MessageDialog msg("hay un proceso ejecución, por favor espere...");
		//msg.run();
		return false;
	}

	return true;
}

void BackupWindow::signal_exception_handler()
{
	try {
		throw;
	}
	catch( Glib::Exception &e ) {
		// This usually just means a missing .glade file,
		// so we try to carry on.
//		std::cerr << "Glib::Exception caught in main: " << std::endl;
//		std::cerr << e.what() << std::endl;
//		Gtk::MessageDialog msg(e.what());
//		msg.run();
		std::cout << "ERROR:glade\n";

	}
	catch( ... ) {
		// anything else, terminate window and pass on to next handler
		// (which should be in main.cc)
		hide();
		throw;
	}
}


//////////////////////////////////////////////////////////////////////////////
// signal handlers

void BackupWindow::treeview_update()
{
	for( Gtk::TreeModel::iterator i = m_pListStore->children().begin();
		i != m_pListStore->children().end(); ++i ) {
		unsigned int id = (*i)[m_columns.m_id];
		Thread *thread = m_threads[id].get();
		(*i)[m_columns.m_pin] = thread->GetPIN().str();
		(*i)[m_columns.m_name] = thread->GetDeviceName();
		(*i)[m_columns.m_status] = thread->Status();
		unsigned int finished(thread->GetRecordFinished()), total(thread->GetRecordTotal());
		unsigned int percentage(0);
		if( total == 0 || finished == total )
			percentage = 0;
		else {
			percentage = 100 * finished / total;
			if( percentage == 100 ) // never say 100% unless finished
				percentage = 99.9;
		}
		(*i)[m_columns.m_percentage] = percentage;
		m_pProgressBar->set_fraction(percentage * 0.01);
		//m_pStatusbar->push(percentage);

		if( thread->CheckFinishedMarker() ) {
			std::string op;

			if( thread->GetThreadState() & THREAD_STATE_BACKUP )
				op = "Respaldo";
			else if( thread->GetThreadState() & THREAD_STATE_RESTORE )
				op = "Proceso de restauración";
			else
				op = "Proceso";

			//StatusbarSet(op + " en " + thread->GetFullname() + " finalizado");
			//StatusbarSet(op + " finalizado.");
			//if( (thread->GetThreadState() & THREAD_STATE_BACKUP) &&
			  //   finished != total )
			if( (thread->GetThreadState() & THREAD_STATE_BACKUP)) 
			//     finished != total )
			{
				// in some cases, not all records are backed
				// up, possibly due to international chars
				// in the Blackberry data which somehow
				// forces the device to use a different
				// low level protocol... here we need to
				// warn the user that not all data was
				// included in the backup
				//std::ostringstream oss;
				//oss << "¡Respaldo finalizado!";
				//Gtk::MessageDialog msg(oss.str());
				//msg.run();
				std::cout << "Finalizado" << std::endl;
				on_cerrar();
				m_pProgressBar->set_fraction(0.00);
			}
			if( (thread->GetThreadState() & THREAD_STATE_RESTORE)) 
			//     finished != total )
			{
				// in some cases, not all records are backed
				// up, possibly due to international chars
				// in the Blackberry data which somehow
				// forces the device to use a different
				// low level protocol... here we need to
				// warn the user that not all data was
				// included in the backup
				std::cout << "Finalizado" << std::endl;
				std::ostringstream oss;
				oss << "¡Restauración finalizada!";
				//Gtk::MessageDialog msg(oss.str());
				//msg.run();
				on_cerrar();
				m_pProgressBar->set_fraction(0.00);
			}

		}
	}
}






void BackupWindow::on_backup()
{
	Thread *thread = GetActive();

	if( !thread || !Connect(thread) )
		return;

	// already working?
	if( thread->Working() ) {
		//Gtk::MessageDialog msg("ya hay un proceso en ejecución, espere a que temine...");
		//msg.run();
		return;
	}

	// make sure our target directory exists
	if( !::CheckPath(thread->GetPath()) ) {
		Gtk::MessageDialog msg("Imposible crear carpeta: " + thread->GetPath());
		//msg.run();
		std::cout << "ERROR:carpeta" << std::endl;
		return;
	}

	// anything to do?
	if( thread->GetBackupList().size() == 0 ) {
		Gtk::MessageDialog msg("¡No hay bases de datos configuradas! Debe ir a configuración y seleccionar por lo menos una base de datos RIM...");
		//msg.run();
	std::cout << "ERROR:basedatos" << std::endl;
		return;
	}

	// prompt for a backup label, if so configured
	std::string backupLabel;
	if( thread->PromptBackupLabel() ) {
		PromptDlg dlg;
		dlg.SetPrompt("Defina un nombre para este respaldo, si deja el campo en blanco se asignará el nombre estandar:");
		if( dlg.run() == Gtk::RESPONSE_OK ) {
			backupLabel = dlg.GetAnswer();
		}
		else {
			// user cancelled
			return;
		}
	}

	// start the thread
	if( !thread->Backup(backupLabel) ) {
		Gtk::MessageDialog msg("Error!!! imposible respaldar: " +
			thread->LastInterfaceError());
		//msg.run();
		std::cout << "ERROR:nopuedorespaldar" << std::endl;
	}
	else {
		//StatusbarSet("Backup of " + thread->GetFullname() + " in progress...");
		StatusbarSet("Se está haciendo el respaldo");
	}
}

bool BackupWindow::PromptForRestoreTarball(std::string &restoreFilename,
					   const std::string &start_path)
{
	char buffer[PATH_MAX];
	char *buf = getcwd(buffer, PATH_MAX);

	// start at the base path given... if it fails, just open
	// the dialog where we are
	chdir(start_path.c_str());
	
	


	Gtk::FileChooserDialog dlg(*this, "Seleccione el respaldo para restaurar");
	dlg.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
	dlg.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	int result = dlg.run();

	if( buf )
		chdir(buf);

	if( result != Gtk::RESPONSE_OK )
		return false;

	restoreFilename = dlg.get_filename();
	return true;
}

void BackupWindow::on_restore()
{
	Thread *thread = GetActive();

	
	if( !thread || !Connect(thread) )
		return;

	// already working?
	if( thread->Working() ) {
		Gtk::MessageDialog msg("Hay un proceso ejecutandose.");
		//msg.run();
		std::cout << "ERROR:proceso ejecutandose" << std::endl;
		return;
	}

	std::string restoreFilename;
	//atrevimiento por parte de israel	
	//***************************************************************
	//	if( !PromptForRestoreTarball(restoreFilename, thread->GetPath()) )
	//		return;	// nothing to do
	restoreFilename = thread->GetRestoreFile();
	//**************************************************************
	//fin atrevimiento

	// start the thread
	// if( !thread->RestoreAndBackup(restoreFilename) ) {

	

	if( !thread->Restore(restoreFilename) ) {
		Gtk::MessageDialog msg("Error!!! no se puede restaurar: " +
			thread->LastInterfaceError());
		//msg.run();
		std::cout << "ERROR:no puedo restaurar" << std::endl;
	}
	else {
		//StatusbarSet("Restore of " + thread->GetFullname() + " in progress...");
		StatusbarSet("Proceso de restauración en progreso...");
	}
}

void BackupWindow::on_disconnect()
{
	Thread *thread = GetActive();
	if( thread )
		Disconnect(thread);
}

void BackupWindow::on_disconnect_all()
{
	for( unsigned int i = 0; i < m_device_count; ++i )
		Disconnect(m_threads[i].get());
}

void BackupWindow::on_device_change()
{
	Thread *thread = GetActive();
	if( m_pActive )
		m_pActive->UnsetActive();
	m_pActive = thread;
	if( thread && Connect(thread) )
		thread->SetActive();
}

void BackupWindow::on_config()
{
	Thread *thread = GetActive();

	if( !thread || !Connect(thread) )
		return;

	thread->LoadConfig();

	ConfigDlg dlg(thread->GetDBDB(), *thread);
	if( dlg.run() == Gtk::RESPONSE_OK ) {
		thread->SetBackupList(dlg.GetBackupList());
		thread->SetRestoreList(dlg.GetRestoreList());
		thread->SetDeviceName(dlg.GetDeviceName());
		thread->SetBackupPath(dlg.GetBackupPath());
		thread->SetPromptBackupLabel(dlg.GetPromptBackupLabel());
		if( !thread->Save() )
			StatusbarSet("Errro!!! Archivo de configuración" +
				thread->LastConfigError());
		else
			StatusbarSet("configuración aplicada...");
	}
	thread->LoadConfig();
}

void BackupWindow::on_reload()
{
	if( CheckWorking() ) {
		Scan();
	}
}

void BackupWindow::on_file_quit()
{
	if( CheckWorking() ) {
		hide();
	}
}

void BackupWindow::on_help_about()
{
	Gtk::AboutDialog dlg;
	dlg.set_copyright("Copyright (C) 2007-2009, Net Direct Inc.");
	dlg.set_license(
"    This program is free software; you can redistribute it and/or modify\n"
"    it under the terms of the GNU General Public License as published by\n"
"    the Free Software Foundation; either version 2 of the License, or\n"
"    (at your option) any later version.\n"
"\n"
"    This program is distributed in the hope that it will be useful,\n"
"    but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
"\n"
"    See the GNU General Public License in the COPYING file at the\n"
"    root directory of this project for more details.\n");

	std::vector<std::string> authors;
	authors.push_back("pas");
	authors.push_back("pas");
	authors.push_back("pas");

	dlg.set_authors(authors);

	int major, minor;
	const char *LinberryVersion = Linberry::Version(major, minor);
	dlg.set_name("linberry");
	dlg.set_version("0.16");
	dlg.set_comments(std::string("Using library: ") + LinberryVersion);
	dlg.set_website("http://linberry.webcindario.com");
	dlg.run();
}

bool BackupWindow::on_startup()
{
	
	Scan();
	return false;
}

bool BackupWindow::on_delete_event(GdkEventAny *)
{
	if( CheckWorking() )
		return false;	// allow closing of window via window manager
	else
		return true;	// stop the close
}

