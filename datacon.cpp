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

/* datacon.cpp
 *
 * This class manages the data callbacks from XBee network, i.e, messages coming from Arduino through serial port
 *
 */

#include "datacon.h"
#include <iostream>
#include <string.h>

#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <stdlib.h>
#include <string>
#include <pthread.h>
#include "SocketException.h"

using namespace std;

datacon::~datacon(void) {
	//std::cout << "datacon de " << this->node_name << " deleted!" << std::endl;
}

void datacon::xbee_conCallback(libxbee::Pkt **pkt) {
	int i;
	unsigned int j;
	myData = "";

	for (i = 0; i < (*pkt)->size(); i++) {
		myData += ((**pkt)[i]);
	}

	try {
		if (socket != NULL) {
			for (j = 0; j < myData.size(); j++) {
				if (myData[j] == '\n') {
					if (socket != NULL) {
						try {
							(*socket) << this->node_name.c_str() << "@" << aux + "\n";
						}
						catch (SocketException &e) {
							std::cout << "Error at xbee_conCallBack: " << e.description() << std::endl;
						}
					}
					else {
						std::cout << this->node_name.c_str() << "@" << aux + "\n";
					}
				//	fprintf(stream, "%s\n",aux.c_str());
					//std::cout  << this->node_name.c_str() << "@" << aux + "\n";
					aux = "";
				}
				else {
					aux += myData[j];
				}
			}
		}
	} catch (SocketException& e) {
		std::cout << "Exception was caught:" << e.description() << std::endl;
	}


}


