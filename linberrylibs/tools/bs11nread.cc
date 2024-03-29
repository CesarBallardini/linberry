///
/// \file	bs11nread.cc
///		Reads an boost serialization file and dumps to stdout.
///

/*
    Copyright (C) 2008-2009, Net Direct Inc. (http://www.netdirect.ca/)

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

#define __LINBERRY_BOOST_MODE__	// this program always requires BOOST
#include <linberry/linberry.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <getopt.h>


using namespace std;
using namespace Linberry;

void Usage()
{
   int major, minor;
   const char *Version = Linberry::Version(major, minor);

   cerr
   << "bs11nread - Reads a boost serialization file (from btool)\n"
   << "            and dumps data to stdout\n"
   << "        Copyright 2008-2009, Net Direct Inc. (http://www.netdirect.ca/)\n"
   << "        Using: " << Version << "\n"
   << "\n"
   << "   -f file   Filename to save or load handheld data to/from\n"
   << "   -h        This help\n"
   << "   -S        Show list of supported database parsers\n"
   << endl;
}

template <class Record>
bool Dump(const std::string &dbName, ifstream &ifs)
{
	if( dbName != Record::GetDBName() )
		return false;

	std::vector<Record> records;
	boost::archive::text_iarchive ia(ifs);
	ia >> records;
	cout << records.size()
	     << " records loaded" << endl;
	sort(records.begin(), records.end());

	typename std::vector<Record>::const_iterator
		beg = records.begin(), end = records.end();
	for( ; beg != end; beg++ ) {
		cout << (*beg) << endl;
	}

	return true;
}

void DumpDB(const string &filename)
{
	// filename is available, attempt to load
	ifstream ifs(filename.c_str());
	std::string dbName;
	getline(ifs, dbName);

	// check for recognized database names
	Dump<Contact>		(dbName, ifs) ||
	Dump<Message>		(dbName, ifs) ||
	Dump<Calendar>		(dbName, ifs) ||
	Dump<ServiceBook>	(dbName, ifs) ||
	Dump<Memo>		(dbName, ifs) ||
	Dump<Task>		(dbName, ifs) ||
	Dump<PINMessage>	(dbName, ifs) ||
	Dump<SavedMessage>	(dbName, ifs) ||
	Dump<Folder>		(dbName, ifs) ||
	Dump<Timezone>		(dbName, ifs) ||
		cerr << "Unknown database name: " << dbName << endl;
}

void ShowParsers()
{
	cout << "Supported Database parsers:\n"
	<< "   Address Book\n"
	<< "   Messages\n"
	<< "   Calendar\n"
	<< "   Service Book\n"
	<< "   Memos\n"
	<< "   Tasks\n"
	<< "   PIN Messages\n"
	<< "   Saved Email Messages\n"
	<< "   Folders\n"
	<< "   Time Zones\n"
	<< endl;
}

int main(int argc, char *argv[])
{
	try {
		string filename;

		// process command line options
		for(;;) {
			int cmd = getopt(argc, argv, "f:hS");
			if( cmd == -1 )
				break;

			switch( cmd )
			{
			case 'f':	// filename
				filename = optarg;
				break;

			case 'S':	// show supported databases
				ShowParsers();
				return 0;

			case 'h':	// help
			default:
				Usage();
				return 0;
			}
		}

		// Initialize the linberry library.  Must be called before
		// anything else.
		Linberry::Init();

		if( !filename.size() ) {
			cerr << "Filename must be specified" << endl;
			return 1;
		}

		DumpDB(filename);

	}
	catch( boost::archive::archive_exception &ae ) {
		cerr << "Archive exception: "
		     << ae.what() << endl;
		return 1;
	}
	catch( Usb::Error &ue) {
		std::cerr << "Usb::Error caught: " << ue.what() << endl;
		return 1;
	}
	catch( Linberry::Error &se ) {
		std::cerr << "Linberry::Error caught: " << se.what() << endl;
		return 1;
	}
	catch( std::exception &e ) {
		std::cerr << "std::exception caught: " << e.what() << endl;
		return 1;
	}

	return 0;
}

