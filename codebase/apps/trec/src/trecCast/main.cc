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

#include <signal.h>
#include <toolsa/port.h>
#include <toolsa/pmu.h>

#include "FileLister.hh"
#include "Params.hh"

void dieGracefully( int signal );
void usage();

main( int argc, char **argv )
{
   int status;

   Params *P;

   //
   // Grab TDRP parameters.
   //
   P = new Params();

   if (P->loadFromArgs(argc,argv,NULL,NULL)){
     fprintf(stderr,"Specify params file with -params option.\n");
     exit(-1);
   } 

   //
   // Trap signals for a clean exit
   //
   PORTsignal( SIGINT,  dieGracefully );
   PORTsignal( SIGTERM, dieGracefully );
   PORTsignal( SIGQUIT, dieGracefully );
   PORTsignal( SIGKILL, dieGracefully );

   //
   // Instantiate and initialize the top-level application class
   //
   FileLister *F = new FileLister();
   status = F->init( argc, argv, P );
   if ( status != 0 ) {
      dieGracefully( status );
   }

   //
   // Fire off the application -- that's it
   //
   status = F->run( P );

   dieGracefully( status );

}

////////////////////////////////////////////////////

void dieGracefully( int signal )
{
   PMU_auto_unregister();
   exit( signal );
}


//////////////////////////////////////////////////




