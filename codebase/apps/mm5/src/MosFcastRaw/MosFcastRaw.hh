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
#include <Spdb/DsSpdb.hh>

#include "Params.hh"
#include "ApplyRegression.hh"
using namespace std;

class MosFcastRaw {

public:

  //
  // Constructor
  //
  MosFcastRaw();

  
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
  ~MosFcastRaw();

   //
   // Constants
   //
   static const int HOURS_TO_SEC;
   static const int DAYS_TO_SEC;

  private:

  //
  // Start and end times - set to 0 unless in archive mode.
  //
  time_t startTime;
  time_t endTime;

  //
  // Table Driven Runtime Parameters.
  //
  Params P;
  tdrp_override_t override;

  //
  // ApplyRegression object
  //
  ApplyRegression applyRegression;
   
  //
  // Methods.
  //

  int ParseArgs(int argc, char *argv[]);
  void usage(ostream &out);
  int _runArchive();
  int _runRealtime();
  int _processChunk(const Spdb::chunk_t &chunk,
		    DsSpdb &OutSpdb);

};




#endif
