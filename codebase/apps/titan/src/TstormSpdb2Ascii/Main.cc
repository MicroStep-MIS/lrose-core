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
// main for TstormSpdb2Ascii
//
// Dan Megenhardt, RAP, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// August 2005
//
///////////////////////////////////////////////////////////////
//
// update_storm_vectors changes the vectors associated with the
// storms in a given SPDB database using either storm vector
// information from another SPDB database (presumably created
// using different source data or different TITAN parameters)
// or from an MDV file containing motion vectors created by
// TREC.
//
///////////////////////////////////////////////////////////////

#include <stdio.h>
#include <signal.h>

#include <toolsa/os_config.h>
#include <toolsa/port.h>

#include "TstormSpdb2Ascii.hh"

//
// Prototypes for static functions.
//

static void tidy_and_exit (int sig);

//
// Global variables
//

TstormSpdb2Ascii *Prog = (TstormSpdb2Ascii *)NULL;


/*****************************************************************
 * main() - main program.
 */

int main(int argc, char **argv)
{
  // create program object

  Prog = TstormSpdb2Ascii::Inst(argc, argv);

  if (!Prog->okay)
    return(-1);

  // set signal handling
  
  PORTsignal(SIGINT, tidy_and_exit);
  PORTsignal(SIGHUP, tidy_and_exit);
  PORTsignal(SIGTERM, tidy_and_exit);
  PORTsignal(SIGPIPE, (PORTsigfunc)SIG_IGN);

  // run it

  Prog->run();

  // clean up

  tidy_and_exit(0);
  return (0);
}


/*****************************************************************
 * tidy_and_exit() - Clean up memory and exit from the program.
 */

static void tidy_and_exit(int sig)
{
  // Delete the program object.

  if (Prog != (TstormSpdb2Ascii *)NULL)
    delete Prog;

  // Now exit the program.

  exit(sig);
}

