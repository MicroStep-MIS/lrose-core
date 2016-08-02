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
//
// Ltg2RampClosures: Process ltg data in airport regions and convert to ramp 
// closure information
//
///////////////////////////////////////////////////////////////////////

#ifndef LTG2RAMPCLOSURES_H
#define LTG2RAMPCLOSURES_H

#include <string>
#include <math.h>
#include <Mdv/MdvxPjg.hh>
#include <Spdb/DsSpdb.hh>
#include <rapformats/ltg.h>
#include <Spdb/DsSpdb.hh>
#include <toolsa/pmu.h>
#include <toolsa/DateTime.hh>
#include <dsdata/DsLdataTrigger.hh>
#include <dsdata/DsTimeListTrigger.hh>
#include <dsdata/DsTrigger.hh>
#include <dsdata/TriggerInfo.hh>
#include <euclid/geometry.h>
#include "Args.hh"
#include "Params.hh"
#include "RampRegion.hh"

using namespace std;

////////////////////////
// This class

class Ltg2RampClosures {
  
public:

  // constructor

  Ltg2RampClosures (int argc, char **argv);

  // destructor
  
  ~Ltg2RampClosures();

  // run 

  int Run();

  // data members

  bool isOK;

protected:
  
private:
  
  string _progName;
  char *_paramsPath;
  Args _args;
  Params _params;
  vector <RampRegion*> _rampRegions;


  void _clear();
  void _initRampRegions();
  int _processData(time_t start_time, time_t end_time); 
  int _readSpdb(time_t start_time, time_t end_time);
  int _writeTimeSeries();
};

#endif

