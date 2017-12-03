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


#include "rematcon.h"
#include <iostream>


rematcon::~rematcon(void) {

	//std::cout << "Delete atcon START" << std::endl;

	//std::cout << "Delete atcon END" << std::endl;

}


void rematcon::xbee_conCallback(libxbee::Pkt **pkt) {
	char buffer[20];

	if ((*pkt)->getATCommand() == "DB") {
		if ((*pkt)->size() > 0) {
			rssi = (**pkt)[0];
		}
	}

	if ((*pkt)->getATCommand() == "IR") {
			if ((*pkt)->size() > 0) {
				ir = (**pkt)[0];
			}
	}

	if ((*pkt)->getATCommand() == "IC") {
				if ((*pkt)->size() > 0) {
					sprintf(buffer, "%02X%02X", (**pkt)[0],(**pkt)[1]);
					ic = std::stoul(buffer, nullptr, 16);
				}
	}

	if ((*pkt)->getATCommand() == "D0") {
		if ((*pkt)->size() > 0) {
			d0 = (**pkt)[0];
		}
	}
	if ((*pkt)->getATCommand() == "D1") {
		if ((*pkt)->size() > 0) {
			d1 = (**pkt)[0];
		}
	}
	if ((*pkt)->getATCommand() == "D2") {
		if ((*pkt)->size() > 0) {
			d2 = (**pkt)[0];
		}
	}
	if ((*pkt)->getATCommand() == "D3") {
		if ((*pkt)->size() > 0) {
			d3 = (**pkt)[0];
		}
	}
	if ((*pkt)->getATCommand() == "D4") {
		if ((*pkt)->size() > 0) {
			d4 = (**pkt)[0];
		}
	}
	if ((*pkt)->getATCommand() == "D5") {
		if ((*pkt)->size() > 0) {
			d5 = (**pkt)[0];
		}
	}


	if ((*pkt)->getATCommand() == "VR") {
			if ((*pkt)->size() > 1) {
				sprintf(buffer, "%02X%02X", (**pkt)[0],(**pkt)[1]);

				swversion = std::string(buffer);
				//std::cout << "swversion: " << swversion << std::endl;
			}
	}

	if ((*pkt)->getATCommand() == "HV") {
				if ((*pkt)->size() > 1) {
					if ((**pkt)[0] == 25) {
						sprintf(buffer, "XBee_%2X%2X",(**pkt)[0],(**pkt)[1]);
					}
					else {
						sprintf(buffer, "XBee_PRO_%2X%2X",(**pkt)[0],(**pkt)[1]);
					}
					hwversion = std::string(buffer);
					//std::cout << "hwversion: " << hwversion << std::endl;
				}
	}

/*
	for (int i = 0; i < (*pkt)->size(); i++) {
			 printf("rx: 0x%02X  %d\n", (**pkt)[i],(**pkt)[i]);
	}
*/

}

char* rematcon::get_xbeedata() {
	char *msg = new char[100];

	sprintf(msg,"DB=%d VR=%s HV=%s IR=%d IC=%d D0=%d D1=%d D2=%d D3=%d D4=%d D5=%d", rssi, swversion.c_str(), hwversion.c_str(),ir,ic,d0,d1,d2,d3,d4,d5);
	return msg;
}
