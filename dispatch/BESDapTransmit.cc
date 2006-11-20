// BESDapTransmit.cc

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

#include <sstream>

using std::ostringstream ;

#include "BESDapTransmit.h"
#include "DODSFilter.h"
#include "BESContainer.h"
#include "BESDataNames.h"
#include "cgi_util.h"
#include "BESDASResponse.h"
#include "BESDDSResponse.h"
#include "BESDataDDSResponse.h"
#include "BESTransmitException.h"
#include "Error.h"

void
BESDapTransmit::send_basic_das( BESResponseObject *obj,
                                BESDataHandlerInterface &dhi )
{
    BESDASResponse *bdas = dynamic_cast<BESDASResponse *>(obj) ;
    DAS *das = bdas->get_das() ;
    dhi.first_container();

    try
    {
	DODSFilter df ;
	df.set_dataset_name( dhi.container->get_real_name() ) ;
	df.send_das( stdout, *das, "", false ) ;
	fflush( stdout ) ;
    }
    catch( Error &e )
    {
	ostringstream s ;
	s << "libdap exception transmitting DAS"
	  << ": error_code = " << e.get_error_code()
	  << ": " << e.get_error_message() ;
	BESTransmitException ex( s.str(), __FILE__, __LINE__ ) ;
	throw ex ;
    }
    catch( ... )
    {
	string s = "unknown exception caught transmitting DAS" ;
	BESTransmitException ex( s, __FILE__, __LINE__ ) ;
	throw ex ;
    }
}

void
BESDapTransmit::send_http_das( BESResponseObject *obj,
                               BESDataHandlerInterface &dhi )
{
    set_mime_text( stdout, dods_das ) ;
    BESDapTransmit::send_basic_das( obj, dhi ) ;
}

void
BESDapTransmit::send_basic_dds( BESResponseObject *obj,
				BESDataHandlerInterface &dhi )
{
    BESDDSResponse *bdds = dynamic_cast<BESDDSResponse *>(obj) ;
    DDS *dds = bdds->get_dds() ;
    ConstraintEvaluator &ce = bdds->get_ce() ;
    dhi.first_container();

    try
    {
	DODSFilter df ;
	df.set_dataset_name( dhi.container->get_real_name() ) ;
	df.set_ce( dhi.data[POST_CONSTRAINT] ) ;
	df.send_dds( stdout, *dds, ce, true, "", false ) ;
	fflush( stdout ) ;
    }
    catch( Error &e )
    {
	ostringstream s ;
	s << "libdap exception transmitting DDS"
	  << ": error_code = " << e.get_error_code()
	  << ": " << e.get_error_message() ;
	BESTransmitException ex( s.str(), __FILE__, __LINE__ ) ;
	throw ex ;
    }
    catch( ... )
    {
	string s = "unknown exception caught transmitting DDS" ;
	BESTransmitException ex( s, __FILE__, __LINE__ ) ;
	throw ex ;
    }
}

void
BESDapTransmit::send_http_dds( BESResponseObject *obj,
			       BESDataHandlerInterface &dhi )
{
    set_mime_text( stdout, dods_dds ) ;
    BESDapTransmit::send_basic_dds( obj, dhi ) ;
}

void
BESDapTransmit::send_basic_data( BESResponseObject *obj,
				 BESDataHandlerInterface &dhi )
{
    BESDataDDSResponse *bdds = dynamic_cast<BESDataDDSResponse *>(obj) ;
    DataDDS *dds = bdds->get_dds() ;
    ConstraintEvaluator &ce = bdds->get_ce() ;
    dhi.first_container() ;

    try
    {
	DODSFilter df ;
	df.set_dataset_name( dds->filename() ) ;
	df.set_ce(dhi.data[POST_CONSTRAINT]);
	df.send_data( *dds, ce, stdout, "", false ) ;
	fflush( stdout ) ;
    }
    catch( Error &e )
    {
	ostringstream s ;
	s << "libdap exception transmitting DataDDS"
	  << ": error_code = " << e.get_error_code()
	  << ": " << e.get_error_message() ;
	BESTransmitException ex( s.str(), __FILE__, __LINE__ ) ;
	throw ex ;
    }
    catch( ... )
    {
	string s = "unknown exception caught transmitting DataDDS" ;
	BESTransmitException ex( s, __FILE__, __LINE__ ) ;
	throw ex ;
    }
}

void
BESDapTransmit::send_http_data( BESResponseObject *obj,
				BESDataHandlerInterface &dhi )
{
    //set_mime_binary( stdout, dods_data ) ;
    BESDapTransmit::send_basic_data( obj, dhi ) ;
}

void
BESDapTransmit::send_basic_ddx( BESResponseObject *obj,
				BESDataHandlerInterface &dhi )
{
    BESDDSResponse *bdds = dynamic_cast<BESDDSResponse *>(obj) ;
    DDS *dds = bdds->get_dds() ;
    ConstraintEvaluator &ce = bdds->get_ce() ;
    dhi.first_container() ;

    DODSFilter df ;
    df.set_dataset_name( dhi.container->get_real_name() ) ;
    df.set_ce( dhi.data[POST_CONSTRAINT] ) ;
    df.send_ddx( *dds, ce, stdout, false ) ;

    fflush( stdout ) ;
}

void
BESDapTransmit::send_http_ddx( BESResponseObject *obj,
			       BESDataHandlerInterface &dhi )
{
    set_mime_text( stdout, dods_dds ) ;
    BESDapTransmit::send_basic_ddx( obj, dhi ) ;
}
