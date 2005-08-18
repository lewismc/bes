// UnixSocket.cc

// 2005 Copyright University Corporation for Atmospheric Research

#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <iostream>

using std::cout ;
using std::endl ;
using std::flush ;

#include "UnixSocket.h"
#include "SocketException.h"
#include "SocketUtilities.h"

void
UnixSocket::connect()
{
    if( _listening )
    {
	string err( "Socket is already listening" ) ;
	throw SocketException( err, __FILE__, __LINE__ ) ;
    }

    if( _connected )
    {
	string err( "Socket is already connected" ) ;
	throw SocketException( err, __FILE__, __LINE__ ) ;
    }

    char path[500] = "" ;
    getcwd( path, sizeof( path ) ) ;
    _tempSocket = path ;
    _tempSocket += "/" ;
    _tempSocket += SocketUtilities::create_temp_name() ;
    _tempSocket += ".unixSocket" ;
    // maximum path for struct sockaddr_un.sun_path is 108
    // get sure we will not exceed to max for creating sockets
    cout << "creating client unix socket@" << _tempSocket << endl ;
    if( _tempSocket.length() > 108 )
    {
	string msg = "path to temporary unix socket " ;
	msg += _tempSocket + " is too long" ;
	throw( SocketException( msg, __FILE__, __LINE__ ) ) ;
    }

    struct sockaddr_un client_addr ;
    struct sockaddr_un server_addr ;

    strcpy( server_addr.sun_path, _unixSocket.c_str() ) ;
    server_addr.sun_family = AF_UNIX ;

    int descript = socket( AF_UNIX, SOCK_STREAM, 0 ) ;
    if( descript != -1 )
    {
	strcpy( client_addr.sun_path, _tempSocket.c_str() ) ;
	client_addr.sun_family = AF_UNIX ;

	cout << "Trying to bind to socket ... " << flush ;
	int clen = sizeof( client_addr.sun_family ) ;
	clen += strlen( client_addr.sun_path )  ;
	if( bind( descript, (struct sockaddr*)&client_addr, clen ) != -1 )
	{
	    cout << "OK" << endl ;
	    cout << "Trying to connect to sever ... " << flush ;
	    int slen = sizeof( server_addr.sun_family ) ;
	    slen += strlen( server_addr.sun_path ) ;
	    if( ::connect( descript, (struct sockaddr*)&server_addr, slen ) != -1)
	    {
		cout << "OK" << endl ;
		_socket = descript ;
		_connected = true ;
	    }
	    else
	    {
		string msg = "could not connect via " ;
		msg += _unixSocket ;
		char *err = strerror( errno ) ;
		if( err )
		    msg = msg + "\n" + err ;
		else
		    msg = msg + "\nCould not retrieve error message" ;
		throw SocketException( msg, __FILE__, __LINE__ ) ;
	    }
	}
	else
	{
	    string msg = "could not bind to Unix socket " ;
	    msg += _tempSocket ;
	    char *err = strerror( errno ) ;
	    if( err )
		msg = msg + "\n" + err ;
	    else
		msg = msg + "\nCould not retrieve error message" ;
	    throw SocketException( msg, __FILE__, __LINE__ ) ;
	}
    }
    else
    {
	string msg = "could not create a Unix socket" ;
	char *err = strerror( errno ) ;
	if( err )
	    msg = msg + "\n" + err ;
	else
	    msg = msg + "\nCould not retrieve error message" ;
	throw SocketException( msg, __FILE__, __LINE__ ) ;
    }
}

void
UnixSocket::listen()
{
    if( _connected )
    {
	string err( "Socket is already connected" ) ;
	throw SocketException( err, __FILE__, __LINE__ ) ;
    }

    if( _listening )
    {
	string err( "Socket is already listening" ) ;
	throw SocketException( err, __FILE__, __LINE__ ) ;
    }

    int on = 1 ;
    static struct sockaddr_un server_add ;
    cout << "Trying to get Unix socket ... " << flush ;
    _socket = socket( AF_UNIX,SOCK_STREAM, 0 ) ;
    if( _socket >= 0 )
    {
	cout << "OK" << endl ;
	server_add.sun_family = AF_UNIX;
	strcpy( server_add.sun_path, _unixSocket.c_str() ) ;
	unlink( _unixSocket.c_str() ) ;
	cout << "Trying to set Unix socket properties ... " << flush ;
	if( !setsockopt( _socket, SOL_SOCKET, SO_REUSEADDR,
	                 (char*)&on, sizeof( on ) ) )
	{
	    cout << "OK" << endl ;
	    cout << "Trying to bind Unix socket ... " << flush ;
	    if( bind( _socket, (struct sockaddr*)&server_add, sizeof( server_add.sun_family ) + strlen( server_add.sun_path ) ) != -1)
	    {
		string is_ok = (string)"OK binding to " + _unixSocket ;
		cout << is_ok.c_str() << endl ;
		cout << "Trying to listen to Unix socket ... " << flush ;
		if( ::listen( _socket, 5 ) == 0 )
		{
		    _listening = true ;
		    cout << "OK" << endl ;
		}
		else
		{
		    cout << "FAILED" << endl ;
		    string error( "could not listen Unix socket" ) ;
		    const char* error_info = strerror( errno ) ;
		    if( error_info )
			error += " " + (string)error_info ;
		    throw SocketException( error, __FILE__, __LINE__ ) ;
		}
	    }
	    else
	    {
		cout << "FAILED" << endl ;
		string error( "could not bind Unix socket" ) ;
		const char* error_info = strerror( errno ) ;
		if( error_info )
		    error += " " + (string)error_info ;
		throw SocketException( error, __FILE__, __LINE__ ) ;
	    }
	}
	else
	{
	    cout << "FAILED" << endl ;
	    string error( "could not set SO_REUSEADDR on Unix socket" ) ;
	    const char *error_info = strerror( errno ) ;
	    if( error_info )
		error += " " + (string)error_info ;
	    throw SocketException( error, __FILE__, __LINE__ ) ;
	}
    }
    else
    {
	cout << "FAILED" << endl ;
	string error( "could not get Unix socket" ) ;
	const char *error_info = strerror( errno ) ;
	if( error_info )
	    error += " " + (string)error_info ;
	throw SocketException( error, __FILE__, __LINE__ ) ;
    }
}

void
UnixSocket::close()
{
    Socket::close() ;
    if( _tempSocket != "" )
    {
	if( !access( _tempSocket.c_str(), F_OK ) )
	{
	    remove( _tempSocket.c_str() ) ;
	}
    }
}

// $Log: UnixSocket.cc,v $
