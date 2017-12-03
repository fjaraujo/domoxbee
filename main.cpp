//-----------------------------------------------------------------------------
//
// domoxbee a C++/C-wrapper to add XBee (ZigBee) support to DomotiGa.
//
// DomotiGa - an open source home automation program.
// Copyright (C) Ron Klinkien, The Netherlands.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General PUBLIC License for more details.
//
// You should have received a copy of the GNU General PUBLIC License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------


#include <cstdlib>
#include <iostream>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <list>
#include <iomanip>

#include "atcon.h"
#include "datacon.h"
#include "remotenode.h"
#include "domoxbee.h"


int main(int argc, char *argv[]) {
	int i;

	try {

		std::cout << "Test for project utils" << std::endl;
		std::cout << DomoXBee_Version() << std::endl;

		/* get available libxbee modes */
		try {
			std::list<std::string> modes = libxbee::getModes();
			std::list<std::string>::iterator i;

			std::cout << "Available libxbee modes:\n";
			for (i = modes.begin(); i != modes.end(); i++) {
				std::cout << "  " << *i;
			}
			std::cout << "\n";
		} catch (xbee_err ret) {
			std::cout << "Error while retrieving libxbee modes...\n";
		}

		// Open server socket


		 DomoXBee_Init("/dev/ttyUSB0",57600,10,2501);

		 std::cout << "INIT OK" << std::endl;

		/* get available connection types */
/*
		libxbee::XBee xbee("xbeeZB", "/dev/ttyUSB0", 57600);
		std::cout << "Running libxbee in mode '" << xbee.mode() << std::endl;

		try {
			std::list<std::string> types = xbee.getConTypes();
			std::list<std::string>::iterator i;

			std::cout << "Available connection types:\n";
			for (i = types.begin(); i != types.end(); i++) {
				std::cout << "  " << *i << std::endl;
			}
			std::cout << "\n";
		} catch (xbee_err ret) {
			std::cout << "Error while retrieving connection types...\n";
		}
*/

	//	usleep(5000000); // wait 5 seconds

		DomoXBee_Discover(15);


		i=0;
			while (i< 15) {
				usleep(1000000); // wait 1 second
				i++;
			}
		//	return 0;

//		std::cout << "addr: " << DomoXBee_GetNodeAddress(0) << std::endl;

		/*
		std::cout << "Descubierto nodo" << DomoXBee_GetNodeName(0) << std::endl;
*/


     	DomoXBee_SendCmd("GARAJE_ACS","[CONNECT]\n");
//     	DomoXBee_SendCmd("GARAJE_ACS","[CONFIG]\n");

		while(1);

//		DomoXBee_SendCmd("NODO2","[CONNECT]\n");

		DomoXBee_SetIO("NODO1",3,0);

		usleep(500000);

		DomoXBee_SendCmd("NODO2","[RETRIEVE]\n");


		DomoXBee_SetIO("NODO1",3,1);

/*
		sleep(1);

		DomoXBee_SendCmd("NODO1","[LAMP1 OFF]\n");
*/

     	DomoXBee_Destroy();


	} catch (xbee_err err) {
		std::cout << "Error " << err << "\n";
	}

	return 0;
}


