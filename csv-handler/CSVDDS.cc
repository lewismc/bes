// CSVDDS.cc

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// Copyright (c) 2004,2005 University Corporation for Atmospheric Research
// Author: Stephan Zednik <zednik@ucar.edu> and Patrick West <pwest@ucar.edu>
// and Jose Garcia <jgarcia@ucar.edu>
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
//	zednik      Stephan Zednik <zednik@ucar.edu>
//      pwest       Patrick West <pwest@ucar.edu>
//      jgarcia     Jose Garcia <jgarcia@ucar.edu>

#include <vector>
#include <string>

#include "CSVDDS.h"
#include "BESHandlerException.h"
#include "BaseTypeFactory.h"
#include "DDS.h"
#include "Error.h"

#include "Str.h"
#include "Int16.h"
#include "Int32.h"
#include "Float32.h"
#include "Float64.h"

#include "Array.h"

#include "CSV_Obj.h"

void csv_read_descriptors(DDS &dds, const string &filename) {
  
  vector<string> fieldList;
  string type;
  int recordCount;
  int index;

  Array* ar;
  void* data;
  BaseType *bt = NULL;

  CSV_Obj* csvObj = new CSV_Obj();
  csvObj->open(filename);
  csvObj->load();

  dds.set_dataset_name(filename);

  fieldList = csvObj->getFieldList();
  recordCount = csvObj->getRecordCount();

  for(vector<string>::iterator it = fieldList.begin(); it != fieldList.end(); it++) {
    type = csvObj->getFieldType(*it);
    ar = dds.get_factory()->NewArray(string(*it));
    data = csvObj->getFieldData(*it);

    if(type.compare(string(STRING)) == 0) {
      string* strings = new string[recordCount];

      bt = dds.get_factory()->NewStr(string(*it));
      ar->add_var(bt);
      ar->append_dim(recordCount, "record");
      
      index = 0;
      for(vector<string>::iterator it = ((vector<string>*)data)->begin(); 
	  it != ((vector<string>*)data)->end(); it++) {
	strings[index] = *it;
	index++;
      }
      
      ar->set_value(strings, recordCount);
      
    } else if(type.compare(string(INT16)) == 0) {
      short* int16 = new short[recordCount];
      bt = dds.get_factory()->NewInt16(*it);
      ar->add_var(bt);
      ar->append_dim(recordCount, "record");

      index = 0;
      for(vector<short>::iterator it = ((vector<short>*)data)->begin();
	  it != ((vector<short>*)data)->end(); it++) {
	int16[index] = *it;
	index++;
      }

      ar->set_value(int16, recordCount);

    } else if(type.compare(string(INT32)) == 0) {
      int* int32 = new int[recordCount];
      bt = dds.get_factory()->NewInt32(*it);
      ar->add_var(bt);
      ar->append_dim(recordCount, "record");

      index = 0;
      for(vector<int>::iterator it = ((vector<int>*)data)->begin();
	  it != ((vector<int>*)data)->end(); it++) {
	int32[index] = *it;
	index++;
      }

      ar->set_value((dods_int32*)int32, recordCount); //blah!

    } else if(type.compare(string(FLOAT32)) == 0) {
      float* floats = new float[recordCount];
      bt = dds.get_factory()->NewFloat32(*it);
      ar->add_var(bt);
      ar->append_dim(recordCount, "record");

      index = 0;
      for(vector<float>::iterator it = ((vector<float>*)data)->begin(); 
	  it != ((vector<float>*)data)->end(); it++) {
	floats[index] = *it;
	index++;
      }

      ar->set_value(floats, recordCount);

    } else if(type.compare(string(FLOAT64)) == 0) {
      double* doubles = new double[recordCount];
      bt = dds.get_factory()->NewFloat64(*it);
      ar->add_var(bt);
      ar->append_dim(recordCount, "record");

      index = 0;
      for(vector<double>::iterator it = ((vector<double>*)data)->begin(); 
	  it != ((vector<double>*)data)->end(); it++) {
	doubles[index] = *it;
	index++;
      }

      ar->set_value(doubles, recordCount);
    } else {
	throw BESHandlerException( "Bad Things Man", __FILE__, __LINE__ ) ;
    }

    dds.add_var(ar);
    delete ar;
    delete bt;
  }

  delete csvObj;
}
