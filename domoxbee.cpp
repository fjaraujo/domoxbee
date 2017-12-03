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

//-----------------------------------------------------------------------------
// Define and get version numbers of DomoXBee and the xbee library
//-----------------------------------------------------------------------------
char domoxbee_vers[] = "XBee version 3.0.5";

//-----------------------------------------------------------------------------

// system
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <pthread.h>
#include <ctime>
#include <sys/time.h>
#include <map>
#include <unistd.h>
#include <cstring>
#include <thread>

#include "ServerSocket.h"
#include "SocketException.h"
#include <string>


// wrapper
#include "domoxbee.h"
#include "atcon.h"

//using namespace DomoXBee;

/* setup libxbee */
libxbee::XBee static *xbee = NULL;  // static
atcon *con = NULL; // static

bool keep_alive = true;
bool server_running = false;

bool socket_client_connected = false;

ServerSocket *server_socket = NULL, *domotiga_socket = NULL; // static

FILE * pFileLog;

using namespace std;
///////////////////////////////////////////////////////////////////////////////
// C style bindings are required since we call these functions from gambas.
///////////////////////////////////////////////////////////////////////////////


extern "C" {


void task_start_socket(int port) {

	// Create the socket
	server_socket = new ServerSocket(port);

	domotiga_socket = new ServerSocket();

	std::cout << "Server Socket iniciado" <<  endl;
	server_running = true;

	server_socket->accept(*domotiga_socket);

	std::cout << "Cliente conectado" << endl;

	socket_client_connected = true;

	//usleep(5000);

	con->set_socket(domotiga_socket);
	// Update socket in all the nodes
	/*
	for (std::map<std::string, remotenode>::iterator n = con->node_map.begin(); n != con->node_map.end(); n++) {
		n->second.set_socket(domotiga_socket);
	}
*/

	try {
		while (keep_alive) {
			usleep(2000);
		}
	} catch (SocketException& e) {
		std::cout << "Exception was caught:" << e.description()
				<< "\nExiting.\n";
	}

	delete domotiga_socket;
	delete server_socket;

	server_running = false;

	std::cout << "Fin del proceso!" << endl;

	return;
}

int DomoXBee_Init( const char* serialport, const int baudrate, const int timeout, const int tcpport)
{
	std::stringstream stream;

	try
	    {

		xbee = new libxbee::XBee("xbeeZB", serialport, baudrate);

/*
		pFileLog = fopen ("/home/fjaraujo/domotiga/xbee_domotrix.log","w");
			if (pFileLog != NULL) {
				xbee->setLogTarget(pFileLog);
				xbee->setLogLevel(100);
			}
*/
		/* make a connection */
		con = new atcon(*xbee, "Local AT"); /* AT connection with a callback handler */


		// Now, set the Node Discovery Timeout at Coordinator. Max value = 25 seconds
		if (timeout <= 25) {
			(*con) << "NT" + std::string(1, timeout*10);
		}

		keep_alive = true;

		std::thread ts(task_start_socket, tcpport);
		ts.detach();

	  }
	  catch ( SocketException& e )
	    {
	      std::cout << "Exception was caught:" << e.description() << "\nExiting.\n";
	      return -2;
	    }
	  catch ( libxbee::xbee_etx &err) {
		  std::cout << "Exception was caught:" << err.retVal << "\nExiting.\n";
		  return -1;
	  }
	  catch (xbee_errors &err) {
	      	std::cout << "Init error: " << err << endl;
	      	return err;
	  }
	  catch (int err) {
  		std::cout << "Error " << err << "\n";
 	  }

	  while (!server_running);

	  return 0;
}


void task_discover_nodes(int timeout) {
	int i = 0;

	if (domotiga_socket == NULL) return;


	try
		{
		    /* start node detect */
			con->start_node_detect();

			/* lazy-wait for it to finish */
			i=0;
			while (i< timeout) {
				usleep(1000000); // wait 1 second
				i++;
			}
			if (domotiga_socket != NULL) {
				try {
					(*domotiga_socket) << "[DISCOVEROK]\n";
				}
				catch (SocketException &e) {
					std::cout << "Error at discover_nodes: " << e.description() << std::endl;
				}
			}
			else {
				std::cout << "[DISCOVER]\n";
			}

		}
		catch ( SocketException& e )
		{
			std::cout << "Exception was caught:" << e.description() << "\nExiting.\n";
		}
		catch ( libxbee::xbee_etx &err) {
			std::cout << "Exception was caught:" << err.retVal << "\nExiting.\n";
		}
		catch (xbee_errors &err) {
			std::cout << "Error at discovery: " << err << endl;
		}

		/*
		for (i = 0; i < con->node_map.size(); i++) {
			usleep(3000000); // wait 3 seconds for each node discovered, to give them time to complete reset
			i++;
		}*/
		//return con->node_map.size();
}

bool DomoXBee_Discover(const int timeout)
{

	if (xbee == NULL) {
		return false;
	}

	try {
		std::thread ts(task_discover_nodes, timeout);
		std::cout << "Lanzo discover nodes" << endl;
		ts.detach();
	}
	catch ( libxbee::xbee_etx &err) {
		std::cout << "Exception was caught:" << err.retVal << "\nExiting.\n";
	}
	catch (xbee_errors &err) {
		std::cout << "Error at discovery: " << err << endl;
	}
	return true;
}

int DomoXBee_SendCmd(const char* xbee_name, const char* cmd) {

	std::map<std::string,remotenode*>::iterator it;
	std::string key(xbee_name);

//	std::cout << "node_map size: " << con->node_map.size() << std::endl;

    try {
		it = con->node_map.find(key);
		if (it != con->node_map.end()) {
		   remotenode* node = it->second;
		   node->send_message(cmd);
		   return 0;
		}
		return -30;
    }
    catch (libxbee::xbee_etx &err) {
    	std::cout << err.retVal << endl;
   		return err.retVal;
    }
    catch (xbee_errors &err) {
    	std::cout << err << endl;
    	return err;
    }

}

int DomoXBee_SetIO(const char* xbee_name, const int port, const int value) {

	std::map<std::string,remotenode*>::iterator it;
	std::string key(xbee_name);

	try {
		it = con->node_map.find(key);
		if (it != con->node_map.end()) {
		  remotenode* node = it->second;
		  node->set_io(port,value);
		  return 0;
		}
		return -30;
	}
	catch (libxbee::xbee_etx &err) {
		return err.retVal;
	}
	catch (xbee_errors &err) {
	   	return err;
	}
}

bool DomoXBee_GetXbeeData(const char* xbee_name) {

	std::map<std::string,remotenode*>::iterator it;
	std::string key(xbee_name);

	try {
		it = con->node_map.find(key);
		if (it != con->node_map.end()) {
		  remotenode* node = it->second;
		  node->getXbeeData();
		  return true;
		}
		return false;
	}
	catch (libxbee::xbee_etx &err) {
		return false;
	}
	catch (xbee_errors &err) {
	   	return false;
	}
}

// Start listener to get info about IO ports changes
bool DomoXBee_start_IO_listener(const char* xbee_name) {
	std::map<std::string,remotenode*>::iterator it;
	std::string key(xbee_name);

	try {
		it = con->node_map.find(key);
		if (it != con->node_map.end()) {
			remotenode* node = it->second;
			node->init_io_connection();
			return true;
		}
		return false;
	} catch (libxbee::xbee_etx &err) {
		return false;
	} catch (xbee_errors &err) {
		return false;
	}
}

const char* DomoXBee_GetNodeName(const int index) {
	try {
		if (con->node_map.size() > (unsigned)index)
		{
			std::map<std::string, remotenode*>::iterator it = con->node_map.begin();
			std::advance(it, index);
			return it->second->getName().c_str();
		}
	} catch (int err) {
  		std::cout << "Error " << err << "\n";
 	}
	return NULL;
}

const char* DomoXBee_GetNodeAddress(const int index) {
	char *buffer;

	try {
		if (con->node_map.size() > (unsigned)index)
		{
			std::map<std::string, remotenode*>::iterator it = con->node_map.begin();
			std::advance(it, index);
		    buffer = new char[7];
			snprintf ( buffer, 7, "0x%04X", it->second->getAddr16());
			return buffer;
		}
	} catch (int err) {
  		std::cout << "Error " << err << "\n";
 	}
	return NULL;
}

//-----------------------------------------------------------------------------
// <XBee_Log>
//-----------------------------------------------------------------------------

void DomoXBee_Log( bool logging )
{


}


void DomoXbee_WaitForTCPConnection()
{
	while (!socket_client_connected) {
		usleep(10000);
	}
}


//-----------------------------------------------------------------------------
// <DomoXBee_Destroy>
// Stops the XBee wrapper
//-----------------------------------------------------------------------------

void DomoXBee_Destroy( )
{
//	std::cout << "DomoXBee_Destroy START" << std::endl;


	for (std::map<std::string, remotenode*>::iterator n = con->node_map.begin(); n != con->node_map.end(); n++) {
		xbee->conUnregister(n->second->get_data_conn());
		xbee->conUnregister(n->second->get_at_conn());
		//delete (n->second.get_data_conn()); // warning: deleting object of polymorphic class type ‘datacon’ which has non-virtual destructor might cause undefined behaviour [-Wdelete-non-virtual-dtor]
		//delete (n->second.get_at_conn());
	}

	if (con != NULL) {
		xbee->conUnregister(con);
		// delete con;  // warning: deleting object of polymorphic class type ‘atcon’ which has non-virtual destructor might cause undefined behaviour [-Wdelete-non-virtual-dtor]
		delete xbee;
	}


//	std::cout << "DomoXBee_Destroy END" << std::endl;
	keep_alive = false;

	usleep(5000000);
	while (server_running);
}

//-----------------------------------------------------------------------------
// <DomoXBee_Version>
// Returns the readable xbee wrapper revision
//-----------------------------------------------------------------------------

const char* DomoXBee_Version( )
{
	return "1.0";
}


}


// ***********************************************************************************************************************************




