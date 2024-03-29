///
/// \file	ldif.cc
///		Routines for reading and writing LDAP LDIF data.
///

/*
    Copyright (C) 2005-2009, Net Direct Inc. (http://www.netdirect.ca/)

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

#include "ldif.h"
#include "record.h"
#include "r_contact.h"
#include "base64.h"
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <string.h>

#define __DEBUG_MODE__
#include "debug.h"

namespace Linberry {

const ContactLdif::NameToFunc ContactLdif::FieldMap[] = {
	{ "Email", "Email address",
		&ContactLdif::Email, &ContactLdif::SetEmail },
	{ "Phone", "Phone number",
		&ContactLdif::Phone, &ContactLdif::SetPhone },
	{ "Fax", "Fax number",
		&ContactLdif::Fax, &ContactLdif::SetFax },
	{ "WorkPhone", "Work phone number",
		&ContactLdif::WorkPhone, &ContactLdif::SetWorkPhone },
	{ "HomePhone", "Home phone number",
		&ContactLdif::HomePhone, &ContactLdif::SetHomePhone },
	{ "MobilePhone", "Mobile phone number",
		&ContactLdif::MobilePhone, &ContactLdif::SetMobilePhone },
	{ "Pager", "Pager number",
		&ContactLdif::Pager, &ContactLdif::SetPager },
	{ "PIN", "PIN",
		&ContactLdif::PIN, &ContactLdif::SetPIN },
	{ "FirstName", "First name",
		&ContactLdif::FirstName, &ContactLdif::SetFirstName },
	{ "LastName", "Last name",
		&ContactLdif::LastName, &ContactLdif::SetLastName },
	{ "Company", "Company name",
		&ContactLdif::Company, &ContactLdif::SetCompany },
	{ "DefaultCommunicationsMethod", "Default communications method",
		&ContactLdif::DefaultCommunicationsMethod, &ContactLdif::SetDefaultCommunicationsMethod },
	{ "WorkAddress1", "Work Address, line 1",
		&ContactLdif::WorkAddress1, &ContactLdif::SetWorkAddress1 },
	{ "WorkAddress2", "Work Address, line 2",
		&ContactLdif::WorkAddress2, &ContactLdif::SetWorkAddress2 },
	{ "WorkAddress3", "Work Address, line 3",
		&ContactLdif::WorkAddress3, &ContactLdif::SetWorkAddress3 },
	{ "WorkCity", "WorkCity",
		&ContactLdif::WorkCity, &ContactLdif::SetWorkCity },
	{ "WorkProvince", "WorkProvince / State",
		&ContactLdif::WorkProvince, &ContactLdif::SetWorkProvince },
	{ "WorkPostalCode", "Work Postal / ZIP code",
		&ContactLdif::WorkPostalCode, &ContactLdif::SetWorkPostalCode },
	{ "WorkCountry", "WorkCountry",
		&ContactLdif::WorkCountry, &ContactLdif::SetWorkCountry },
	{ "JobTitle", "Job Title",
		&ContactLdif::JobTitle, &ContactLdif::SetJobTitle },
	{ "PublicKey", "Public key",
		&ContactLdif::PublicKey, &ContactLdif::SetPublicKey },
	{ "Notes", "Notes",
		&ContactLdif::Notes, &ContactLdif::SetNotes },
	{ "WorkPostalAddress", "Mailing Work address (includes address lines, city, province, country, and postal code)",
		&ContactLdif::WorkPostalAddress, &ContactLdif::SetWorkPostalAddress },
	{ "HomePostalAddress", "Mailing home address (includes address lines, city, province, country, and postal code)",
		&ContactLdif::HomePostalAddress, &ContactLdif::SetHomePostalAddress },
	{ "FullName", "First + Last names",
		&ContactLdif::FullName, &ContactLdif::SetFullName },
	{ "FQDN", "Fully qualified domain name",
		&ContactLdif::FQDN, &ContactLdif::SetFQDN },
	{ 0, 0, 0 }
};


bool ContactLdif::LdifAttribute::operator<(const LdifAttribute &other) const
{
	// the dn attribute always comes first in LDIF output
	if( name == "dn" ) {
		if( other.name == "dn" )
			return false;	// both dn, so equal
		return true;
	}
	else if( other.name == "dn" )
		return false;

	return (order < other.order && name != other.name) ||
		(order == other.order && name < other.name);
}

bool ContactLdif::LdifAttribute::operator==(const LdifAttribute &other) const
{
	return name == other.name;
}


///////////////////////////////////////////////////////////////////////////////
// ContactLdif class

ContactLdif::ContactLdif(const std::string &baseDN)
	: m_baseDN(baseDN)
{
	// setup some sane defaults
	Map("mail", &ContactLdif::Email, &ContactLdif::SetEmail);
	Map("facsimileTelephoneNumber", &ContactLdif::Fax, &ContactLdif::SetFax);
	Map("telephoneNumber", &ContactLdif::WorkPhone, &ContactLdif::SetWorkPhone);
	Map("homePhone", &ContactLdif::HomePhone, &ContactLdif::SetHomePhone);
	Map("mobile", &ContactLdif::MobilePhone, &ContactLdif::SetMobilePhone);
	Map("pager", &ContactLdif::Pager, &ContactLdif::SetPager);
	Map("l", &ContactLdif::WorkCity, &ContactLdif::SetWorkCity);
	Map("st", &ContactLdif::WorkProvince, &ContactLdif::SetWorkProvince);
	Map("postalCode", &ContactLdif::WorkPostalCode, &ContactLdif::SetWorkPostalCode);
	Map("o", &ContactLdif::Company, &ContactLdif::SetCompany);
	Map("c", &ContactLdif::WorkCountry, &ContactLdif::SetWorkCountry);
	SetObjectClass("c", "country");

	Map("title", &ContactLdif::JobTitle, &ContactLdif::SetJobTitle);
	Map("dn", &ContactLdif::FQDN, &ContactLdif::SetFQDN);
	Map("displayName", &ContactLdif::FullName, &ContactLdif::SetFullName);
	Map("cn", &ContactLdif::FullName, &ContactLdif::SetFullName);
	Map("sn", &ContactLdif::LastName, &ContactLdif::SetLastName);
	Map("givenName", &ContactLdif::FirstName, &ContactLdif::SetFirstName);
	Map("street", &ContactLdif::WorkAddress1, &ContactLdif::SetWorkAddress1);
	Map("postalAddress", &ContactLdif::WorkPostalAddress, &ContactLdif::SetWorkPostalAddress);
	Map("homePostalAddress", &ContactLdif::HomePostalAddress, &ContactLdif::SetHomePostalAddress);
	Map("note", &ContactLdif::Notes, &ContactLdif::SetNotes);

	// add heuristics hooks
	Hook("cn", &m_cn);
	Hook("displayName", &m_displayName);
	Hook("sn", &m_sn);
	Hook("givenName", &m_givenName);

	// set default DN attribute
	SetDNAttr("cn");
}

ContactLdif::~ContactLdif()
{
}

void ContactLdif::DoWrite(Linberry::Contact &con,
			  const std::string &attr,
			  const std::string &data)
{
	// valid?
	if( attr.size() == 0 || data.size() == 0 )
		return;

	// now have attr/data pair, check hooks:
	HookMapType::iterator hook = m_hookMap.find(attr);
	if( hook != m_hookMap.end() ) {
		*(hook->second) = data;
	}

	// run according to map
	AccessMapType::iterator acc = m_map.find(attr);
	if( acc != m_map.end() ) {
		(this->*(acc->second.write))(con, data);
	}
}

void ContactLdif::Hook(const std::string &ldifname, std::string *var)
{
	m_hookMap[ldifname] = var;
}

const ContactLdif::NameToFunc*
ContactLdif::GetField(const std::string &fieldname) const
{
	for( const NameToFunc *n = FieldMap; n->name; n++ ) {
		if( fieldname == n->name )
			return n;
	}
	return 0;
}

std::string ContactLdif::GetFieldReadName(GetFunctionType read) const
{
	for( const NameToFunc *n = FieldMap; n->name; n++ ) {
		if( read == n->read )
			return n->name;
	}
	return "<unknown>";
}

std::string ContactLdif::GetFieldWriteName(SetFunctionType write) const
{
	for( const NameToFunc *n = FieldMap; n->name; n++ ) {
		if( write == n->write )
			return n->name;
	}
	return "<unknown>";
}

bool ContactLdif::Map(const LdifAttribute &ldifname,
		      const std::string &readField,
		      const std::string &writeField)
{
	const NameToFunc *read = GetField(readField);
	const NameToFunc *write = GetField(writeField);
	if( !read || !write )
		return false;
	Map(ldifname, read->read, write->write);
	return true;
}

void ContactLdif::Map(const LdifAttribute &ldifname,
		      GetFunctionType read,
		      SetFunctionType write)
{
	m_map[ldifname] = AccessPair(read, write);
}

void ContactLdif::Unmap(const LdifAttribute &ldifname)
{
	m_map.erase(ldifname);
}

//
// SetDNAttr
//
/// Sets the LDIF attribute name to use when constructing the FQDN.
/// The FQDN field will take this name, and combine it with the
/// baseDN from the constructor to produce a FQDN for the record.
///
bool ContactLdif::SetDNAttr(const LdifAttribute &name)
{
	// try to find the attribute in the map
	AccessMapType::iterator i = m_map.find(name);
	if( i == m_map.end() )
		return false;

	m_dnAttr = name;
	return true;
}

bool ContactLdif::SetObjectClass(const LdifAttribute &name,
				 const std::string &objectClass)
{
	AccessMapType::iterator i = m_map.find(name);
	if( i == m_map.end() )
		return false;

	LdifAttribute key = i->first;
	AccessPair pair = i->second;
	m_map.erase(key);
	key.objectClass = objectClass;
	m_map[key] = pair;
	return true;
}

bool ContactLdif::SetObjectOrder(const LdifAttribute &name, int order)
{
	AccessMapType::iterator i = m_map.find(name);
	if( i == m_map.end() )
		return false;

	LdifAttribute key = i->first;
	AccessPair pair = i->second;
	m_map.erase(key);
	key.order = order;
	m_map[key] = pair;
	return true;
}


std::string ContactLdif::Email(const Linberry::Contact &con) const
{
	return con.GetEmail(m_emailIndex++);
}

std::string ContactLdif::Phone(const Linberry::Contact &con) const
{
	return con.Phone;
}

std::string ContactLdif::Fax(const Linberry::Contact &con) const
{
	return con.Fax;
}

std::string ContactLdif::WorkPhone(const Linberry::Contact &con) const
{
	return con.WorkPhone;
}

std::string ContactLdif::HomePhone(const Linberry::Contact &con) const
{
	return con.HomePhone;
}

std::string ContactLdif::MobilePhone(const Linberry::Contact &con) const
{
	return con.MobilePhone;
}

std::string ContactLdif::Pager(const Linberry::Contact &con) const
{
	return con.Pager;
}

std::string ContactLdif::PIN(const Linberry::Contact &con) const
{
	return con.PIN;
}

std::string ContactLdif::FirstName(const Linberry::Contact &con) const
{
	return con.FirstName;
}

std::string ContactLdif::LastName(const Linberry::Contact &con) const
{
	return con.LastName;
}

std::string ContactLdif::Company(const Linberry::Contact &con) const
{
	return con.Company;
}

std::string ContactLdif::DefaultCommunicationsMethod(const Linberry::Contact &con) const
{
	return con.DefaultCommunicationsMethod;
}

std::string ContactLdif::WorkAddress1(const Linberry::Contact &con) const
{
	return con.WorkAddress.Address1;
}

std::string ContactLdif::WorkAddress2(const Linberry::Contact &con) const
{
	return con.WorkAddress.Address2;
}

std::string ContactLdif::WorkAddress3(const Linberry::Contact &con) const
{
	return con.WorkAddress.Address3;
}

std::string ContactLdif::WorkCity(const Linberry::Contact &con) const
{
	return con.WorkAddress.City;
}

std::string ContactLdif::WorkProvince(const Linberry::Contact &con) const
{
	return con.WorkAddress.Province;
}

std::string ContactLdif::WorkPostalCode(const Linberry::Contact &con) const
{
	return con.WorkAddress.PostalCode;
}

std::string ContactLdif::WorkCountry(const Linberry::Contact &con) const
{
	return con.WorkAddress.Country;
}

std::string ContactLdif::JobTitle(const Linberry::Contact &con) const
{
	return con.JobTitle;
}

std::string ContactLdif::PublicKey(const Linberry::Contact &con) const
{
	return con.PublicKey;
}

std::string ContactLdif::Notes(const Linberry::Contact &con) const
{
	return con.Notes;
}

std::string ContactLdif::WorkPostalAddress(const Linberry::Contact &con) const
{
	return con.WorkAddress.GetLabel();
}

std::string ContactLdif::HomePostalAddress(const Linberry::Contact &con) const
{
	return con.HomeAddress.GetLabel();
}

std::string ContactLdif::FullName(const Linberry::Contact &con) const
{
	return con.GetFullName();
}

std::string ContactLdif::FQDN(const Linberry::Contact &con) const
{
	std::string FQDN = m_dnAttr.name;
	FQDN += "=";

	AccessMapType::const_iterator i = m_map.find(m_dnAttr);
	if( i != m_map.end() ) {
		FQDN += (this->*(i->second.read))(con);
	}
	else {
		FQDN += "unknown";
	}

	FQDN += ",";
	FQDN += m_baseDN;
	return FQDN;
}

bool ContactLdif::IsArrayFunc(GetFunctionType getf) const
{
	// Currently, only the Email getter has array data
	if( getf == &ContactLdif::Email )
		return true;
	return false;
}

void ContactLdif::ClearArrayState() const
{
	m_emailIndex = 0;
}

void ContactLdif::SetEmail(Linberry::Contact &con, const std::string &val) const
{
	con.EmailAddresses.push_back(val);
}

void ContactLdif::SetPhone(Linberry::Contact &con, const std::string &val) const
{
	con.Phone = val;
}

void ContactLdif::SetFax(Linberry::Contact &con, const std::string &val) const
{
	con.Fax = val;
}

void ContactLdif::SetWorkPhone(Linberry::Contact &con, const std::string &val) const
{
	con.WorkPhone = val;
}

void ContactLdif::SetHomePhone(Linberry::Contact &con, const std::string &val) const
{
	con.HomePhone = val;
}

void ContactLdif::SetMobilePhone(Linberry::Contact &con, const std::string &val) const
{
	con.MobilePhone = val;
}

void ContactLdif::SetPager(Linberry::Contact &con, const std::string &val) const
{
	con.Pager = val;
}

void ContactLdif::SetPIN(Linberry::Contact &con, const std::string &val) const
{
	con.PIN = val;
}

void ContactLdif::SetFirstName(Linberry::Contact &con, const std::string &val) const
{
	con.FirstName = val;
}

void ContactLdif::SetLastName(Linberry::Contact &con, const std::string &val) const
{
	con.LastName = val;
}

void ContactLdif::SetCompany(Linberry::Contact &con, const std::string &val) const
{
	con.Company = val;
}

void ContactLdif::SetDefaultCommunicationsMethod(Linberry::Contact &con, const std::string &val) const
{
	con.DefaultCommunicationsMethod = val;
}

void ContactLdif::SetWorkAddress1(Linberry::Contact &con, const std::string &val) const
{
	con.WorkAddress.Address1 = val;
}

void ContactLdif::SetWorkAddress2(Linberry::Contact &con, const std::string &val) const
{
	con.WorkAddress.Address2 = val;
}

void ContactLdif::SetWorkAddress3(Linberry::Contact &con, const std::string &val) const
{
	con.WorkAddress.Address3 = val;
}

void ContactLdif::SetWorkCity(Linberry::Contact &con, const std::string &val) const
{
	con.WorkAddress.City = val;
}

void ContactLdif::SetWorkProvince(Linberry::Contact &con, const std::string &val) const
{
	con.WorkAddress.Province = val;
}

void ContactLdif::SetWorkPostalCode(Linberry::Contact &con, const std::string &val) const
{
	con.WorkAddress.PostalCode = val;
}

void ContactLdif::SetWorkCountry(Linberry::Contact &con, const std::string &val) const
{
	con.WorkAddress.Country = val;
}

void ContactLdif::SetJobTitle(Linberry::Contact &con, const std::string &val) const
{
	con.JobTitle = val;
}

void ContactLdif::SetPublicKey(Linberry::Contact &con, const std::string &val) const
{
	con.PublicKey = val;
}

void ContactLdif::SetNotes(Linberry::Contact &con, const std::string &val) const
{
	con.Notes = val;
}

void ContactLdif::SetWorkPostalAddress(Linberry::Contact &con, const std::string &val) const
{
	// FIXME;
//	throw std::runtime_error("SetWorkPostalAddress() not implemented");
//	std::cout << "SetWorkPostalAddress() not implemented: " << val << std::endl;
}

void ContactLdif::SetHomePostalAddress(Linberry::Contact &con, const std::string &val) const
{
	// FIXME;
//	throw std::runtime_error("SetHomePostalAddress() not implemented");
//	std::cout << "SetHomePostalAddress() not implemented: " << val << std::endl;
}

void ContactLdif::SetFullName(Linberry::Contact &con, const std::string &val) const
{
	std::string first, last;
	Contact::SplitName(val, first, last);
	con.FirstName = first;
	con.LastName = last;
}

void ContactLdif::SetFQDN(Linberry::Contact &con, const std::string &val) const
{
	throw std::runtime_error("not implemented");
}


void ContactLdif::ClearHeuristics()
{
	m_cn.clear();
	m_displayName.clear();
	m_sn.clear();
	m_givenName.clear();
}

bool ContactLdif::RunHeuristics(Linberry::Contact &con)
{
	// start fresh
	con.LastName.clear();
	con.FirstName.clear();

	// find the best match for name... prefer sn/givenName if available
	if( m_sn.size() ) {
		con.LastName = m_sn;
	}
	if( m_givenName.size() ) {
		con.FirstName = m_givenName;
	}

	if( !con.LastName.size() || !con.FirstName.size() ) {
		std::string first, last;

		// still don't have a complete name, check cn first
		if( m_cn.size() ) {
			Contact::SplitName(m_cn, first, last);
			if( !con.LastName.size() && last.size() )
				con.LastName = last;
			if( !con.FirstName.size() && first.size() )
				con.FirstName = first;
		}

		// displayName is last chance
		if( m_displayName.size() ) {
			Contact::SplitName(m_displayName, first, last);
			if( !con.LastName.size() && last.size() )
				con.LastName = last;
			if( !con.FirstName.size() && first.size() )
				con.FirstName = first;
		}
	}

	return con.LastName.size() && con.FirstName.size();
}


//
// DumpLdif
//
/// Output contact data to os in LDAP LDIF format.
///
void ContactLdif::DumpLdif(std::ostream &os,
		       const Linberry::Contact &con) const
{
	// start fresh
	ClearArrayState();

	// setup stream state
	std::ios::fmtflags oldflags = os.setf(std::ios::left);
	char fill = os.fill(' ');

	if( FirstName(con).size() == 0 && LastName(con).size() == 0 )
		return;			// nothing to do

	os << "# Contact 0x" << std::hex << con.GetID() << ", "
		<< FullName(con) << "\n";

	// cycle through the map
	for(	AccessMapType::const_iterator b = m_map.begin();
		b != m_map.end();
		++b )
	{
		// print only fields with data
		std::string field;

		do {
			field = (this->*(b->second.read))(con);
			if( field.size() ) {
				os << b->first.name << MakeLdifData(field) << "\n";
				if( b->first.objectClass.size() )
					os << "objectClass: " << b->first.objectClass << "\n";
			}
		} while( IsArrayFunc(b->second.read) && field.size() );
	}

	os << "objectClass: inetOrgPerson\n";

	// last line must be empty
	os << "\n";

	// cleanup the stream
	os.flags(oldflags);
	os.fill(fill);
}

bool ContactLdif::ReadLdif(std::istream &is, Linberry::Contact &con)
{
	std::string line;

	// start fresh
	con.Clear();
	ClearHeuristics();

	// search for beginning dn: line
	bool found = false;
	while( std::getline(is, line) ) {
		if( strncmp(line.c_str(), "dn: ", 4) == 0 ) {
			found = true;
			break;
		}
	}
	if( !found )
		return false;

	// storage for various name styles
	std::string coded, decode, attr, data;
	bool b64field = false;

	// read ldif lines until empty line is found
	while( getline(is, line) && line.size() ) {

		if( b64field ) {
			// processing a base64 encoded field
			if( line[0] == ' ' ) {
				coded += "\n";
				coded += line;
				continue;
			}
			else {
				// end of base64 block... ignore errors,
				// and attempt to save everything decodable...
				// the LDAP server sometimes returns incomplete
				// base64 encoding, but otherwise the data is fine
				base64_decode(coded, decode);
				DoWrite(con, attr, decode);
				coded.clear();
				b64field = false;
			}
			// fall through to process new line
		}


		// split into attribute / data
		std::string::size_type delim = line.find(':'), dstart;
		if( delim == std::string::npos )
			continue;

		attr.assign(line, 0, delim);
		dstart = delim + 1;
		while( line[dstart] == ' ' || line[dstart] == ':' )
			dstart++;
		data = line.substr(dstart);

		// is this data base64 encoded?
		if( line[delim + 1] == ':' ) {
			coded = data;
			b64field = true;
			continue;
		}

		DoWrite(con, attr, data);
	}

	if( b64field ) {
		// clean up base64 decoding... ignore errors, see above comment
		base64_decode(coded, decode);
		DoWrite(con, attr, decode);
		coded.clear();
		b64field = false;
	}

	return RunHeuristics(con);
}

void ContactLdif::DumpMap(std::ostream &os) const
{
	std::ios::fmtflags oldflags = os.setf(std::ios::left);
	char fill = os.fill(' ');

	os << "ContactLdif Mapping:\n";

	// cycle through the map
	for(	AccessMapType::const_iterator b = m_map.begin();
		b != m_map.end();
		++b )
	{
		os << "   " << std::left << std::setw(20) << b->first.name
		   << "->  " << GetFieldReadName(b->second.read)
		   << " / " << GetFieldWriteName(b->second.write) << "\n";

		// find read/write names

		if( b->first.objectClass.size() ) {
			os << "   " << std::setw(20) << " "
			   << "objectClass: " << b->first.objectClass << "\n";
		}
	}

	os << "   >>> DN attribute: " << m_dnAttr.name << "\n";

	// cleanup the stream
	os.flags(oldflags);
	os.fill(fill);
}

std::string ContactLdif::MakeLdifData(const std::string &str)
{
	std::string data = ":";

	if( NeedsEncoding(str) ) {
		std::string b64;
		base64_encode(str, b64);

		data += ": ";
		data += b64;
	}
	else {
		data += " ";
		data += str;
	}

	return data;
}

//
// RFC 2849
//
// Must not contain:
//	0x00 (NUL), 0x0a (LF), 0x0d (CR), or anything greater than 0x7f
//
// First char must meet above criteria, plus must not be:
//	0x20 (SPACE), 0x3a (colon), 0x3c ('<')
//
bool ContactLdif::NeedsEncoding(const std::string &str)
{
	for( std::string::size_type i = 0; i < str.size(); i++ ) {
		unsigned char c = str[i];

		switch( c )
		{
		case 0x00:
		case 0x0a:
		case 0x0d:
			return true;

		case 0x20:
		case 0x3a:
		case 0x3c:
			if( i == 0 )
				return true;
		}

		if( c > 0x7f )
			return true;
	}
	return false;
}

} // namespace Linberry

