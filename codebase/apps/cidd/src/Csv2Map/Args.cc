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
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

// RCS info
//   $Author: dixon $
//   $Locker:  $
//   $Date: 2016/03/07 18:28:25 $
//   $Id: Args.cc,v 1.2 2016/03/07 18:28:25 dixon Exp $
//   $Revision: 1.2 $
//   $State: Exp $
 
/**-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**/
/*********************************************************************
 * Args.cc: class controlling the command line arguments for the
 *          program.
 *
 * RAP, NCAR, Boulder CO
 *
 * Sept 2014
 *
 * Nancy Rehak
 *
 *********************************************************************/

#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>

#include <toolsa/os_config.h>
#include <toolsa/str.h>

#include "Args.hh"

using namespace std;

/**********************************************************************
 * Constructor
 */

Args::Args (int argc, char **argv, char *prog_name)
{
  string tmp_str;

  // Intialize

  bool okay = true;

  TDRP_init_override(&override);
  
  // search for command options
  
  for (int i =  1; i < argc; i++)
  {
    string arg = argv[i];
    
    if (arg == "--" ||
	arg == "-help" ||
	arg == "-man")
    {
      _usage(prog_name, stdout);
      exit(0);
    }
    else if (arg == "-debug")
    {
      tmp_str = "debug = true;";
      TDRP_add_override(&override, (char *)tmp_str.c_str());
    }
    else if (arg == "-f" ||
	     arg == "-file" ||
	     arg == "-input_file")
    {
      ++i;
      if (i >= argc)
      {
	cerr << "*** ERROR: Input file path not specified" << endl;
	_usage(prog_name, stderr);
	exit(-1);
      }
      
      tmp_str = "input_path = \"" + (string)argv[i] + "\";";
      TDRP_add_override(&override, (char *)tmp_str.c_str());
    }
  } /* i */

  if (!okay)
  {
    _usage(prog_name, stderr);
    exit(-1);
  }
    
}


/**********************************************************************
 * Destructor
 */

Args::~Args(void)
{
}
  

/**********************************************************************
 *              Private Member Functions                              *
 **********************************************************************/

/**********************************************************************
 * _usage() - Print the usage for this program.
 */

void Args::_usage(char *prog_name, FILE *stream)
{
  fprintf(stream, "%s%s%s",
	  "Usage:\n\n", prog_name, " [options]\n"
	  " options as below:\n\n"
	  "       [ --, -help, -man ] produce this list.\n"
	  "       [ -debug ] debugging on\n"
	  "       [ -f <input file path> ] use this input csv file\n"
	  "\n");


  TDRP_usage(stream);
}






