// BESDapModule.cc

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// Copyright (c) 2004,2005 University Corporation for Atmospheric Research
// Author: Patrick West <pwest@ucar.edu>
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

#include <iostream>

using std::endl ;

#include "BESDapModule.h"
#include "BESLog.h"

#include "BESDapRequestHandler.h"
#include "BESRequestHandlerList.h"

#include "BESResponseNames.h"
#include "BESResponseHandlerList.h"

#include "BESDASResponseHandler.h"
#include "BESDDSResponseHandler.h"
#include "BESDataResponseHandler.h"
#include "BESDDXResponseHandler.h"

#include "BESCatalogResponseHandler.h"

#include "BESDapTransmit.h"
#include "BESTransmitter.h"
#include "BESReturnManager.h"
#include "BESTransmitterNames.h"

void
BESDapModule::initialize( const string &modname )
{
    if( BESLog::TheLog()->is_verbose() )
	(*BESLog::TheLog()) << "Initializing OPeNDAP modules:" << endl;

    if( BESLog::TheLog()->is_verbose() )
	(*BESLog::TheLog()) << "    adding " << modname << " request handler" 
		      << endl ;
    BESRequestHandlerList::TheList()->add_handler( modname, new BESDapRequestHandler( modname ) ) ;

    if( BESLog::TheLog()->is_verbose() )
	(*BESLog::TheLog()) << "    adding " << DAS_RESPONSE << " response handler" << endl;
    BESResponseHandlerList::TheList()->add_handler( DAS_RESPONSE, BESDASResponseHandler::DASResponseBuilder ) ;

    if( BESLog::TheLog()->is_verbose() )
	(*BESLog::TheLog()) << "    adding " << DDS_RESPONSE << " response handler" << endl;
    BESResponseHandlerList::TheList()->add_handler( DDS_RESPONSE, BESDDSResponseHandler::DDSResponseBuilder ) ;

    if( BESLog::TheLog()->is_verbose() )
	(*BESLog::TheLog()) << "    adding " << DDX_RESPONSE << " response handler" << endl;
    BESResponseHandlerList::TheList()->add_handler( DDX_RESPONSE, BESDDXResponseHandler::DDXResponseBuilder ) ;

    if( BESLog::TheLog()->is_verbose() )
	(*BESLog::TheLog()) << "    adding " << DATA_RESPONSE << " response handler" << endl;
    BESResponseHandlerList::TheList()->add_handler( DATA_RESPONSE, BESDataResponseHandler::DataResponseBuilder ) ;

    if( BESLog::TheLog()->is_verbose() )
	(*BESLog::TheLog()) << "    adding " << CATALOG_RESPONSE << " response handler" << endl;
    BESResponseHandlerList::TheList()->add_handler( CATALOG_RESPONSE, BESCatalogResponseHandler::CatalogResponseBuilder ) ;

    BESTransmitter *t = BESReturnManager::TheManager()->find_transmitter( BASIC_TRANSMITTER ) ;
    if( t )
    {
	if( BESLog::TheLog()->is_verbose() )
	    (*BESLog::TheLog()) << "    adding basic " << DAS_TRANSMITTER << " transmit function" << endl ;
	t->add_method( DAS_TRANSMITTER, BESDapTransmit::send_basic_das ) ;

	if( BESLog::TheLog()->is_verbose() )
	    (*BESLog::TheLog()) << "    adding basic " << DDS_TRANSMITTER << " transmit function" << endl ;
	t->add_method( DDS_TRANSMITTER, BESDapTransmit::send_basic_dds ) ;

	if( BESLog::TheLog()->is_verbose() )
	    (*BESLog::TheLog()) << "    adding basic " << DDX_TRANSMITTER << " transmit function" << endl ;
	t->add_method( DDX_TRANSMITTER, BESDapTransmit::send_basic_ddx ) ;

	if( BESLog::TheLog()->is_verbose() )
	    (*BESLog::TheLog()) << "    adding basic " << DATA_TRANSMITTER << " transmit function" << endl ;
	t->add_method( DATA_TRANSMITTER, BESDapTransmit::send_basic_data ) ;
    }

    t = BESReturnManager::TheManager()->find_transmitter( HTTP_TRANSMITTER ) ;
    if( t )
    {
	if( BESLog::TheLog()->is_verbose() )
	    (*BESLog::TheLog()) << "    adding http " << DAS_TRANSMITTER << " transmit function" << endl ;
	t->add_method( DAS_TRANSMITTER, BESDapTransmit::send_http_das ) ;

	if( BESLog::TheLog()->is_verbose() )
	    (*BESLog::TheLog()) << "    adding http " << DDS_TRANSMITTER << " transmit function" << endl ;
	t->add_method( DDS_TRANSMITTER, BESDapTransmit::send_http_dds ) ;

	if( BESLog::TheLog()->is_verbose() )
	    (*BESLog::TheLog()) << "    adding http " << DDX_TRANSMITTER << " transmit function" << endl ;
	t->add_method( DDX_TRANSMITTER, BESDapTransmit::send_http_ddx ) ;

	if( BESLog::TheLog()->is_verbose() )
	    (*BESLog::TheLog()) << "    adding http " << DATA_TRANSMITTER << " transmit function" << endl ;
	t->add_method( DATA_TRANSMITTER, BESDapTransmit::send_http_data ) ;
    }
}

void
BESDapModule::terminate( const string &modname )
{
    if( BESLog::TheLog()->is_verbose() )
	(*BESLog::TheLog()) << "Removing OPeNDAP modules" << endl;

    BESResponseHandlerList::TheList()->remove_handler( DAS_RESPONSE ) ;
    BESResponseHandlerList::TheList()->remove_handler( DDS_RESPONSE ) ;
    BESResponseHandlerList::TheList()->remove_handler( DDX_RESPONSE ) ;
    BESResponseHandlerList::TheList()->remove_handler( DATA_RESPONSE ) ;
    BESResponseHandlerList::TheList()->remove_handler( CATALOG_RESPONSE ) ;
}

extern "C"
{
    BESAbstractModule *maker()
    {
	return new BESDapModule ;
    }
}

