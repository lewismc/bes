// BESInfo.h

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
 
// (c) COPYRIGHT University Corporation for Atmostpheric Research 2004-2005
// Please read the full copyright statement in the file COPYRIGHT_UCAR.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>
//      jgarcia     Jose Garcia <jgarcia@ucar.edu>

#ifndef BESInfo_h_
#define BESInfo_h_ 1

#include <string>
#include <ostream>
#include <stack>
#include <map>

using std::string ;
using std::ostream ;
using std::stack ;
using std::map ;

#include "DODSResponseObject.h"
#include "BESDataHandlerInterface.h"
#include "BESTransmitter.h"
#include "BESException.h"

/** @brief informational response object
 *
 * This class provides a means to store informational responses, such
 * as help information and version information. The retrieval of this
 * information can be buffered until all information is retrieved, or can be
 * directly output thereby not using memory resources.
 *
 * Information is added to this response object through the add_data method
 * and then output using the print method. If the information is not buffered
 * then the information is output during the add_data processing, otherwise
 * the print method performs the output.
 *
 * This class is can not be directly created but simply provides a base class
 * implementation of DODSResponseObject for simple informational responses.
 *
 * @see DODSResponseObject
 */
class BESInfo :public DODSResponseObject
{
protected:
    ostream		*_strm ;
    bool		_buffered ;
    bool		_response_started ;

    stack<string>	_tags ;
    string		_response_name ;

public:
    			BESInfo( ) ;
    			BESInfo( const string &buffered_key ) ;
    virtual		~BESInfo() ;

    virtual void	begin_response( const string &response_name ) ;
    virtual void	end_response( ) ;

    virtual void	add_tag( const string &tag_name,
                                 const string &tag_data,
				 map<string,string> *attrs = 0 ) = 0 ;
    virtual void	begin_tag( const string &tag_name,
                                   map<string,string> *attrs = 0 ) ;
    virtual void	end_tag( const string &tag_name ) ;

    virtual void 	add_data( const string &s ) ;
    virtual void	add_space( unsigned long num_spaces ) = 0 ;
    virtual void	add_break( unsigned long num_breaks ) = 0 ;

    virtual void 	add_data_from_file( const string &key,
                                            const string &name ) ;

    virtual void	add_exception( const string &type, BESException &e ) ;

    /** @brief transmit the informational object
     *
     * The derived informational object knows how it needs to be
     * transmitted. Does it need to be sent as html? As text? As something
     * else?
     *
     * @param transmitter The type of transmitter to use to transmit the info
     * @param dhi information to help with the transmission
     */
    virtual void	transmit( BESTransmitter *transmitter,
				  BESDataHandlerInterface &dhi ) = 0 ;

    virtual void 	print( FILE *out ) ;

    /** @brief return whether the information is to be buffered or not.
     *
     * @return true if information is buffered, false if not
     */
    virtual bool	set_buffered( bool buffered ) { _buffered = buffered ; }

    /** @brief return whether the information is to be buffered or not.
     *
     * @return true if information is buffered, false if not
     */
    virtual bool	is_buffered() { return _buffered ; }
};

#endif // BESInfo_h_
