///
/// \file	brecsum.cc
///		Generate SHA1 sums of raw Blackberry database records.
///		This is mostly useful for data verification during testing.
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

#include <linberry/linberry.h>
#include <iomanip>
#include <iostream>
#include <vector>
#include <string>
#include <getopt.h>

using namespace std;
using namespace Linberry;

void Usage()
{
   int major, minor;
   const char *Version = Linberry::Version(major, minor);

   cerr
   << "brecsum - Generate SHA1 sums of raw Blackberry database records.\n"
   << "        Copyright 2008-2009, Net Direct Inc. (http://www.netdirect.ca/)\n"
   << "        Using: " << Version << "\n"
   << "\n"
   << "   -d db     Read database 'db' and sum all its records.\n"
   << "             Can be used multiple times to fetch more than one DB\n"
   << "   -h        This help\n"
   << "   -i        Include Type and Unique record IDs in the checksums\n"
   << "   -p pin    PIN of device to talk with\n"
   << "             If only one device is plugged in, this flag is optional\n"
   << "   -P pass   Simplistic method to specify device password\n"
   << "   -v        Dump protocol data during operation\n"
   << endl;
}

class ChecksumParser : public Linberry::Parser
{
	bool m_IncludeIds;
	SHA_CTX m_ctx;

public:
	explicit ChecksumParser(bool IncludeIds)
		: m_IncludeIds(IncludeIds)
	{}

	virtual void Clear()
	{
		SHA1_Init(&m_ctx);
	}

	virtual void SetIds(uint8_t RecType, uint32_t UniqueId)
	{
		if( m_IncludeIds ) {
			SHA1_Update(&m_ctx, &RecType, sizeof(RecType));
			SHA1_Update(&m_ctx, &UniqueId, sizeof(UniqueId));
		}
	}

	virtual void ParseHeader(const Linberry::Data &, size_t &)
	{
		// do nothing here, parse it all at once in ParseFields
	}

	virtual void ParseFields(const Linberry::Data &data, size_t &offset,
				const Linberry::IConverter *ic)
	{
		int len = data.GetSize() - offset;
		SHA1_Update(&m_ctx, data.GetData() + offset, len);
		offset += len;
	}

	virtual void Store()
	{
		unsigned char sha1[SHA_DIGEST_LENGTH];
		SHA1_Final(sha1, &m_ctx);

		for( int i = 0; i < SHA_DIGEST_LENGTH; i++ ) {
			cout << hex << setfill('0') << setw(2)
				<< (unsigned int) sha1[i];
		}
		cout << endl;
	}
};

int main(int argc, char *argv[])
{
	cout.sync_with_stdio(true);	// leave this on, since libusb uses
					// stdio for debug messages

	try {

		uint32_t pin = 0;
		bool
			data_dump = false,
			include_ids = false;
		string password;
		vector<string> dbNames;

		// process command line options
		for(;;) {
			int cmd = getopt(argc, argv, "d:hip:P:v");
			if( cmd == -1 )
				break;

			switch( cmd )
			{
			case 'd':	// show dbname
				dbNames.push_back(string(optarg));
				break;

			case 'i':	// Include IDs
				include_ids = true;
				break;

			case 'p':	// Blackberry PIN
				pin = strtoul(optarg, NULL, 16);
				break;

			case 'P':	// Device password
				password = optarg;
				break;

			case 'v':	// data dump on
				data_dump = true;
				break;

			case 'h':	// help
			default:
				Usage();
				return 0;
			}
		}

		// Display usage info if user appears confused
		if( !dbNames.size() ) {
			Usage();
			return 0;
		}

		// Initialize the linberry library.  Must be called before
		// anything else.
		Linberry::Init(data_dump);

		// Probe the USB bus for Blackberry devices and display.
		Linberry::Probe probe;
		int activeDevice = probe.FindActive(pin);
		if( activeDevice == -1 ) {
			cerr << "No device selected, or PIN not found" << endl;
			return 1;
		}

		// Create our controller object
		Linberry::Controller con(probe.Get(activeDevice));
		Linberry::Mode::Desktop desktop(con);

		// Sum all specified databases
		if( dbNames.size() ) {
			vector<string>::iterator b = dbNames.begin();
			ChecksumParser parser(include_ids);

			desktop.Open(password.c_str());
			for( ; b != dbNames.end(); b++ ) {
				unsigned int id = desktop.GetDBID(*b);
				desktop.LoadDatabase(id, parser);
			}
		}

	}
	catch( std::exception &e ) {
		std::cerr << e.what() << endl;
		return 1;
	}

	return 0;
}

