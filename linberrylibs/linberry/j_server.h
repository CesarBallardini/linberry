///
/// \file	j_server.h
///		Java Debug server classes
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

#ifndef __LINBERRYJDWP_SERVER_H__
#define __LINBERRYJDWP_SERVER_H__

#include "dll.h"
#include "j_manager.h"
#include "dp_codinfo.h"
#include "m_jvmdebug.h"
#include "threadwrap.h"
#include <string>
#include <memory>


namespace Linberry { namespace JDWP {

class BXEXPORT JDWServer
{
public:
	typedef void (*ConsoleCallbackType)(const std::string &);

private:
	Linberry::Mode::JVMDebug *jvmdebug;

	int acceptfd;
	int sockfd;

	std::string address;
	int port;

	bool loop;
	bool targetrunning;

	std::string password;

	Linberry::JVMModulesList modulesList;				// List of COD applications installed on the device
	Linberry::JDG::DebugFileList debugFileList;		// List of debug file on the host

	JDWAppList appList;								// List of BlackBerry application (an application contents several COD files)
	Linberry::JDG::ClassList visibleClassList;		// Visible class list from JDB

	std::auto_ptr<Thread> handler;
	ConsoleCallbackType printConsoleMessage;

	void CommandsetProcess(Linberry::Data &cmd);

	void CommandsetVirtualMachineProcess(Linberry::Data &cmd);
	void CommandsetEventRequestProcess(Linberry::Data &cmd);

	void CommandVersion(Linberry::Data &cmd);
	void CommandIdSizes(Linberry::Data &cmd);
	void CommandAllClasses(Linberry::Data &cmd);
	void CommandAllThreads(Linberry::Data &cmd);
	void CommandSuspend(Linberry::Data &cmd);
	void CommandResume(Linberry::Data &cmd);
	void CommandClassPaths(Linberry::Data &cmd);

	void CommandSet(Linberry::Data &cmd);

//	void BackgroundDeviceProcess();

protected:

public:
	JDWServer(Linberry::Mode::JVMDebug &device, const char *address, int port);
	~JDWServer();

	void SetPasswordDevice(std::string password);

	void SetConsoleCallback(ConsoleCallbackType callback);

	bool Start();		// starts new thread
	bool AcceptConnection();
	bool AttachToDevice();
	bool InitVisibleClassList();
	bool Hello();
	void Run();
	void DetachFromDevice();
	bool Stop();		// cancels thread if still running, and
				// cleans up Start()
};

}} // namespace Linberry::JDWP

#endif

