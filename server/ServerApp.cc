// ServerApp.cc

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// Copyright (c) 2004,2005 University Corporation for Atmospheric Research
// Author: Patrick West <pwest@ucar.org> and Jose Garcia <jgarcia@ucar.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact University Corporation for Atmospheric Research at
// 3080 Center Green Drive, Boulder, CO 80301
 
// (c) COPYRIGHT University Corporation for Atmostpheric Research 2004-2005
// Please read the full copyright statement in the file COPYRIGHT_UCAR.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>
//      jgarcia     Jose Garcia <jgarcia@ucar.edu>

#include <signal.h>
#include <unistd.h>

#include <iostream>

using std::cout ;
using std::cerr ;
using std::endl ;
using std::flush ;

#include "ServerApp.h"
#include "ServerExitConditions.h"
#include "TheDODSKeys.h"
#include "SocketListener.h"
#include "TcpSocket.h"
#include "UnixSocket.h"
#include "OPeNDAPServerHandler.h"
#include "DODSException.h"
#include "PPTException.h"
#include "PPTServer.h"
#include "PPTException.h"
#include "SocketException.h"
#include "DODSMemoryManager.h"

#include "default_module.h"
#include "opendap_commands.h"

ServerApp::ServerApp()
    : OPeNDAPModuleApp(),
      _portVal( 0 ),
      _gotPort( false ),
      _unixSocket( "" ),
      _secure( false ),
      _mypid( 0 ),
      _ts( 0 ),
      _us( 0 ),
      _ps( 0 )
{
    _mypid = getpid() ;
    cout << "**** my pid = " << _mypid << endl ;
}

ServerApp::~ServerApp()
{
}

void
ServerApp::signalTerminate( int sig )
{
    if( sig == SIGTERM )
    {
	cout << OPeNDAPApp::TheApplication()->appName() << " : " << getpid()
	     << ": got terminate signal, exiting!" << endl ;
	OPeNDAPApp::TheApplication()->terminate( sig ) ;
	exit( SERVER_EXIT_NORMAL_SHUTDOWN ) ;
    }
}

void
ServerApp::signalInterrupt( int sig )
{
    if( sig == SIGINT )
    {
	cout << OPeNDAPApp::TheApplication()->appName() << " : " << getpid()
	     << ": got interrupt signal, exiting!" << endl ;
	OPeNDAPApp::TheApplication()->terminate( sig ) ;
	exit( SERVER_EXIT_NORMAL_SHUTDOWN ) ;
    }
}

void
ServerApp::signalRestart( int sig )
{
    if( sig == SIGUSR1 )
    {
	cout << OPeNDAPApp::TheApplication()->appName() << " : " << getpid()
	     << ": got restart signal." << endl ;
	OPeNDAPApp::TheApplication()->terminate( sig ) ;
	exit( SERVER_EXIT_RESTART ) ;
    }
}

void
ServerApp::showUsage()
{
    cout << OPeNDAPApp::TheApplication()->appName()
         << ": -d -v -s -p <PORT> -u <UNIX_SOCKET>" << endl ;
    cout << "-d set the server to debugging mode" << endl ;
    cout << "-v echos version and exit" << endl ;
    cout << "-s specifies a secure server using SLL authentication" << endl ;
    cout << "-p set port to PORT" << endl ;
    cout << "-u set unix socket to UNIX_SOCKET" << endl ;
    exit( 0 ) ;
}

void
ServerApp::showVersion()
{
  cout << OPeNDAPApp::TheApplication()->appName() << " version 2.0" << endl ;
  exit( 0 ) ;
}

int
ServerApp::initialize( int argc, char **argv )
{
    cout << "Trying to register SIGTERM ... " << flush ;
    if( signal( SIGTERM, signalTerminate ) == SIG_ERR )
    {
	cerr << "FAILED: Can not register SIGTERM signal handler" << endl ;
	exit( SERVER_EXIT_FATAL_CAN_NOT_START ) ;
    }
    else
	cout << "OK" << endl ;

    cout << "Trying to register SIGINT ... " << flush ;
    if( signal( SIGINT, signalInterrupt ) == SIG_ERR )
    {
	cerr << "FAILED: Can not register SIGINT signal handler" << endl ;
	exit( SERVER_EXIT_FATAL_CAN_NOT_START ) ;
    }
    else
	cout << "OK" << endl ;

    cout << "Trying to register SIGUSR1 ... " << flush ;
    if( signal( SIGUSR1, signalRestart ) == SIG_ERR )
    {
	cerr << "FAILED: Can not register SIGUSR1 signal handler" << endl ;
	exit( SERVER_EXIT_FATAL_CAN_NOT_START ) ;
    }
    else
	cout << "OK" << endl ;

    int c = 0 ;

    while( ( c = getopt( argc, argv, "dvsp:u:" ) ) != EOF )
    {
	switch( c )
	{
	    case 'p':
		_portVal = atoi( optarg ) ;
		_gotPort = true ;
		break ;
	    case 'u':
		_unixSocket = optarg ;
		break ;
	    case 'd':
		setDebug( true ) ;
		break ;
	    case 'v':
		showVersion() ;
		break ;
	    case 's':
		_secure = true ;
		cout << "**** server is secure" << endl ;
		break ;
	    case '?':
		showUsage() ;
		break ;
	}
    }

    bool found = false ;
    string key ;
    if( !_gotPort )
    {
	key = "OPeNDAP.ServerPort" ;
	string sPort = TheDODSKeys::TheKeys()->get_key( key, found ) ;
	_portVal = atoi( sPort.c_str() ) ;
	if( !found || _portVal == 0 )
	{
	    cout << endl << "Unable to determine server port" << endl ;
	    cout << "Please specify on the command line with -p <port>"
	         << endl
		 << "Or specify in the opendap configuration file with " << key
		 << endl << endl ;
	    showUsage() ;
	}
    }

    found = false ;
    if( _unixSocket == "" )
    {
	key = "OPeNDAP.ServerUnixSocket" ;
	_unixSocket = TheDODSKeys::TheKeys()->get_key( key, found ) ;
	if( !found || _unixSocket == "" )
	{
	    cout << endl << "Unable to determine unix socket" << endl ;
	    cout << "Please specify on the command line with -u <unix_socket>"
	         << endl
		 << "Or specify in the opendap configuration file with " << key
		 << endl << endl ;
	    showUsage() ;
	}
    }

    found = false ;
    if( _secure == false )
    {
	key = "OPeNDAP.ServerSecure" ;
	string isSecure = TheDODSKeys::TheKeys()->get_key( key, found ) ;
	if( isSecure == "Yes" || isSecure == "YES" || isSecure == "yes" )
	{
	    cout << "**** server is secure" << endl ;
	    _secure = true ;
	}
    }

    default_module::initialize( argc, argv ) ;
    opendap_commands::initialize( argc, argv ) ;

    return OPeNDAPModuleApp::initialize( argc, argv ) ;
}

int
ServerApp::run()
{
    try
    {
	DODSMemoryManager::initialize_memory_pool() ;

	SocketListener listener ;

	_ts = new TcpSocket( _portVal ) ;
	listener.listen( _ts ) ;

	_us = new UnixSocket( _unixSocket ) ;
	listener.listen( _us ) ;

	OPeNDAPServerHandler handler ;

	_ps = new PPTServer( &handler, &listener, _secure ) ;
	_ps->initConnection() ;
    }
    catch( SocketException &se )
    {
	cerr << "caught SocketException" << endl ;
	cerr << se.getMessage() << endl ;
	return 1 ;
    }
    catch( PPTException &pe )
    {
	cerr << "caught PPTException" << endl ;
	cerr << pe.getMessage() << endl ;
	return 1 ;
    }
    catch( ... )
    {
	cerr << "caught unknown exception" << endl ;
	return 1 ;
    }

    return 0 ;
}

int
ServerApp::terminate( int sig )
{
    pid_t apppid = getpid() ;
    if( apppid == _mypid )
    {
	if( _ps )
	{
	    _ps->closeConnection() ;
	    delete _ps ;
	}
	if( _ts )
	{
	    _ts->close() ;
	    delete _ts ;
	}
	if( _us )
	{
	    _us->close() ;
	    delete _us ;
	}
	OPeNDAPModuleApp::terminate( sig ) ;
    }
    return sig ;
}

int
main( int argc, char **argv )
{
    try
    {
	ServerApp app ;
	return app.main( argc, argv ) ;
    }
    catch( DODSException &e )
    {
	cerr << "Caught unhandled exception: " << endl ;
	cerr << e.get_error_description() << endl ;
	return 1 ;
    }
    catch( ... )
    {
	cerr << "Caught unhandled, unknown exception" << endl ;
	return 1 ;
    }
    return 0 ;
}

