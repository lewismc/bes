// GatewayModule.cc

#include <iostream>
#include <vector>
#include <string>

using std::endl;
using std::vector;
using std::string;

#include "GatewayModule.h"

#include <BESRequestHandlerList.h>
#include <BESDebug.h>
#include <BESResponseHandlerList.h>
#include <BESResponseNames.h>
#include <BESContainerStorageList.h>
#include <TheBESKeys.h>
#include <BESSyntaxUserError.h>

#include "GatewayRequestHandler.h"
#include "GatewayResponseNames.h"
#include "GatewayContainerStorage.h"
#include "GatewayUtils.h"
#include "GatewayPathInfoResponseHandler.h"
#include "GatewayPathInfoCommand.h"

void GatewayModule::initialize(const string &modname)
{
    BESDEBUG(modname, "Initializing Gateway Module " << modname << endl);

    BESDEBUG(modname, "    adding " << modname << " request handler" << endl);
    BESRequestHandlerList::TheList()->add_handler(modname, new GatewayRequestHandler(modname));

    BESDEBUG(modname, "    adding " << modname << " container storage" << endl);
    BESContainerStorageList::TheList()->add_persistence(new GatewayContainerStorage(modname));

    BESDEBUG(modname, "    initialize the gateway utilities and params" << endl);
    GatewayUtils::Initialize();

    BESDEBUG(modname, "    adding Gateway debug context" << endl);
    BESDebug::Register(modname);

    BESDEBUG( modname, "    adding " << SHOW_GATEWAY_PATH_INFO_RESPONSE_STR << " command" << endl ) ;
    BESXMLCommand::add_command( SHOW_GATEWAY_PATH_INFO_RESPONSE_STR, GatewayPathInfoCommand::CommandBuilder ) ;

    BESDEBUG(modname, "    adding " << SHOW_GATEWAY_PATH_INFO_RESPONSE << " response handler" << endl ) ;
    BESResponseHandlerList::TheList()->add_handler( SHOW_GATEWAY_PATH_INFO_RESPONSE, GatewayPathInfoResponseHandler::GatewayPathInfoResponseBuilder ) ;

    BESDEBUG(modname, "Done Initializing Gateway Module " << modname << endl);
}

void GatewayModule::terminate(const string &modname)
{
    BESDEBUG(modname, "Cleaning Gateway module " << modname << endl);

    BESResponseHandlerList::TheList()->remove_handler( SHOW_GATEWAY_PATH_INFO_RESPONSE) ;
    BESXMLCommand::del_command( SHOW_GATEWAY_PATH_INFO_RESPONSE_STR) ;


    BESDEBUG(modname, "    removing " << modname << " request handler" << endl);
    BESRequestHandler *rh = BESRequestHandlerList::TheList()->remove_handler(modname);
    if (rh)
        delete rh;

    BESContainerStorageList::TheList()->deref_persistence(modname);

    // TERM_END
    BESDEBUG(modname, "Done Cleaning Gateway module " << modname << endl);
}

extern "C" {
BESAbstractModule *maker()
{
    return new GatewayModule;
}
}

void GatewayModule::dump(ostream &strm) const
{
    strm << BESIndent::LMarg << "GatewayModule::dump - (" << (void *) this << ")" << endl;
}

