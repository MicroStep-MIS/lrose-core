// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
// ** Copyright UCAR (c) 1990 - 2016                                         
// ** University Corporation for Atmospheric Research (UCAR)                 
// ** National Center for Atmospheric Research (NCAR)                        
// ** Boulder, Colorado, USA                                                 
// ** BSD licence applies - redistribution and use in source and binary      
// ** forms, with or without modification, are permitted provided that       
// ** the following conditions are met:                                      
// ** 1) If the software is modified to produce derivative works,            
// ** such modified software should be clearly marked, so as not             
// ** to confuse it with the version available from UCAR.                    
// ** 2) Redistributions of source code must retain the above copyright      
// ** notice, this list of conditions and the following disclaimer.          
// ** 3) Redistributions in binary form must reproduce the above copyright   
// ** notice, this list of conditions and the following disclaimer in the    
// ** documentation and/or other materials provided with the distribution.   
// ** 4) Neither the name of UCAR nor the names of its contributors,         
// ** if any, may be used to endorse or promote products derived from        
// ** this software without specific prior written permission.               
// ** DISCLAIMER: THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS  
// ** OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED      
// ** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.    
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* 
///////////////////////////////////////////////////////////////
// BasinList.cc
//
// Class representing a group of watershed basins.
//
// Nancy Rehak, RAP, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// July 2000
//
///////////////////////////////////////////////////////////////

#include <iostream>

#include <hydro/Basin.hh>
#include <hydro/BasinList.hh>
#include <shapelib/shapefil.h>
using namespace std;

/*********************************************************************
 * Constructors
 */

BasinList::BasinList(const bool debug_flag) :
  _debugFlag(debug_flag)
{
  _basinListIter = (vector< Basin* >::iterator) 0;
}


/*********************************************************************
 * Destructor
 */

BasinList::~BasinList()
{
  // Reclaim the basins in the list

  vector< Basin* >::iterator basin_iter;
  
  for (basin_iter = _basinList.begin();
       basin_iter != _basinList.end();
       ++basin_iter)
    delete *basin_iter;
  
}


/*********************************************************************
 * addFromShapeFile() - Add all of the basins in the given shape file
 *                      to the basin list.  shape_file_base is the path
 *                      of the shape files not including the extensions.
 *
 * Returns true if successful, false otherwise.
 */

bool BasinList::addFromShapeFile(const string shape_file_base)
{
  const string routine_name = "addFromShapeFile()";
  
  bool return_code = true;
  
  if (_debugFlag)
    cerr << "*** Reading shapes from " << shape_file_base << endl;

  // Open the shape file

  SHPHandle shape_handle;
  string shape_file_name = shape_file_base + ".shp";
  
  if ((shape_handle = SHPOpen(shape_file_name.c_str(), "rb")) == 0)
  {
    cerr << "ERROR: " << _className() << "::" << routine_name << endl;
    cerr << "Error opening shape file: " << shape_file_name << endl;
    
    return false;
  }
  
  // Open the database file

  DBFHandle dbf_handle;
  string dbf_file_name = shape_file_base + ".dbf";
  
  if ((dbf_handle = DBFOpen(dbf_file_name.c_str(), "rb")) == 0)
  {
    cerr << "ERROR: " << _className() << "::" << routine_name << endl;
    cerr << "Error opening database file: " << dbf_file_name << endl;
    
    SHPClose(shape_handle);
    
    return false;
  }
  
  // Retrieve the shape information from the file

  int n_shapes;
  int n_shape_types;
  double file_min_bounds[4];
  double file_max_bounds[4];
  
  SHPGetInfo(shape_handle, &n_shapes, &n_shape_types,
	     file_min_bounds, file_max_bounds);

  if (_debugFlag)
    cerr << "   Shape file contains " << n_shapes << " objects" << endl;
  
  // Process each shape in the file

  string id_field_name = Basin::getIdFieldFromShapeBase(shape_file_base);
  
  for (int i = 0; i < n_shapes; ++i)
  {
    Basin *basin = new Basin(_debugFlag);
    
    if (!basin->loadShapeInfo(shape_handle, dbf_handle, i, id_field_name))
    {
      cerr << "ERROR: " << _className() << "::" << routine_name << endl;
      cerr << "Error loading shape " << i << " from shape file " <<
	shape_file_base << endl;
      cerr << "--- Skipping basin ---" << endl;
      
      delete basin;
      return_code = false;

      continue;
    }
    
    _basinList.push_back(basin);
    
  } /* endfor - i */

  // Close the shape files since we're done with them.

  SHPClose(shape_handle);
  DBFClose(dbf_handle);
  
  return return_code;
}


/*********************************************************************
 * getFirstBasin() - Gets the first basin in the basin list.
 *
 * If successful, returns a pointer the the first basin in the list.
 * This pointer points directly to the Basin object in the list so
 * any changes to this object will change the object in the list.
 * Also, this pointer should not be deleted.
 * If there are no basins in the list, returns 0.
 */

Basin *BasinList::getFirstBasin(void)
{
  _basinListIter = _basinList.begin();
  
  if (_basinListIter == _basinList.end())
    return 0;
  
  return *_basinListIter;
}


/*********************************************************************
 * getNextBasin() - Gets the next basin in the basin list.  Note that
 *                  getFirstBasin() MUST be called before this method
 *                  can be called.
 *
 * If successful, returns a pointer the the next basin in the list.
 * This pointer points directly to the Basin object in the list so
 * any changes to this object will change the object in the list.
 * Also, this pointer should not be deleted.
 * If there are no more basins in the list, returns 0.
 */

Basin *BasinList::getNextBasin(void)
{
  if (_basinListIter == (vector< Basin* >::iterator) 0 ||
      _basinListIter == _basinList.end())
    return 0;
  
  ++_basinListIter;
  
  if (_basinListIter == _basinList.end())
    return 0;
  
  return *_basinListIter;
}


/*********************************************************************
 * print() - Print the current basin list information to the given stream
 *           for debugging purposes.
 */

void BasinList::print(ostream &stream) const
{
  stream << "BasinList information:" << endl;
  stream << "======================" << endl;
  stream << endl;
  stream << "basin list contains " << _basinList.size() << " basins." << endl;
  stream << endl;
  
  vector< Basin* >::const_iterator basin_iter;
  int basin_num;
  
  for (basin_iter = _basinList.begin(), basin_num = 0;
       basin_iter != _basinList.end();
       ++basin_iter, ++basin_num)
  {
    stream << "Basin #" << basin_num << ":" << endl;
    (*basin_iter)->print(stream, false);
    stream << endl;
  } /* endfor - basin_iter */
  
}


/**************************************************************
 * PRIVATE MEMBER FUNCTIONS
 **************************************************************/
