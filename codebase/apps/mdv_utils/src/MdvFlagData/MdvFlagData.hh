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
////////////////////////////////////////////////////////////
// MdvFlagData.hh
//
// Mdv2NetCDF object
//
// Sue Dettling, RAP, NCAR
// P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// December 2008
//
///////////////////////////////////////////////////////////////
//
// MdvFlagData modifies field data based on a flag field  .
//
///////////////////////////////////////////////////////////////////////

#ifndef MDVFLAGDATA_H
#define MDVFLAGDATA_H

#include <string>
#include <math.h>
#include <Mdv/DsMdvx.hh>
#include <Mdv/MdvxField.hh>
#include <didss/DsInputPath.hh>
#include <toolsa/pmu.h>
#include <toolsa/DateTime.hh>
#include <dsdata/DsLdataTrigger.hh>
#include <dsdata/DsFileListTrigger.hh>
#include <dsdata/DsTrigger.hh>
#include <dsdata/TriggerInfo.hh>
#include "Args.hh"
#include "Params.hh"

using namespace std;

////////////////////////
// This class
//

class MdvFlagData {
  
public:

  //
  // constructor
  //
  MdvFlagData (int argc, char **argv);

  //
  // destructor
  //
  ~MdvFlagData();
  
  int Run();

  //
  // public data member
  //
  bool isOK;

protected:
   
private:

  // data members

  string _progName;
  
  char *_paramsPath;
  
  Args _args;

  Params _params;

  DsTrigger *_trigger;

  DsMdvx _mdvx;

  Mdvx::master_header_t _masterHdr;

  Mdvx::field_header_t _fHdr;

  Mdvx::field_header_t _flagsFhdr;
  
  Mdvx::vlevel_header_t _vHdr;

  Mdvx::vlevel_header_t _flagsVhdr;

  int _nz;

  int _nx;

  int _ny;

  fl32 *_fieldData;

  fl32 *_flagsFieldData;

  fl32 _fieldMissing;

  fl32 _flagsMissing;

  //
  // methods
  //
  
 
  int _setUpTrigger();

  int _processData(const time_t inputTime, const int leadTime, const string filepath); 

  int _readMdv( const time_t requestTime, const int leadTime, const string filepath);
  
  int _applyFlagMods();

  int _writeMdv();
};

#endif

