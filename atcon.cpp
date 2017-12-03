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

#include "atcon.h"
#include "remotenode.h"
#include <iostream>


atcon::~atcon(void) {

	//std::cout << "Delete atcon START" << std::endl;

	node_map.clear();

	//std::cout << "Delete atcon END" << std::endl;

}


void atcon::xbee_conCallback(libxbee::Pkt **pkt) {
	char msg[100];
	remotenode *node;
	std::map<std::string,remotenode*>::iterator it;

	/*
	int i;
	for (i = 0; i < (*pkt)->size(); i++) {
		std::cout << (**pkt)[i];
	}
	std::cout << "\n";
*/
	int i;

	for (i = 0; i < (*pkt)->size(); i++) {
		//printf("rx: 0x%02X  %d\n", (**pkt)[i],(**pkt)[i]);
	}

	if ((*pkt)->getATCommand() != "ND") {
		//std::cout << (*pkt)->getATCommand() << std::endl;
		return;
	}


	std::vector<unsigned char> data = (*pkt)->getVector();

	/* push a new remotenode object into our map */

	node = new remotenode(data);

	sprintf(msg,"[NODE %s 0x%04X 0x%08X 0x%08X 0x%04X %d %d %d %d]\n",
			node->getName().c_str(), node->getAddr16(), node->getAddr64Hi(), node->getAddr64Lo(),
			node->getParentNetworkAddr16(), node->getDeviceType(), node->getStatus(),
			node->getProfileId(), node->getManufacturerId());

	// Send through the socket the info about the node discovered
	if (socket != NULL) {
		(*socket) << msg;
	}
	else {
		std::cout << msg;
	}

	std::string key(node->getName());

	it = this->node_map.find(key);
	if (it == this->node_map.end()) {
		node->init_data_connection(*xbee);
		node->set_socket(socket);

		// Send AT commands to get technical data of the node

		node->send_atcommand("DB"); // -> RSSI (-DBm)
		node->send_atcommand("VR"); // -> Firmware version
		node->send_atcommand("HV"); // -> Hardware version
		node->send_atcommand("IR"); // -> IO Sample Rate
		node->send_atcommand("IC"); // -> IO Digital Change Detection
		node->send_atcommand("D0"); // -> Config of port D0
		node->send_atcommand("D1"); // -> Config of port D1
		node->send_atcommand("D2"); // -> Config of port D2
		node->send_atcommand("D3"); // -> Config of port D3
		node->send_atcommand("D4"); // -> Config of port D4
		node->send_atcommand("D5"); // -> Config of port D5

		// It is a new node, and is inserted in the node_map
		this->node_map[node->getName()] = node;
	}

	//delete node;

}

void atcon::start_node_detect() {
	/* clear the list of it's previous results */
	//this->node_map.clear();

	/* and execute the 'ND' command */
	try {
		*this << "ND";
	} catch (xbee_err &err) {
		/* we expect the ND command to return -17 / XBEE_ETIMEOUT because it's a long running operation */
		if (err != -17) {
			std::cout << "XBEE_ETIMEOUT" << std::endl;
		}
	}
	catch (xbee_errors &err) {
			std::cout << "Error at start_node_detect: " << err << std::endl;
	}
}
