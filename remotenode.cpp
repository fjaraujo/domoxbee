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

#include "remotenode.h"
#include "SocketException.h"
#include <iostream>
#include <sstream>

#include <string.h>


remotenode::~remotenode(void) {

	// this->parent_xbee->conUnregister(data_conn);
	//std::cout << "remotenode " << this->name << " deleted!" << std::endl;
}


remotenode::remotenode(const remotenode& other) {

	//std::cout << "copying " << other.name << std::endl;

	memset(&this->addr,0,sizeof(this->addr));
	this->addr.addr64_enabled = other.addr.addr64_enabled;
	for (int j=0; j < 8; j++) {
		this->addr.addr64[j] = other.addr.addr64[j];
	}
	this->socket = other.socket;
	this->addr16 = other.addr16;
	this->addr64 = other.addr64;
	this->device_type = other.device_type;
	this->manufacturer_id = other.manufacturer_id;
	this->name = other.name;
	this->parent_network_addr16 = other.parent_network_addr16;
	this->status = other.status;
	this->profile_id = other.profile_id;

	this->parent_xbee = other.parent_xbee;

	this->data_conn = other.data_conn;
	this->at_conn = other.at_conn;

	this->has_data_conn = other.has_data_conn;

	/*
	if (other.has_data_conn) {
		this->parent_xbee->conUnregister(other.data_conn);
		this->data_conn = new datacon(*(this->parent_xbee),"Data",&this->addr,this->name, this->socket);
		this->has_data_conn = true;
	}
	else {
		this->has_data_conn = false;
		this->data_conn = NULL;
	}
*/
}

remotenode::remotenode(std::vector<unsigned char> nd_payload) {
	unsigned int i = 0;
	unsigned int j = 0;
	bool end_of_string = false;

	this->addr16  = (nd_payload[i + 0] << 8) & 0xFF00;
	this->addr16 |= (nd_payload[i + 1] << 0) & 0x00FF;

	memset(&this->addr, 0, sizeof(this->addr));

	this->addr.addr16_enabled = 0;
	this->addr.addr16[0] = (nd_payload[i + 0] << 8) & 0xFF00;
	this->addr.addr16[1] = (nd_payload[i + 1] << 0) & 0x00FF;

	i += 2;

	this->addr64 = nd_payload[i + 0] & 0xFF;
	this->addr64 = ((this->addr64 << 8) & ~0xFF) | (nd_payload[i + 1] & 0xFF);
	this->addr64 = ((this->addr64 << 8) & ~0xFF) | (nd_payload[i + 2] & 0xFF);
	this->addr64 = ((this->addr64 << 8) & ~0xFF) | (nd_payload[i + 3] & 0xFF);
	this->addr64 = ((this->addr64 << 8) & ~0xFF) | (nd_payload[i + 4] & 0xFF);
	this->addr64 = ((this->addr64 << 8) & ~0xFF) | (nd_payload[i + 5] & 0xFF);
	this->addr64 = ((this->addr64 << 8) & ~0xFF) | (nd_payload[i + 6] & 0xFF);
	this->addr64 = ((this->addr64 << 8) & ~0xFF) | (nd_payload[i + 7] & 0xFF);

	this->addr.addr64_enabled = 1;
	this->addr.addr64[0] = 0x00;
	for (j=1; j < 8; j++) {
		this->addr.addr64[j] = nd_payload[i + j];
	}


	i += 8;

	this->name = "";
	for (; i < nd_payload.size() && !end_of_string; i++) {
		if (nd_payload[i] == 0x00) {
			end_of_string = true;
		}
		else {
			this->name += char(nd_payload[i]);
		}
	}

	//std::cout << this->name.c_str() << " i= " << i << ", payload= " << nd_payload.size() << std::endl;

	this->parent_network_addr16  = (nd_payload[i + 0] << 8) & 0xFF00;
	this->parent_network_addr16 |= (nd_payload[i + 1] << 0) & 0x00FF;
	i += 2;

	this->device_type = nd_payload[i];
	i += 1;

	this->status = nd_payload[i];
	i +=1;

	this->profile_id  = (nd_payload[i + 0] << 8);
	this->profile_id |= (nd_payload[i + 1] << 0);
	i += 2;

	this->manufacturer_id  = (nd_payload[i + 0] << 8);
	this->manufacturer_id |= (nd_payload[i + 1] << 0);

	this->has_data_conn = false;

	//this->name += '\0';
}


void remotenode::init_data_connection(libxbee::XBee &parent) {
	if (this->has_data_conn) {
		return;
	}
	this->parent_xbee = &parent;
	this->has_data_conn = true;
	data_conn = new datacon(parent,"Data",&this->addr, this->name, this->socket);
	at_conn = new rematcon(parent,"Remote AT",&this->addr);

//	std::cout << "Init_data_connection of " << this->name << std::endl;

/*
 * ESTO AQUÍ DA PROBLEMAS, PORQUE SE LANZA DURANTE EL NODE DISCOVERY
*/	// keep DIO3 to HIGH by default
	this->set_io(3,5);
	// Send a reset signal
	usleep(500000);
	this->set_io(3,4);
	usleep(500000);
	this->set_io(3,5);

}

void remotenode::set_socket(ServerSocket *s) {
	this->socket = s;
	if (this->has_data_conn) {
		data_conn->set_socket(s);
	}
	if (this->has_io_conn) {
			io_conn->set_socket(s);
	}
}

void remotenode::init_io_connection() {
	if (this->has_io_conn) return;

	io_conn = new iocon(*this->parent_xbee,"I/O",&this->addr, this->name, this->socket);
	this->has_io_conn = true;
	if (this->socket != NULL) {
		io_conn->set_socket(this->socket);
	}

	// Now force sample to send the initial status of each IO port (checking IC mask)
	//(*at_conn) << "IC" + std::string(1, 0xFFFF);

	//(*at_conn) << "IS";

}

void remotenode::send_message(const char *msg) {
	std::string s(msg);

	//std::cout << "send_message START" << std::endl;

	if (data_conn != NULL) {
		(*data_conn) << s;
	}

	//std::cout << "send_message END" << std::endl;
	return;
}

/* Configure the IO port at XBee. Possible values:
 *
 * 0 : Disabled
 * 2 : Analog input
 * 3 : Digital input
 * 4 : Digital output, low
 * 5 : Digital oputput, high
 */

void remotenode::set_io(const int port, const int value) {

	try {
		if (at_conn != NULL) {
		switch (value) {
			case 5: // DIGITAL OUTPUT HIGH
				(*at_conn) << "D" + std::to_string(port) + std::string(1, 0x05);
				break;
			case 4: // DIGITAL OUTPUT LOW
				(*at_conn) << "D" + std::to_string(port) + std::string(1, 0x04);
				break;
			case 3: // DIGITAL INPUT
				(*at_conn) << "D" + std::to_string(port) + std::string(1, 0x03);
				break;
			case 0: // DISABLED
				(*at_conn) << "D" + std::to_string(port) + std::string(1, 0x00);
				break;
		}
	  }
	}
	catch (xbee_err &err) {
		std::cout << "ERROR at set_io (" << this->name << ", port= " << port << ", value= " << value << "): " << err << std::endl;
	}
}

void remotenode::send_atcommand(const char *at, const int value) {
	try {
			if (at_conn != NULL) {
					(*at_conn) << at + std::string(1, value);
			}
		}
		catch (xbee_err &err) {
			std::cout << "ERROR at send_atcommand: " << err << std::endl;
		}
}

void remotenode::send_atcommand(const char *at) {
	try {
			if (at_conn != NULL) {
					(*at_conn) << at;
			}
		}
		catch (xbee_err &err) {
			std::cout << "ERROR at send_atcommand: " << err << std::endl;
		}
}


void remotenode::getXbeeData() {
	char *msg;

	msg = at_conn->get_xbeedata();

	if (socket != NULL) {
		try {
			(*socket) << "[XBEE " + this->name + " " + msg + "]\n";
		}
		catch (SocketException &e) {
			std::cout << "Error at getXBeeData: " << e.description() << std::endl;
		}
	}
	delete msg;
}



