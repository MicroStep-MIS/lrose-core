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
//////////////////////////////////////////////////////////////////
// main entry point for application
//
// Jaimi Yee, RAP, NCAR, Boulder, CO, USA, 80307-3000
// July 2004
//
// Adapted from nexrad2dsr application by Terri Betancourt, 
// RAP, NCAR, Boulder, CO, USA, 80307-3000
//
// $Id: Main.cc,v 1.4 2016/03/07 01:23:03 dixon Exp $
//
/////////////////////////////////////////////////////////////////

#define _NEXRAD2NETCDF_MAIN_

#include <toolsa/port.h>
#include <toolsa/umisc.h>
#include <toolsa/pmu.h>

#include "Nexrad2Netcdf.hh"
#include "Args.hh"

using namespace std;

void dieGracefully( int sig );

Nexrad2Netcdf *nexrad2Netcdf;

//
// Global msgLog object
//
MsgLog msgLog;

int main( int argc, char **argv )
{
  int status;
  
  //        
  // Trap signals for a clean exit
  //                                                                        
  PORTsignal( SIGHUP,  dieGracefully );
  PORTsignal( SIGINT,  dieGracefully );                    
  PORTsignal( SIGTERM, dieGracefully );
  PORTsignal( SIGQUIT, dieGracefully );      
  PORTsignal( SIGKILL, dieGracefully );      
  PORTsignal( SIGPIPE, (PORTsigfunc)SIG_IGN );

  Path program;
  program.setPath( argv[0] );

  // Get the CLI arguments and load params
  Args args;
  Params *P = new  Params(); 
  args.parse(argc, argv, program.getFile(), P);
    
  //                                                        
  // Instantiate and initialize the top-level application class
  //                              
  nexrad2Netcdf = new Nexrad2Netcdf(P, program.getFile().c_str());
  
  //
  // Startup the application -- the end.
  //
  status = nexrad2Netcdf->run(args.inputFileList, args.startTime, args.endTime);
  dieGracefully( status );
}

void dieGracefully( int sig )
{
   POSTMSG("Exiting application with signal %d", sig );

   //
   // Remove the top-level application class                                   
   // 
   delete nexrad2Netcdf;

   //
   // Unregister with process mapper
   //
   PMU_auto_unregister();
   exit( sig );
}
