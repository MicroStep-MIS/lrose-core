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
//////////////////////////////////////////////////////////
// Args.cc : command line args
//
// Niles Oien, RAP, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// April 1998
//
//////////////////////////////////////////////////////////
//
// This is the class that handles the command line arguments
// for the Sams2Spdb applications.
//
#include "Args.hh"
#include <string.h>
#include <toolsa/str.h>
#include <toolsa/umisc.h>
using namespace std;
//
// Constructor. Most of the parsing work is done here.
//
Args::Args (int argc, char **argv, char *prog_name){

  char tmp_str[BUFSIZ];

  //
  // intialize
  //
  OK = TRUE;
  nFiles = 0;
  TDRP_init_override(&override); 

  //
  // Loop through the command line arguments.
  //
  for (int i =  1; i < argc; i++) {

    //
    // The '-h' argument is requesting the help message.
    // Print it and exit.
    //
    if (!strcmp(argv[i], "--") ||
	!strcmp(argv[i], "-h") ||
	!strcmp(argv[i], "-help") ||
	!strcmp(argv[i], "-man")) {
      
      _usage(prog_name, stdout);
      exit(0);
      
    } else if (!strcmp(argv[i], "-params")) { // TDRP argument - record it.
	
      if (i < argc - 1) {
	paramsFilePath = argv[i+1];
	i=i+1; if (i>=argc) continue;
      } else {
	OK = FALSE;
      }
    } else if (!strcmp(argv[i], "-check_params")) { // TDRP argument - ignore it.
      OK=TRUE;
    } else if (!strcmp(argv[i], "-print_params")) { // TDRP argument - ignore it.
      OK=TRUE;
    } else if (!strcmp(argv[i], "-print_short")) { // TDRP argument - ignore it.
      OK=TRUE;
    } else if (!strcmp(argv[i], "-mode")) { // Set the mode.
      if (i < argc - 1) {
	sprintf(tmp_str, "mode = %s;", argv[i+1]);
	TDRP_add_override(&override, tmp_str);
	i=i+1; if (i>argc) continue;
      } else {
	OK = FALSE;
      }
    } else if (!strcmp(argv[i], "-f")) {
      //
      // This specifies a list of files to process. Put the system into archive
      // mode and record the number of files and their names.
      //  
      sprintf(tmp_str, "mode = ARCHIVE;");

      TDRP_add_override(&override, tmp_str);

      if(i < argc - 1) {

	int j;

	// search for next arg which starts with '-'

	for (j = i + 1; j < argc; j++)
	  if (argv[j][0] == '-')
	    break;
	
	/*
	 * compute number of files
	 */

	nFiles = j - i - 1;

	// set file name array

	filePaths = argv + i + 1;

	// Update i
	i=i+nFiles;
	
      }

    } else {
      OK = FALSE;
    } // if
    
  } // i

  if (!OK) {
    _usage(prog_name, stderr);
  }
    
}
//////////////////////////////////////////////////
//
// Small method to print a help message.
//
void Args::_usage(char *prog_name, FILE *out){

  fprintf(out, "%s%s%s%s",
	  "Usage: ", prog_name, " [options as below]\n",
	  "options:\n"
	  "       [ --, -h, -help, -man ] produce this list.\n"
	  "       [ -check_params ] check parameter use.\n"
	  "       [ -f file_paths] file paths list - ARCHIVE MODE\n"
	  "       [ -mode ?] ARCHIVE or REALTIME\n"
	  "       [ -params ?] params file path\n"
	  "       [ -print_params ] print parameters with comments\n"
	  "       [ -print_short ] print parameters - short version\n"
	  "\n");

  fprintf(out,
	  "NOTE: for ARCHIVE mode, use  -f \n"
	  "      ARCHIVE mode will be automatically invoked.\n");

  fprintf(out, "\n\n");

}






