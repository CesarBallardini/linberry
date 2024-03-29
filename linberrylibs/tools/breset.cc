///
/// \file	breset.cc
///		Attempt to reset all connected Blackberry devices via software
///
///		This file is part of the Linberry project:
///
///		http://www.netdirect.ca/software/packages/linberry
///		http://sourceforge.net/projects/linberry
///
///		Compile with the following command (needs libusb):
///
///		g++ -o breset breset.cc -lusb
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

#include <usb.h>
#include <stdio.h>
#include <unistd.h>

#define VENDOR_RIM		0x0fca
#define PRODUCT_RIM_BLACKBERRY	0x0001
#define PRODUCT_RIM_PEARL_DUAL	0x0004
#define PRODUCT_RIM_PEARL_8120	0x8004
#define PRODUCT_RIM_PEARL	0x0006

#define BLACKBERRY_INTERFACE		0
#define BLACKBERRY_CONFIGURATION	1

bool reset(struct usb_device *dev)
{
	usb_dev_handle *handle = usb_open(dev);
	if( !handle )
		return false;

	bool ret = usb_reset(handle) == 0;
	usb_close(handle);
	return ret;
}

int main()
{
	struct usb_bus *busses;

	usb_init();
	usb_find_busses();
	usb_find_devices();
	busses = usb_get_busses();

	printf("Verificando bus Dispositivo Blackberry...\n");
	int found = 0;

	struct usb_bus *bus;
	for( bus = busses; bus; bus = bus->next ) {
		struct usb_device *dev;

		for (dev = bus->devices; dev; dev = dev->next) {
			// Is this a blackberry?
			if( dev->descriptor.idVendor == VENDOR_RIM &&
			    (dev->descriptor.idProduct == PRODUCT_RIM_BLACKBERRY ||
			     dev->descriptor.idProduct == PRODUCT_RIM_PEARL ||
			     dev->descriptor.idProduct == PRODUCT_RIM_PEARL_8120 ||
			     dev->descriptor.idProduct == PRODUCT_RIM_PEARL_DUAL ) ) {
			    	printf("Positivo...");
				printf("Intentando resetear usb.\n");
				if( reset(dev) )
					found++;
				else
					printf("Imposible resetear el bus %s, devnum %u\n", bus->dirname, (unsigned int) dev->devnum);
			}
		}
	}

	printf("%d dispositivo%s usb configurado.\n", found, found > 1 ? "s" : "");
}

