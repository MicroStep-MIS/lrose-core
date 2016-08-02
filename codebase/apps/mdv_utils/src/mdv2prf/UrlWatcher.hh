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

#ifndef _URL_WATCHER_INC_
#define _URL_WATCHER_INC_


#include <toolsa/pmu.h>
#include <Mdv/DsMdvxTimes.hh>

#include "Params.hh"
using namespace std;

class UrlWatcher {

public:

  //
  // Constructor
  //
  UrlWatcher(int argc,char *argv[]);

  
  //
  // Initialisation.
  //
  int init( int argc, char **argv );


  //
  // Main method - run.
  //
  int run( );

  //
  // Destructor
  //
  ~UrlWatcher();


  time_t next();

  private :

  //
  // Table Driven Runtime Parameters.
  //
  Params P;

  //
  // Methods.
  //

  int ParseArgs(int argc, char *argv[]);

  time_t _startTime;
  time_t _endTime;

  time_t _lastTriggerTime;
  vector<time_t> _lastUrlTimes;
  vector<time_t> _currentUrlTimes;
  vector<DsMdvxTimes *> _mdvxTimes;
  
  bool _hasUpdated(int UrlNum);
  void _initTimeArrays();
  void _updateLatestToCurrent();
  void _updateCurrent();


};




#endif
