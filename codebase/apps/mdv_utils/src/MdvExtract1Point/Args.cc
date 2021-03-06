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
//   $Date: 2016/03/04 02:22:11 $
//   $Id: Args.cc,v 1.3 2016/03/04 02:22:11 dixon Exp $
//   $Revision: 1.3 $
//   $State: Exp $
 
/**-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**/
/*********************************************************************
 * Args: class controlling the command line arguments for the
 *       program.
 *
 * RAP, NCAR, Boulder CO
 *
 * August 2009
 *
 * Nancy Rehak
 *
 *********************************************************************/

#include <fstream>
#include <iostream>
#include <string>

#include <string.h>

#include <toolsa/os_config.h>
#include <tdrp/tdrp.h>
#include <toolsa/DateTime.hh>
#include <toolsa/str.h>
#include <toolsa/umisc.h>

#include "Args.hh"

using namespace std;

/**********************************************************************
 * Constructor
 */

Args::Args (int argc, char **argv, char *prog_name) :
  _progName(prog_name),
  _startTime(DateTime::NEVER),
  _endTime(DateTime::NEVER)
{
  string tmp_str;

  // Intialize

  bool okay = true;

  TDRP_init_override(&override);
  
  // search for command options
  
  for (int i =  1; i < argc; i++)
  {
    if (STRequal_exact(argv[i], "--") ||
	STRequal_exact(argv[i], "-help") ||
	STRequal_exact(argv[i], "-man"))
    {
      _usage(stdout);
      exit(0);
    }
    else if (STRequal_exact(argv[i], "-debug"))
    {
      tmp_str = "debug = true;";
      TDRP_add_override(&override, tmp_str.c_str());
    }
    else if (STRequal_exact(argv[i], "-end") ||
	     STRequal_exact(argv[i], "-endtime") ||
	     STRequal_exact(argv[i], "-end_time"))
    {
      if (i < argc - 1)
      {
	if (_endTime.set(argv[++i]) == DateTime::NEVER)
	{
	  cerr << "*** Invalid end time string entered: " <<
	    argv[i] << endl << endl;
	  
	  okay = false;
	}
	
	TDRP_add_override(&override, "trigger_mode = TIME_LIST;");
      }
      else
      {
	okay = false;
      }
    }
    else if (STRequal_exact(argv[i], "-start") ||
	     STRequal_exact(argv[i], "-starttime") ||
	     STRequal_exact(argv[i], "-start_time"))
    {
      if (i < argc - 1)
      {
	if (_startTime.set(argv[++i]) == DateTime::NEVER)
	{
	  cerr << "*** Invalid start time string entered: " <<
	    argv[i] << endl << endl;
	  
	  okay = false;
	}
	
	TDRP_add_override(&override, "trigger_mode = TIME_LIST;");
      }
      else
      {
	okay = false;
      }
    }
    else if (STRequal_exact(argv[i], "-url"))
    {
      if (i < argc - 1)
      {
	tmp_str = (((string) "url = \"") + argv[++i]) + "\";";
	TDRP_add_override(&override, tmp_str.c_str());
      }
      else
      {
	okay = false;
      }
    }
    else if (STRequal_exact(argv[i], "-max_valid_secs"))
    {
      if (i < argc - 1)
      {
	tmp_str = (((string) "max_valid_secs = ") + argv[++i]) + ";";
	TDRP_add_override(&override, tmp_str.c_str());
      }
      else
      {
	okay = false;
      }
    }
    else if (STRequal_exact(argv[i], "-field_name"))
    {
      if (i < argc - 1)
      {
	tmp_str = (((string) "field_name = \"") + argv[++i]) + "\";";
	TDRP_add_override(&override, tmp_str.c_str());
      }
      else
      {
	okay = false;
      }
    }
    else if (STRequal_exact(argv[i], "-field_num"))
    {
      if (i < argc - 1)
      {
	tmp_str = (((string) "field_num = ") + argv[++i]) + ";";
	TDRP_add_override(&override, tmp_str.c_str());
      }
      else
      {
	okay = false;
      }
    }
    else if (STRequal_exact(argv[i], "-vlevel_num"))
    {
      if (i < argc - 1)
      {
	tmp_str = (((string) "vlevel_num = ") + argv[++i]) + ";";
	TDRP_add_override(&override, tmp_str.c_str());
      }
      else
      {
	okay = false;
      }
    }
    else if (STRequal_exact(argv[i], "-lat"))
    {
      if (i < argc - 1)
      {
	tmp_str = (((string) "lat = ") + argv[++i]) + ";";
	TDRP_add_override(&override, tmp_str.c_str());
      }
      else
      {
	okay = false;
      }
    }
    else if (STRequal_exact(argv[i], "-lon"))
    {
      if (i < argc - 1)
      {
	tmp_str = (((string) "lon = ") + argv[++i]) + ";";
	TDRP_add_override(&override, tmp_str.c_str());
      }
      else
      {
	okay = false;
      }
    }
  } /* i */

  if (!okay)
  {
    _usage(stderr);
    exit(-1);
  }
}


/**********************************************************************
 * Destructor
 */

Args::~Args(void)
{
  TDRP_free_override(&override);
}
  

/**********************************************************************
 *              Private Member Functions                              *
 **********************************************************************/

/**********************************************************************
 * _usage() - Print the usage for this program.
 */

void Args::_usage(FILE *stream)
{
  fprintf(stream, "%s%s%s",
	  "This program extracts the field values for a given list of\n"
	  "lat/lon locations and outputs them in the specified format.\n"
	  "\n"
	  "Usage:\n\n", _progName.c_str(), " [options] as below:\n\n"
	  "       [ --, -help, -man ]\n"
	  "           produce this list.\n"
	  "       [ -debug ]\n"
	  "           turn debugging on\n"
	  "       [ -start <start time> ]\n"
	  "           start time for archive mode\n"
	  "       [ -end <end time> ]\n"
	  "           end time for archive mode\n"
	  "       [ -url <url>]\n"
	  "           url to MDV files\n"
	  "       [ -max_valid_secs <num>]\n"
	  "           Maximum number of seconds input files are valid.\n"
	  "           Used only if trigger_mode is set to LATEST_DATA.\n"
	  "       [ -field_name <name> ]\n"
	  "           Name of field to extract from the MDV file. \n"
	  "           Set to \"\" to use field number instead.\n"
	  "       [ -field_num <num> ]\n"
	  "           field_num - 0-based index of field to extract from the MDV "
	  "           file. Used only if field_name is set to \"\".\n"
	  "       [ -vlevel_num <num> ]\n\n"
	  "       [ -lat <num> ]\n\n"
	  "       [ -lon <num> ]\n\n"
	  "\n"
    );


  TDRP_usage(stream);
}
