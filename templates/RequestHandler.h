// OPENDAP_CLASSRequestHandler.h

#ifndef I_OPENDAP_CLASSRequestHandler_H
#define I_OPENDAP_CLASSRequestHandler_H

#include "BESRequestHandler.h"

class OPENDAP_CLASSRequestHandler : public BESRequestHandler {
public:
			OPENDAP_CLASSRequestHandler( const string &name ) ;
    virtual		~OPENDAP_CLASSRequestHandler( void ) ;

    virtual void	dump( ostream &strm ) const ;

    static bool		OPENDAP_TYPE_build_vers( BESDataHandlerInterface &dhi ) ;
    static bool		OPENDAP_TYPE_build_help( BESDataHandlerInterface &dhi ) ;
};

#endif // OPENDAP_CLASSRequestHandler.h

