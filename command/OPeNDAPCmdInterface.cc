// OPeNDAPCmdInterface.cc

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

#include <unistd.h>
#include <iostream>
#include <sstream>

using std::endl ;
using std::stringstream ;

#include "OPeNDAPCmdInterface.h"
#include "OPeNDAPCmdParser.h"
#include "DODS.h"
#include "DODSLog.h"
#include "DODSBasicHttpTransmitter.h"
#include "DODSReturnManager.h"
#include "DODSTransmitException.h"
#include "OPeNDAPAggFactory.h"
#include "DODSAggregationServer.h"
#include "OPeNDAPTransmitterNames.h"
#include "OPeNDAPDataNames.h"

/** @brief Instantiate a OPeNDAPCmdInterface object

    @param dri DODSDataRequestInterface built using information from the
    apache module including the request and constraint passed as part of the
    URL.
    @see _DODSDataRequestInterface
 */
OPeNDAPCmdInterface::OPeNDAPCmdInterface()
    : DODS()
{
}

OPeNDAPCmdInterface::OPeNDAPCmdInterface( const string &cmd )
{
    _dhi.data[DATA_REQUEST] = cmd ;
}

OPeNDAPCmdInterface::~OPeNDAPCmdInterface()
{
    clean() ;
}

/** @brief Override execute_request in order to register memory pool

    Once the memory pool is initialized hand over control to parent class to
    execute the request. Once completed, unregister the memory pool.

    This needs to be done here instead of the initialization method
    because???
 */
int
OPeNDAPCmdInterface::execute_request()
{
    return DODS::execute_request() ;
}

/** @brief Initialize the DODS object from the apache environment

    First calls the parent initialization method in order to initialize all
    global variables.

    Once this is completed the DODSDataHandlerInterface is initialized given
    the DODSDataRequestInterface constructed within the module code.

    This includes retreiving the user information from the cookie created on
    the client side in the browser. The cookie name is defined in
    OPENDAP_USER_COOKIE above.

    Also creates the DODSBasicHttpTransmitter object in order to transmit
    the response object via http, setting the mime type and other header
    information for the response.

    @see DODSGlobalInit
 */
void
OPeNDAPCmdInterface::initialize()
{
    pid_t thepid = getpid() ;
    stringstream ss ;
    ss << thepid ;
    _dhi.data[SERVER_PID] = ss.str() ;

    DODS::initialize() ;
}

/** @brief Validate the incoming request information
 */
void
OPeNDAPCmdInterface::validate_data_request()
{
    DODS::validate_data_request() ;
}

/** @brief Build the data request plan using the OPeNDAPCmdParser.

    @see OPeNDAPCmdParser
 */
void
OPeNDAPCmdInterface::build_data_request_plan()
{
    if( DODSLog::TheLog()->is_verbose() )
    {
	*(DODSLog::TheLog()) << _dhi.data[SERVER_PID]
			     << " [" << _dhi.data[DATA_REQUEST] << "] building"
			     << endl ;
    }
    OPeNDAPCmdParser parser ;
    parser.parse( _dhi.data[DATA_REQUEST], _dhi ) ;

    if( _dhi.data[RETURN_CMD] != "" )
    {
	_transmitter = DODSReturnManager::TheManager()->find_transmitter( _dhi.data[RETURN_CMD] ) ;
	if( !_transmitter )
	{
	    throw DODSTransmitException( (string)"Unable to find transmitter " + _dhi.data[RETURN_CMD] ) ;
	}
    }
    else
    {
	string protocol = _dhi.transmit_protocol ;
	if( protocol != "HTTP" )
	{
	    _transmitter = DODSReturnManager::TheManager()->find_transmitter( BASIC_TRANSMITTER ) ;
	    if( !_transmitter )
	    {
		throw DODSTransmitException( (string)"Unable to find transmitter " + BASIC_TRANSMITTER ) ;
	    }
	}
	else
	{
	    _transmitter = DODSReturnManager::TheManager()->find_transmitter( HTTP_TRANSMITTER ) ;
	    if( !_transmitter )
	    {
		throw DODSTransmitException( (string)"Unable to find transmitter " + HTTP_TRANSMITTER ) ;
	    }
	}
    }
    if( DODSLog::TheLog()->is_verbose() )
    {
	*(DODSLog::TheLog()) << "Data Handler Interface:" << endl ;
	*(DODSLog::TheLog()) << "    action = " << _dhi.action << endl ;
	*(DODSLog::TheLog()) << "    transmit = "
	                     << _dhi.transmit_protocol
	                     << endl ;
	map< string, string>::const_iterator data_citer ;
	for( data_citer = _dhi.data.begin(); data_citer != _dhi.data.end(); data_citer++ )
	{
	    *(DODSLog::TheLog()) << "    " << (*data_citer).first << " = "
	                  << (*data_citer).second << endl ;
	}
    }
}

/** @brief Execute the data request plan

    Simply calls the parent method. Prior to calling the parent method logs
    a message to the dods log file.

    @see DODSLog
 */
void
OPeNDAPCmdInterface::execute_data_request_plan()
{
    *(DODSLog::TheLog()) << _dhi.data[SERVER_PID]
		         << " [" << _dhi.data[DATA_REQUEST] << "] executing"
		         << endl ;
    DODS::execute_data_request_plan() ;
}

/** @brief Invoke the aggregation server, if there is one

    Simply calls the parent method. Prior to calling the parent method logs
    a message to the dods log file.

    @see DODSLog
 */
void
OPeNDAPCmdInterface::invoke_aggregation()
{
    if( _dhi.data[AGG_CMD] == "" )
    {
	*(DODSLog::TheLog()) << _dhi.data[SERVER_PID]
			     << " [" << _dhi.data[DATA_REQUEST] << "]"
			     << " not aggregating, aggregation command empty"
			     << endl ;
    }
    else
    {
	DODSAggregationServer *agg = OPeNDAPAggFactory::TheFactory()->find_handler( _dhi.data[AGG_HANDLER] ) ;
	if( !agg )
	{
	    if( DODSLog::TheLog()->is_verbose() )
	    {
		*(DODSLog::TheLog()) << _dhi.data[SERVER_PID]
				     << " [" << _dhi.data[DATA_REQUEST] << "]"
				     << " not aggregating, no handler"
				     << endl ;
	    }
	}
	else
	{
	    if( DODSLog::TheLog()->is_verbose() )
	    {
		*(DODSLog::TheLog()) << _dhi.data[SERVER_PID]
				     << " [" << _dhi.data[DATA_REQUEST]
				     << "] aggregating" << endl ;
	    }
	    agg->aggregate( _dhi ) ;
	}
    }
    DODS::invoke_aggregation() ;
}

/** @brief Transmit the response object

    Simply calls the parent method. Prior to calling the parent method logs
    a message to the dods log file.

    @see DODSLog
 */
void
OPeNDAPCmdInterface::transmit_data()
{
    if( DODSLog::TheLog()->is_verbose() )
    {
	*(DODSLog::TheLog()) << _dhi.data[SERVER_PID]
			     << " [" << _dhi.data[DATA_REQUEST]
			     << "] transmitting" << endl ;
    }
    DODS::transmit_data() ;
} 

/** @brief Log the status of the request to the DODSLog file

    @see DODSLog
 */
void
OPeNDAPCmdInterface::log_status()
{
    *(DODSLog::TheLog()) << _dhi.data[SERVER_PID]
		         << " [" << _dhi.data[DATA_REQUEST] << "] completed"
		         << endl ;
}

/** @brief Clean up after the request is completed

    Calls the parent method clean and then logs to the DODSLog file saying
    that we are done and exiting the process. The exit actually takes place
    in the module code.

    @see DODSLog
 */
void
OPeNDAPCmdInterface::clean()
{
    DODS::clean() ;
    *(DODSLog::TheLog()) << _dhi.data[SERVER_PID]
		         << " [" << _dhi.data[DATA_REQUEST] << "] exiting"
		         << endl ;
}

// $Log: OPeNDAPCmdInterface.cc,v $
