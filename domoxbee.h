//-----------------------------------------------------------------------------
//
// DomoXBee a C++/C-wrapper to add XBee support to DomotiGa.
//
// DomotiGa - an open source home automation program
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
#ifndef DOMOXBEE_H_
#define DOMOXBEE_H_

#include <xbeep.h>

extern "C" {

// Initialize and destroy the wrapper
int DomoXBee_Init( const char* serialport, const int baudrate, const int timeout, const int tcpport);

bool DomoXBee_Discover(const int timeout);

void DomoXBee_Destroy( );


// Send commands to Arduino board (through serial port of XBee)
int DomoXBee_SendCmd(const char* xbee_name, const char* cmd);

// Set value to some port of the XBee
int DomoXBee_SetIO(const char* xbee_name, const int port, const int value);

// Get some parameters from XBee node
bool DomoXBee_GetXBeeData(const char* xbee_name);

// Start listener to get info about IO ports changes
bool DomoXBee_start_IO_listener(const char* xbee_name);

const char* DomoXBee_GetNodeName(const int index);

const char* DomoXBee_GetNodeAddress(const int index);

void DomoXbee_WaitForTCPConnection();

//
void DomoXBee_Log( bool logging );



const char* DomoXBee_Version( );

}


#endif /* DOMOXBEE_H_ */
