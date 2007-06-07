// BESGlobalInit.h

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

#ifndef I_BESGlobalInit_h
#define I_BESGlobalInit_h 1

#include "BESInitializer.h"
#include "BESInitFuns.h"

/** @brief Provides for the orderly initialization and termination of global objects.
 *
 * BESGlobalInit is an implementation of the abstration BESInitializer
 * that provides the orderly initialization and termination of global
 * objects. C++ does not provide such a mechanism, as global objects are
 * created in random order. This gives the user more control over that
 * ordering.
 *
 * For a complete understanding of this global initialization mechanism
 * please see the BESGlobalIQ documentation.
 *
 * @see BESGlobalIQ
 */
class BESGlobalInit : public BESInitializer
{
private:
    BESInitFun                 _initFun;
    BESTermFun                 _termFun;
    BESInitializer *           _nextInit;
public:
                                BESGlobalInit(BESInitFun, BESTermFun,
					       BESInitializer *nextInit,
					       int lvl);
    virtual                     ~BESGlobalInit(void);
    virtual bool           	initialize(int argc, char **argv);
    virtual bool           	terminate(void);

    virtual void		dump( ostream &strm ) const ;
};

#endif // I_BESGlobalInit_h

