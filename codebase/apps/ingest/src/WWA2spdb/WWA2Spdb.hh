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
// WWA2Spdb

#ifndef _WWA2Spdb_H
#define _WWA2Spdb_H


#include <toolsa/pmu.h>
#include <rapformats/NWS_WWA.hh>
#include <Spdb/DsSpdb.hh>

#include "Params.hh"

class WWA2Spdb {

public:

  WWA2Spdb();
  //
  // Initialisation.
  int init( int argc, char **argv );

  // Main method - run.
  int run( );

  int process_file(char *in_file);

  int process_warn(char *start_ptr, int warn_len,DsSpdb &OutSpdb);

  //
  // Destructor
  //
  ~WWA2Spdb();

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
