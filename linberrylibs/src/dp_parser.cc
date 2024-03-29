/** 
 * @file dp_parser.cc
 * @author Nicolas VIVIEN
 * @date 2009-08-01
 *
 * @note CopyRight Nicolas VIVIEN
 *
 * @brief COD debug file parser
 *   RIM's JDE generates several files when you build a COD application.
 *   Indeed, with the COD files for the device, we have a ".debug" file.
 *   This file is usefull to debug an application from JVM.
 *   This tool is a parser to understand these ".debug" files.
 *   Obviously, the file contents only some strings and 32 bits words.
 *
 * @par Modifications
 *   - 2009/08/01 : N. VIVIEN
 *     - First release
 *
 * @par Licences
 *   Copyright (C) 2009-2010, Nicolas VIVIEN
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 *   See the GNU General Public License in the COPYING file at the
 *   root directory of this project for more details.
 */


#include <iostream>
#include <stdlib.h>
#include "dp_parser.h"
#include "endian.h"


using namespace std;


namespace Linberry {

namespace JDG {


string ParseString(istream &input, const int length) 
{
	int i;
	string str;

	for (i=0; i<length; i++) {
		uint16_t value;

		input.read((char *) &value, sizeof(uint16_t));

		str += (char) be_btohs(value);
	}

	return str;
}


uint32_t ParseInteger(istream &input) 
{
	uint32_t value;

	input.read((char *) &value, sizeof(uint32_t));

	return be_btohl(value);
}


} // namespace JDG

} // namespace Linberry


