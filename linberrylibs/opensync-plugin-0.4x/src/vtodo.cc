//
// \file	vtodo.cc
//		Conversion routines for vtodos (VCALENDAR, etc)
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

#include <opensync/opensync.h>
#include <opensync/opensync-time.h>

#include "vtodo.h"
#include "environment.h"
#include "trace.h"
#include "tosserror.h"
#include "vformat.h"		// comes from opensync, but not a public header yet
#include <stdint.h>
#include <glib.h>
#include <string.h>
#include <sstream>


//////////////////////////////////////////////////////////////////////////////
// Utility functions

static void ToLower(std::string &str)
{
	size_t i = 0;
	while( i < str.size() ) {
		str[i] = tolower(str[i]);
		i++;
	}
}

//////////////////////////////////////////////////////////////////////////////
// vTodo

vTodo::vTodo()
	: m_gTodoData(0)
{
}

vTodo::~vTodo()
{
	if( m_gTodoData ) {
		g_free(m_gTodoData);
	}
}

bool vTodo::HasMultipleVTodos() const
{
	int count = 0;
	b_VFormat *format = const_cast<b_VFormat*>(Format());
	GList *attrs = format ? b_vformat_get_attributes(format) : 0;
	for( ; attrs; attrs = attrs->next ) {
		b_VFormatAttribute *attr = (b_VFormatAttribute*) attrs->data;
		if( strcasecmp(b_vformat_attribute_get_name(attr), "BEGIN") == 0 &&
		    strcasecmp(b_vformat_attribute_get_nth_value(attr, 0), "VTODO") == 0 )
		{
			count++;
		}
	}
	return count > 1;
}


// Main conversion routine for converting from Linberry::Task to
// a vTodo string of data.
const std::string& vTodo::ToTask(const Linberry::Task &task)
{
	Trace trace("vTodo::ToTask");
	std::ostringstream oss;
	task.Dump(oss);
	trace.logf("ToTask, initial Linberry record: %s", oss.str().c_str());

	// start fresh
	Clear();
	SetFormat( b_vformat_new() );
	if( !Format() )
		throw ConvertError("resource error allocating vformat");

	// store the Linberry object we're working with
	m_LinberryTask = task;

	// begin building vCalendar data
	AddAttr(NewAttr("PRODID", "-//OpenSync//NONSGML Linberry Task Record//EN"));
	AddAttr(NewAttr("BEGIN", "VTODO"));
	AddAttr(NewAttr("SEQUENCE", "0"));
	AddAttr(NewAttr("SUMMARY", task.Summary.c_str()));
	AddAttr(NewAttr("DESCRIPTION", task.Notes.c_str()));
	AddAttr(NewAttr("CATEGORIES", ToStringList(task.Categories).c_str()));

	// Status
	if (task.StatusFlag == Linberry::Task::InProgress)
		AddAttr(NewAttr("STATUS", "IN-PROCESS"));
	else if (task.StatusFlag == Linberry::Task::Completed)
		AddAttr(NewAttr("STATUS", "COMPLETED"));
	else if (task.StatusFlag == Linberry::Task::Deferred)
		AddAttr(NewAttr("STATUS", "CANCELLED"));
	else if (task.StatusFlag == Linberry::Task::Waiting)
		AddAttr(NewAttr("STATUS", "NEEDS-ACTION"));

	// Priority
	if (task.PriorityFlag == Linberry::Task::High)
		AddAttr(NewAttr("PRIORITY", "3"));
	else if (task.PriorityFlag == Linberry::Task::Normal)
		AddAttr(NewAttr("PRIORITY", "5"));
	else
		AddAttr(NewAttr("PRIORITY", "7"));

	// StartTime
	if( task.StartTime ) {
		TossError te("ToTask, StartTime 2 vtime", trace);
		gStringPtr start(osync_time_unix2vtime(&task.StartTime, te));
		AddAttr(NewAttr("DTSTART", start.Get()));
	}

	// DueTime DueFlag
	if( task.DueDateFlag ) {
		TossError te("ToTask, DueTime 2 vtime", trace);
		gStringPtr due(osync_time_unix2vtime(&task.DueTime, te));
		AddAttr(NewAttr("DUE", due.Get()));
	}

	// FIXME - add a truly globally unique "UID" string?

	AddAttr(NewAttr("END", "VTODO"));

	// generate the raw VTODO data
	m_gTodoData = b_vformat_to_string(Format(), VFORMAT_TODO_20);
	m_vTodoData = m_gTodoData;

	trace.logf("ToTask, resulting vtodo data: %s", m_vTodoData.c_str());
	return m_vTodoData;
}

// Main conversion routine for converting from vTodo data string
// to a Linberry::Task object.
const Linberry::Task& vTodo::ToLinberry(const char *vtodo, uint32_t RecordId)
{
	using namespace std;

	Trace trace("vTodo::ToLinberry");
	trace.logf("ToLinberry, working on vtodo data: %s", vtodo);

	// we only handle vTodo data with one vtodo block
	if( HasMultipleVTodos() )
		throw ConvertError("vCalendar data contains more than one VTODO block, unsupported");

	// start fresh
	Clear();

	// store the vTodo raw data
	m_vTodoData = vtodo;

	// create format parser structures
	SetFormat( b_vformat_new_from_string(vtodo) );
	if( !Format() )
		throw ConvertError("resource error allocating vtodo");

	string summary = GetAttr("SUMMARY", "/vtodo");
	trace.logf("SUMMARY attr retrieved: %s", summary.c_str());
	if( summary.size() == 0 ) {
		summary = "<blank subject>";
		trace.logf("ERROR: bad data, blank SUMMARY: %s", vtodo);
	}

	string notes = GetAttr("DESCRIPTION", "/vtodo");
	trace.logf("DESCRIPTION attr retrieved: %s", notes.c_str());

	string status = GetAttr("STATUS", "/vtodo");
	trace.logf("STATUS attr retrieved: %s", status.c_str());

	string priority = GetAttr("PRIORITY", "/vtodo");
	trace.logf("PRIORITY attr retrieved: %s", priority.c_str());

	string start = GetAttr("DTSTART", "/vtodo");
	trace.logf("DTSTART attr retrieved: %s", start.c_str());

	string due = GetAttr("DUE", "/vtodo");
	trace.logf("DUE attr retrieved: %s", due.c_str());


	//
	// Now, run checks and convert into Linberry object
	//

	// FIXME - we are assuming that any non-UTC timestamps
	// in the vcalendar record will be in the current timezone...
	// This is wrong!  fix this later.
	//
	// Also, we current ignore any time zone
	// parameters that might be in the vcalendar format... this
	// must be fixed.
	//
	time_t now = time(NULL);
	TossError te("ToLinberry, local timezone detection", trace);
	int zoneoffset = osync_time_timezone_diff(localtime(&now), te);


	Linberry::Task &rec = m_LinberryTask;
	rec.SetIds(Linberry::Task::GetDefaultRecType(), RecordId);

	// Categories

	rec.Categories = GetValueVector("CATEGORIES","/vtodo");

	// SUMMARY & DESCRIPTION fields
	rec.Summary = summary;
	rec.Notes = notes;

	// STATUS field
	if (status.size()) {
		ToLower(status);

		const char *s = status.c_str();

		if (strstr(s, "in-process"))
			rec.StatusFlag = Linberry::Task::InProgress;
		else if (strstr(s, "completed"))
			rec.StatusFlag = Linberry::Task::Completed;
		else if (strstr(s, "cancelled"))
			rec.StatusFlag = Linberry::Task::Deferred;
		else if (strstr(s, "needs-action"))
			rec.StatusFlag = Linberry::Task::Waiting;
		else
			rec.StatusFlag = Linberry::Task::NotStarted;
	}

	// PRIORITY field
	if (priority.size()) {
		ToLower(priority);

		const char *s = priority.c_str();

		const int val = atoi(s);

		if (val < 4)
			rec.PriorityFlag = Linberry::Task::High;
		else if (val < 7)
			rec.PriorityFlag = Linberry::Task::Normal;
		else
			rec.PriorityFlag = Linberry::Task::Low;
	}


	// STARTTIME & DUETIME
	if (start.size()) {
		TossError te("ToLinberry, StartTime 2 unix", trace);
		rec.StartTime = osync_time_vtime2unix(start.c_str(), zoneoffset, te);
	}

	if (due.size()) {
		TossError te("ToLinberry, DueTime 2 unix", trace);
		rec.DueDateFlag = true;
		rec.DueTime = osync_time_vtime2unix(due.c_str(), zoneoffset, te);
	}

	std::ostringstream oss;
	m_LinberryTask.Dump(oss);
	trace.logf("ToLinberry, resulting Linberry record: %s", oss.str().c_str());
	return m_LinberryTask;
}

// Transfers ownership of m_gTaskData to the caller.
char* vTodo::ExtractVTodo()
{
	char *ret = m_gTodoData;
	m_gTodoData = 0;
	return ret;
}

void vTodo::Clear()
{
	vBase::Clear();
	m_vTodoData.clear();
	m_LinberryTask.Clear();

	if( m_gTodoData ) {
		g_free(m_gTodoData);
		m_gTodoData = 0;
	}
}



//////////////////////////////////////////////////////////////////////////////
//

VTodoConverter::VTodoConverter()
	: m_Data(0)
{
}

VTodoConverter::VTodoConverter(uint32_t newRecordId)
	: m_Data(0),
	m_RecordId(newRecordId)
{
}

VTodoConverter::~VTodoConverter()
{
	if( m_Data )
		g_free(m_Data);
}

// Transfers ownership of m_Data to the caller
char* VTodoConverter::ExtractData()
{
	Trace trace("VTodoConverter::ExtractData");
	char *ret = m_Data;
	m_Data = 0;
	return ret;
}

bool VTodoConverter::ParseData(const char *data)
{
	Trace trace("VTodoConverter::ParseData");

	try {

		vTodo vtodo;
		m_Task = vtodo.ToLinberry(data, m_RecordId);

	}
	catch( vTodo::ConvertError &ce ) {
		trace.logf("ERROR: vTodo::ConvertError exception: %s", ce.what());
		return false;
	}

	return true;
}

// Linberry storage operator
void VTodoConverter::operator()(const Linberry::Task &rec)
{
	Trace trace("VTodoConverter::operator()");

	// Delete data if some already exists
	if( m_Data ) {
		g_free(m_Data);
		m_Data = 0;
	}

	try {

		vTodo vtodo;
		vtodo.ToTask(rec);
		m_Data = vtodo.ExtractVTodo();

	}
	catch( vTodo::ConvertError &ce ) {
		trace.logf("ERROR: vTodo::ConvertError exception: %s", ce.what());
	}
}

// Linberry builder operator
bool VTodoConverter::operator()(Linberry::Task &rec, unsigned int dbId)
{
	Trace trace("VTodoConverter::builder operator()");

	rec = m_Task;
	return true;
}

// Handles calling of the Linberry::Controller to fetch a specific
// record, indicated by index (into the RecordStateTable).
// Returns a g_malloc'd string of data containing the vevent20
// data.  It is the responsibility of the caller to free it.
// This is intended to be passed into the GetChanges() function.
char* VTodoConverter::GetRecordData(LinberryEnvironment *env, unsigned int dbId,
				Linberry::RecordStateTable::IndexType index)
{
	Trace trace("VTodoConverter::GetRecordData()");

	using namespace Linberry;

	VTodoConverter task2todo;
	RecordParser<Task, VTodoConverter> parser(task2todo);
	env->m_pDesktop->GetRecord(dbId, index, parser);
	return task2todo.ExtractData();
}

bool VTodoConverter::CommitRecordData(LinberryEnvironment *env, unsigned int dbId,
	Linberry::RecordStateTable::IndexType StateIndex, uint32_t recordId,
	const char *data, bool add, std::string &errmsg)
{
	Trace trace("VTodoConverter::CommitRecordData()");

	uint32_t newRecordId;
	if( add ) {
		// use given id if possible
		if( recordId && !env->m_TodoSync.m_Table.GetIndex(recordId) ) {
			// recordId is unique and non-zero
			newRecordId = recordId;
		}
		else {
			trace.log("Can't use recommended recordId, generating new one.");
			newRecordId = env->m_TodoSync.m_Table.MakeNewRecordId();
		}
	}
	else {
		newRecordId = env->m_TodoSync.m_Table.StateMap[StateIndex].RecordId;
	}
	trace.logf("newRecordId: %lu", newRecordId);

	VTodoConverter convert(newRecordId);
	if( !convert.ParseData(data) ) {
		std::ostringstream oss;
		oss << "unable to parse change data for new RecordId: "
		    << newRecordId << " data: " << data;
		errmsg = oss.str();
		trace.logf(errmsg.c_str());
		return false;
	}

	Linberry::RecordBuilder<Linberry::Task, VTodoConverter> builder(convert);

	if( add ) {
		trace.log("adding record");
		env->m_pDesktop->AddRecord(dbId, builder);
	}
	else {
		trace.log("setting record");
		env->m_pDesktop->SetRecord(dbId, StateIndex, builder);
		trace.log("clearing dirty flag");
		env->m_pDesktop->ClearDirty(dbId, StateIndex);
	}

	return true;
}

