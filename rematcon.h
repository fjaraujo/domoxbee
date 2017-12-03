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

#ifndef REMATCON_H_
#define REMATCON_H_

#include <cstdlib>
#include <xbeep.h>
#include <map>



class rematcon: public libxbee::ConCallback {
	public:
		explicit rematcon(libxbee::XBee &parent, std::string type, struct xbee_conAddress *address = NULL):
			libxbee::ConCallback(parent, type, address){ };
		~rematcon(void);
		void xbee_conCallback(libxbee::Pkt **pkt);
		int get_rssi() const {return rssi; }
		int get_ir() const {return ir; }
		int get_ic() const {return ic; }
		std::string get_hwversion();
		std::string get_hwversion() const {return hwversion;}
		char* get_xbeedata();


	private:
		int rssi;
		std::string swversion = "";
		std::string hwversion = "";
		unsigned int ir;
		unsigned int ic;
		int d0; // Config of port d0
		int d1;
		int d2;
		int d3;
		int d4;
		int d5;
};


#endif /* REMATCON_H_ */
