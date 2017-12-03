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

#ifndef ATCON_H_
#define ATCON_H_

#include <cstdlib>
#include <xbeep.h>
#include "remotenode.h"
#include <map>


class atcon: public libxbee::ConCallback {
	public:
		explicit atcon(libxbee::XBee &parent, std::string type, struct xbee_conAddress *address = NULL):
			libxbee::ConCallback(parent, type, address),
			xbee(&parent){ };
		~atcon(void);
		void xbee_conCallback(libxbee::Pkt **pkt);

		void set_socket(ServerSocket *s) { socket = s; }
		void start_node_detect();
		void set_dio(int io, int value);
	    ServerSocket *socket;
	    libxbee::XBee *xbee;
		std::map<std::string, remotenode*> node_map;
};


#endif /* ATCON_H_ */
