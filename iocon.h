/*
 * iocon.h
 *
 *  Created on: 15/10/2016
 *      Author: fjaraujo
 */

#ifndef IOCON_H_
#define IOCON_H_

#include <cstdlib>
#include <xbeep.h>
#include "ServerSocket.h"


class iocon: public libxbee::ConCallback {
	public:
		explicit iocon(libxbee::XBee &parent, std::string type, struct xbee_conAddress *address = NULL, std::string xbee_name = "", ServerSocket *s = NULL): libxbee::ConCallback(parent, type, address){socket=s; node_name = xbee_name; aux="";};
		void xbee_conCallback(libxbee::Pkt **pkt);
		void set_socket(ServerSocket *s) { socket = s; }
		~iocon(void);
	 	std::string myData;
		std::string aux;
		const ServerSocket *socket;
		std::string node_name;
		int iovalues[14];
};



#endif /* IOCON_H_ */
