///
/// \file	addcontact.cc
///		Example code using the Linberry library to add a contact
///		to a Blackberry device.
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

#include <linberry/linberry.h>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace Linberry;


void ReadLine(const char *prompt, std::string &data)
{
	cout << prompt << ": ";
	getline(cin, data);
}

void ReadDate(const char *prompt, Linberry::Date &date)
{
	string datestr;

	for( ;; ) {
		ReadLine(prompt, datestr);
		if( !datestr.size() ) {
			// blank
			date.Clear();
			break;
		}

		if( date.FromYYYYMMDD(datestr) )
			break;

		cout << "Unable to parse date, try again" << endl;
	}
}

void ReadInput(Linberry::Contact &contact)
{
	// ReadLine("FirstName", contact.FirstName);
	//ReadLine("LastName", contact.LastName);
	//ReadLine("Fax", contact.Fax);
	//ReadLine("Main Phone Number", contact.Phone);
	//ReadLine("Home Phone Number", contact.HomePhone);
	//ReadLine("Work Phone Number", contact.WorkPhone);
	//ReadLine("Fax Number", contact.Fax);
	//ReadLine("Cell Number", contact.MobilePhone);
	//ReadLine("Pager Number", contact.Pager);
	//ReadLine("Empresa", contact.Company);
	//ReadLine("Address Line 1", contact.WorkAddress.Address1);
	//ReadLine("Address Line 2", contact.WorkAddress.Address2);
	//ReadLine("Address Line 3", contact.WorkAddress.Address3);
	//ReadLine("City", contact.WorkAddress.City);
	//ReadLine("Province / State", contact.WorkAddress.Province);
	//ReadLine("Country", contact.WorkAddress.Country);
	//ReadLine("Postal / Zip Code", contact.WorkAddress.PostalCode);
	//ReadLine("Notes", contact.Notes);	
	//ReadDate("Birthday (YYYYMMDD format)", contact.Birthday);
	//ReadDate("Anniversary (YYYYMMDD format)", contact.Anniversary);
	//ReadLine("URL", contact.URL);
	//ReadLine("PIN", contact.PIN);
	//ReadLine("Prefix", contact.Prefix);
	//ReadLine("Notes", contact.Notes);
	//ReadLine("UserDefined1", contact.UserDefined1);
	//ReadLine("UserDefined2", contact.UserDefined2);
	//ReadLine("UserDefined3", contact.UserDefined3);
	//ReadLine("UserDefined4", contact.UserDefined4);
	
	
	//****************************************************************
	// modificacion hecha por israel marrero
	
	//Personal
	ReadLine("FirstName"      , contact.FirstName);
	ReadLine("LastName"       , contact.LastName);
	ReadLine("MobilePhone"    , contact.MobilePhone);
	ReadLine("HomePhone"      , contact.HomePhone);
	ReadLine("HomePhone2"     , contact.HomePhone2);
	ReadLine("PIN"            , contact.PIN);
	//--
	  ReadLine("HomeAddress1"   , contact.HomeAddress.Address1);
	  ReadLine("HomeAddress2"   , contact.HomeAddress.Address2);
	  ReadLine("HomeCity"       , contact.HomeAddress.City);
	  ReadLine("HomeProvince"   , contact.HomeAddress.Province);
	  ReadLine("HomePostalCode" , contact.HomeAddress.PostalCode);
	  ReadLine("HomeCountry"    , contact.HomeAddress.Country);
	  
	  
	

	//Trabajo
	ReadLine("Company"        , contact.Company);
	ReadLine("JobTitle"       , contact.JobTitle);
	ReadLine("WorkPhone"      , contact.WorkPhone);
	ReadLine("WorkPhone2"     , contact.WorkPhone2);
	ReadLine("Fax"            , contact.Fax);
	ReadLine("Pager"          , contact.Pager);
	//-
	  ReadLine("WorkAddress1"   , contact.WorkAddress.Address1);
	  ReadLine("WorkAddress2"   , contact.WorkAddress.Address2);
	  ReadLine("WorkCity"       , contact.WorkAddress.City);
	  ReadLine("WorkProvince"   , contact.WorkAddress.Province);
	  ReadLine("WorkPostalCode" , contact.WorkAddress.PostalCode);
	  ReadLine("WorkCountry"    , contact.WorkAddress.Country);
	
	
	//Otros
	ReadLine("OtherPhone"     , contact.OtherPhone);
	ReadLine("URL"            , contact.URL);
	ReadLine("Prefix"         , contact.Prefix);
	ReadDate("Birthday (YYYYMMDD format)", contact.Birthday);
	ReadDate("Anniversary (YYYYMMDD format)", contact.Anniversary);
	ReadLine("UserDefined1"   , contact.UserDefined1);
	ReadLine("UserDefined2"   , contact.UserDefined2);
	ReadLine("UserDefined3"   , contact.UserDefined3);
	ReadLine("UserDefined4"   , contact.UserDefined4);
	//ReadLine("Birthday"       , contact.Birthday);
	//ReadLine("Anniversary"    , contact.Anniversary);
	
	

	//*****************************************************
	string categories;
	ReadLine("Categories", categories);
	contact.Categories.CategoryStr2List(categories);
	
	ReadLine("Notes"          , contact.Notes);	
	

	
	//Aqui se lee la foto
	string image_path;
	ReadLine("Photo path", image_path);
	if( image_path.size() ) {
		ifstream in(image_path.c_str());
		//char block[4096];
		char block[8];

		contact.Image.clear();
		while( in.read(block, sizeof(block)) ) {
			contact.Image.append(block, in.gcount());
		}
	}
	else {
		cout << "Can't open: " << image_path << endl;
	}
	
	//Aqui se lee el email
			string email;
	do {
		ReadLine("Email Address (blank to end)", email);
		if( email.size() )
			contact.EmailAddresses.push_back(email);
	} while( email.size() );

	
	

}

void Upload(const Linberry::ProbeResult &device, const Linberry::Contact &contact)
{
	// connect to address book
	Controller con(device);
	Mode::Desktop desktop(con);
	desktop.Open();
	unsigned int id = desktop.GetDBID("Address Book");

	// find out what records are already there, and make new record ID
	RecordStateTable table;
	desktop.GetRecordStateTable(id, table);
	uint32_t recordId = table.MakeNewRecordId();

	// add it
	desktop.AddRecordByType(recordId, contact);
	cout << "Added successfully." << endl;
}

int main(int argc, char *argv[])
{
	try {

		Linberry::Init();

		Linberry::Probe probe;
		if( probe.GetCount() == 0 ) {
			cout << "NohayBalckBerry" << endl;
			return 1;
		}
		else {
			cout << "LinberryAc , Blackberry PIN: " << std::hex
			     << probe.Get(0).m_pin << endl;
		}


		Linberry::Contact contact;
		ReadInput(contact);
		Upload(probe.Get(0), contact);

	}
	catch( std::exception &e ) {
		std::cerr << "Exception caught: " << e.what() << endl;
		return 1;
	}

	return 0;
}

