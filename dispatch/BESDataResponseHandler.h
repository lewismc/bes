// BESDataResponseHandler.h

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// Copyright (c) 2004,2005 University Corporation for Atmospheric Research
// Author: Patrick West <pwest@ucar.edu> and Jose Garcia <jgarcia@ucar.edu>
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
 
// (c) COPYRIGHT University Corporation for Atmospheric Research 2004-2005
// Please read the full copyright statement in the file COPYRIGHT_UCAR.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>
//      jgarcia     Jose Garcia <jgarcia@ucar.edu>

#ifndef I_BESDataResponseHandler_H
#define I_BESDataResponseHandler_H

#include "BESResponseHandler.h"

/** @brief response handler that builds an OPeNDAP Data DDS object
 *
 * A request 'get data for &lt;def_name&gt;' will be handled by this 
 * response handler. Given a definition name it determines what containers 
 * are to be used to build the response object. It then transmits the 
 * response object using the method send_data on the transmitter object.
 *
 * @see DDS
 * @see BESContainer
 * @see BESTransmitter
 * @see BESDefine
 */
class BESDataResponseHandler : public BESResponseHandler
{
public:
				BESDataResponseHandler( const string &name ) ;
    virtual			~BESDataResponseHandler(void) ;

    virtual void		execute( BESDataHandlerInterface &dhi ) ;
    virtual void		transmit( BESTransmitter *transmitter,
                                          BESDataHandlerInterface &dhi ) ;

    virtual void		dump( ostream &strm ) const ;

    static BESResponseHandler *DataResponseBuilder( const string &name ) ;
};

#endif

