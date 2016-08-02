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
// Noaa Port Lightning 2 Spdb

#ifndef _URL_WATCHER_INC_
#define _URL_WATCHER_INC_


#include <toolsa/pmu.h>
#include <Spdb/DsSpdb.hh>

#include "Params.hh"

int ltg_ingest(char *in_file, Params *P);

class WDTLtg2Spdb {

public:

  //
  // Constructor
  //
  WDTLtg2Spdb();

  
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
  ~WDTLtg2Spdb();

  private :

  //
  // Table Driven Runtime Parameters.
  //
  Params P;

  //
  // The following used in ARCHIVE mode.
  //
  char **filePaths;
  int nFiles;
  //
  // Methods.
  //

  int ParseArgs(int argc, char *argv[]);

};




#endif
