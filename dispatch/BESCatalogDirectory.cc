// BESCatalogDirectory.cc

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

#include "sys/types.h"
#include "sys/stat.h"
#include "dirent.h"
#include "stdio.h"

#include <sstream>

using std::stringstream ;
using std::endl ;

#include "BESCatalogDirectory.h"
#include "TheBESKeys.h"
#include "BESInfo.h"
#include "BESResponseException.h"
#include "BESResponseNames.h"
#include "GNURegex.h"

BESCatalogDirectory::BESCatalogDirectory( const string &name )
{
    bool found = false ;
    string key = (string)"Catalog." + name + ".RootDirectory" ;
    _rootDir = TheBESKeys::TheKeys()->get_key( key, found ) ;
    if( !found || _rootDir == "" )
    {
	string serr = "BESCatalogDirectory - unable to load root directory key "
		      + key + " from initialization file" ;
	throw BESResponseException( serr, __FILE__, __LINE__ ) ;
    }

    DIR *dip = opendir( _rootDir.c_str() ) ;
    if( dip == NULL )
    {
	string serr = "BESCatalogDirectory - root directory "
	              + _rootDir + " does not exist" ;
	throw BESResponseException( serr, __FILE__, __LINE__ ) ;
    }
    closedir( dip ) ;

    key = (string)"Catalog." + name + ".Exclude" ;
    string e_str = TheBESKeys::TheKeys()->get_key( key, found ) ;
    if( found && e_str != "" && e_str != ";" )
    {
	buildList( _exclude, e_str ) ;
    }

    key = (string)"Catalog." + name + ".Include" ;
    string i_str = TheBESKeys::TheKeys()->get_key( key, found ) ;
    if( found && i_str != "" && i_str != ";" )
    {
	buildList( _include, i_str ) ;
    }
}

BESCatalogDirectory::~BESCatalogDirectory( )
{
}

bool
BESCatalogDirectory::show_catalog( const string &node,
                                const string &coi,
				BESInfo *info )
{
    string fullnode ;
    if( node == "" )
    {
	fullnode = _rootDir ;
    }
    else
    {
	fullnode = _rootDir + "/" + node ;
    }
    DIR *dip = opendir( fullnode.c_str() ) ;
    if( dip != NULL )
    {
	struct stat cbuf ;
	stat( fullnode.c_str(), &cbuf ) ;
	map<string,string> a1 ;
	a1["thredds_collection"] = "\"true\"" ;
	info->begin_tag( "dataset", &a1 ) ;
	if( node == "" )
	{
	    add_stat_info( info, cbuf, "/" ) ;
	}
	else
	{
	    add_stat_info( info, cbuf, node ) ;
	}

	struct dirent *dit;
	unsigned int cnt = 0 ;
	struct stat buf;
	while( ( dit = readdir( dip ) ) != NULL )
	{
	    string dirEntry = dit->d_name ;
	    if( dirEntry != "." && dirEntry != ".." )
	    {
		// look at the mode and determine if this is a directory
		string fullPath = fullnode + "/" + dirEntry ;
		stat( fullPath.c_str(), &buf ) ;
		if ( S_ISDIR( buf.st_mode ) )
		{
		    cnt++ ;
		}
		else if ( S_ISREG( buf.st_mode ) )
		{
		    if( include( dirEntry ) )
		    {
			cnt++ ;
		    }
		}
	    }
	}

	stringstream sscnt ;
	sscnt << cnt ;
	info->add_tag( "count", sscnt.str() ) ;

	if( coi == CATALOG_RESPONSE )
	{
	    rewinddir( dip ) ;

	    while( ( dit = readdir( dip ) ) != NULL )
	    {
		string dirEntry = dit->d_name ;
		if( dirEntry != "." && dirEntry != ".." )
		{
		    // look at the mode and determine if this is a directory
		    string fullPath = fullnode + "/" + dirEntry ;
		    stat( fullPath.c_str(), &buf ) ;
		    if ( S_ISDIR( buf.st_mode ) )
		    {
			map<string,string> a2 ;
			a2["thredds_collection"] = "\"true\"" ;
			info->begin_tag( "dataset", &a2 ) ;
			add_stat_info( info, buf, dirEntry ) ;
			info->end_tag( "dataset" ) ;
		    }
		    else if ( S_ISREG( buf.st_mode ) )
		    {
			if( include( dirEntry ) )
			{
			    map<string,string> a3 ;
			    a3["thredds_collection"] = "\"false\"" ;
			    info->begin_tag( "dataset", &a3 ) ;
			    add_stat_info( info, buf, dirEntry ) ;
			    info->end_tag( "dataset" ) ;
			}
		    }
		}
	    }
	}
	closedir( dip ) ;
	info->end_tag( "dataset" ) ;
    }
    else
    {
	struct stat buf;
	int statret = stat( fullnode.c_str(), &buf ) ;
	if ( statret == 0 && S_ISREG( buf.st_mode ) )
	{
	    map<string,string> a4 ;
	    a4["thredds_collection"] = "\"false\"" ;
	    info->begin_tag( "dataset", &a4 ) ;
	    add_stat_info( info, buf, node ) ;
	    info->end_tag( "dataset" ) ;
	}
	else
	{
	    return false ;
	}
    }

    return true ;
}

bool
BESCatalogDirectory::include( const string &inQuestion )
{
    bool toInclude = false ;

    // First check the file against the include list. If the file should be
    // included then check the exclude list to see if there are exceptions
    // to the include list.
    if( _include.size() == 0 )
    {
	toInclude = true ;
    }
    else
    {
	list<string>::iterator i_iter = _include.begin() ;
	list<string>::iterator i_end = _include.end() ;
	for( ; i_iter != i_end; i_iter++ )
	{
	    string reg = *i_iter ;
	    Regex reg_expr( reg.c_str() ) ;
	    if( reg_expr.match( inQuestion.c_str(), inQuestion.length() ) != -1)
	    {
		toInclude = true ;
	    }
	}
    }

    if( toInclude == true )
    {
	list<string>::iterator e_iter = _exclude.begin() ;
	list<string>::iterator e_end = _exclude.end() ;
	for( ; e_iter != e_end; e_iter++ )
	{
	    string reg = *e_iter ;
	    Regex reg_expr( reg.c_str() ) ;
	    if( reg_expr.match( inQuestion.c_str(), inQuestion.length() ) != -1)
	    {
		toInclude = false ;
	    }
	}
    }

    return toInclude ;
}

void
BESCatalogDirectory::buildList( list<string> &theList, const string &listStr )
{
    string::size_type str_begin = 0 ;
    string::size_type str_end = listStr.length() ;
    string::size_type semi = 0 ;
    bool done = false ;
    while( done == false )
    {
	semi = listStr.find( ";", str_begin ) ;
	if( semi == string::npos )
	{
	    string s = (string)"Catalog type match malformed, no semicolon, "
		       "looking for type:regexp;[type:regexp;]" ;
	    throw BESResponseException( s, __FILE__, __LINE__ ) ;
	}
	else
	{
	    string a_member = listStr.substr( str_begin, semi-str_begin ) ;
	    str_begin = semi+1 ;
	    if( semi == str_end-1 )
	    {
		done = true ;
	    }
	    if( a_member != "" ) theList.push_back( a_member ) ;
	}
    }
}

void
BESCatalogDirectory::add_stat_info( BESInfo *info,
				    struct stat &buf,
				    const string &node )
{
    info->add_tag( "name", node ) ;

    off_t sz = buf.st_size ;
    stringstream ssz ;
    ssz << sz ;
    info->add_tag( "size", ssz.str() ) ;

    // %T = %H:%M:%S
    // %F = %Y-%m-%d
    time_t mod = buf.st_mtime ;
    struct tm *stm = gmtime( &mod ) ;
    char mdate[64] ;
    strftime( mdate, 64, "%F", stm ) ;
    char mtime[64] ;
    strftime( mtime, 64, "%T", stm ) ;

    info->begin_tag( "lastmodified" ) ;

    stringstream sdt ;
    sdt << mdate ;
    info->add_tag( "date", sdt.str() ) ;

    stringstream stt ;
    stt << mtime ;
    info->add_tag( "time", stt.str() ) ;

    info->end_tag( "lastmodified" ) ;
}

