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

#ifndef REMOTENODE_H_
#define REMOTENODE_H_


#include <cstdlib>
#include <xbeep.h>
#include <stdint.h>
#include "datacon.h"
#include "iocon.h"
#include "rematcon.h"


class remotenode {
	public:
		explicit remotenode(std::string name, uint16_t addr16, uint64_t addr64):
			name(name),
			addr16(addr16),
			addr64(addr64),
			socket(NULL),
			has_data_conn(false),
			data_conn(NULL){ };
		explicit remotenode(std::vector<unsigned char> nd_payload);
		explicit remotenode() {};
		remotenode(const remotenode& other); // Copy constructor
		~remotenode(void);

		void init_data_connection(libxbee::XBee &parent);
		void init_io_connection();
		void send_message(const char *msg);
		void send_atcommand(const char *at, const int value);
		void send_atcommand(const char *at);
		void set_io(const int port, const int value);
		void set_socket(ServerSocket *s);
		datacon *get_data_conn(void) { return data_conn; }
		rematcon *get_at_conn(void) { return at_conn; }
		std::string getName(void)     const { return name;                        }
		uint16_t    getAddr16(void)   const { return addr16;                      }
		uint64_t    getAddr64(void)   const { return addr64;                      }
		uint32_t    getAddr64Hi(void) const { return (addr64 >> 32) & 0xFFFFFFFF; }
		uint32_t    getAddr64Lo(void) const { return addr64 & 0xFFFFFFFF;         }
		uint16_t	getParentNetworkAddr16(void)	const { return parent_network_addr16; }
		int			getDeviceType(void) 			const { return device_type; }
		int		    getStatus(void)					const { return status; }
		uint16_t	getProfileId(void)				const { return profile_id; }
		uint16_t	getManufacturerId(void)			const { return manufacturer_id; }
		struct xbee_conAddress* getAddr() { return &addr; }
		std::string getHWVersion() const { return at_conn->get_hwversion(); }
		void		getXbeeData();

	private:
		std::string name;
		uint16_t addr16;
		uint64_t addr64;
	public:
		ServerSocket *socket;

	private:
		bool has_data_conn = false;
		datacon *data_conn;
		struct xbee_conAddress addr;
		bool has_io_conn = false;
		uint16_t parent_network_addr16;
		int device_type;
		int status;
		uint16_t profile_id;
		uint16_t manufacturer_id;
		libxbee::XBee *parent_xbee;
		rematcon *at_conn;
		iocon 	*io_conn;
};



#endif /* REMOTENODE_H_ */
