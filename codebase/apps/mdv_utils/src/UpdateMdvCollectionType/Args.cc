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
//   $Date: 2016/03/04 02:22:13 $
//   $Id: Args.cc,v 1.3 2016/03/04 02:22:13 dixon Exp $
//   $Revision: 1.3 $
//   $State: Exp $
 
/**-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**/
/*********************************************************************
 * Args.cc: class controlling the command line arguments for the
 *          program.
 *
 * RAP, NCAR, Boulder CO
 *
 * June 2007
 *
 * Nancy Rehak
 *
 *********************************************************************/

#include <string.h>

#include <toolsa/os_config.h>
#include <toolsa/udatetime.h>
#include <tdrp/tdrp.h>
#include <toolsa/DateTime.hh>
#include <toolsa/str.h>
#include <toolsa/umisc.h>

#include "Args.hh"
using namespace std;

/**********************************************************************
 * Constructor
 */

Args::Args (int argc, char **argv, const string prog_name)
{
  // Intialize

  char ovrstr[256];
  okay = true;
  _startTime = -1;
  _endTime = -1;
  
  TDRP_init_override(&override);
  
  // search for command options
  
  for (int i =  1; i < argc; i++)
  {
    if (STRequal_exact(argv[i], "--") ||
	STRequal_exact(argv[i], "-help") ||
	STRequal_exact(argv[i], "-h") ||
	STRequal_exact(argv[i], "-man")) {
      _usage(prog_name, cout);
      exit(0);
    }

    if (!strcmp(argv[i], "-debug"))
    {
      sprintf(ovrstr, "debug = DEBUG_NORM;");
      TDRP_add_override(&override, ovrstr);
    }
    else if (!strcmp(argv[i], "-verbose"))
    {
      sprintf(ovrstr, "debug = DEBUG_VERBOSE;");
      TDRP_add_override(&override, ovrstr);
    }
    else if (STRequal_exact(argv[i], "-coll_type"))
    {
      if (i < argc - 1)
      {
	sprintf(ovrstr, "data_collection_type = \"%s\";", argv[++i]);
	TDRP_add_override(&override, ovrstr);
      }
      else
      {
	okay = FALSE;
      }
    }
    else if (STRequal_exact(argv[i], "-url"))
    {
      if (i < argc - 1)
      {
	sprintf(ovrstr, "input_url = \"%s\";", argv[++i]);
	TDRP_add_override(&override, ovrstr);
      }
      else
      {
	okay = FALSE;
      }
    }
    else if (STRequal_exact(argv[i], "-file_list") ||
	     STRequal_exact(argv[i], "-f") ||
	     STRequal_exact(argv[i], "-if"))
    {
      while (i < argc - 1 && argv[i+1][0] != '-')
	_fileList.push_back(argv[++i]);
      TDRP_add_override(&override, "mode = FILE_LIST;");
    }
    else if (STRequal_exact(argv[i], "-endtime") ||
	     STRequal_exact(argv[i], "-end"))
    {
      if (i < argc - 1)
      {
	char *end_time = argv[++i];
	if ((_endTime = DateTime::parseDateTime(end_time)) < 0)
	{
	  cerr << "ERROR - " << prog_name << ":_parseTime()" << endl;
	  cerr << "Trying to decode '" << end_time << "'" << endl;
	  cerr << "Invalid date and time" << endl;
	  okay = FALSE;
	}
	else
	{
	  sprintf(ovrstr, "mode = TIME_LIST;");
	  TDRP_add_override(&override, ovrstr);
	}
      }
      else
      {
	okay = FALSE;
      }
    }
    else if (STRequal_exact(argv[i], "-starttime") ||
	     STRequal_exact(argv[i], "-start"))
    {
      if (i < argc - 1)
      {
	char *start_time = argv[++i];
	if ((_startTime = DateTime::parseDateTime(start_time)) < 0)
	{
	  cerr << "ERROR - " << prog_name << ":_parseTime()" << endl;
	  cerr << "Trying to decode '" << start_time << "'" << endl;
	  cerr << "Invalid date and time" << endl;
	  okay = FALSE;
	}
	else
	{
	  sprintf(ovrstr, "mode = TIME_LIST;");
	  TDRP_add_override(&override, ovrstr);
	}
      }
      else
      {
	okay = FALSE;
      }
    } // if
      
  } // i
    
  if (!okay) {
    _usage(prog_name, cerr);
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

void Args::_usage(const string prog_name, ostream &out)
{

  out << "Program: " << prog_name << endl << endl;

  out << "This program updates the data_collection_type field in the\n"
      << "master header of all specified MDV files to the data collection\n"
      << "type specified. The MDV files are updated in place, so make a\n"
      << "copy of the files before running if you need to have the original\n"
      << "versions, too.\n"
      << endl;
  
  out << "Usage: " << prog_name << " [options as below]\n"
      << "options:\n"
      << "       [ --, -h, -help, -man ] produce this list.\n"
      << "       [ -coll_type <type> ] new collection type to use\n"
      << "       [ -debug ] print debug messages\n"
      << "       [ -endtime \"yyyy mm dd hh mm ss\"] end time\n"
      << "         Sets mode to MULTIPLE\n"
      << "       [ -path ?] input file path\n"
      << "         Sets mode to SINGLE\n"
      << "       [ -url ?] input URL, MULTIPLE mode\n"
      << "       [ -starttime \"yyyy mm dd hh mm ss\"] start time\n"
      << "         Sets mode to MULTIPLE\n"
      << "       [ -verbose ] print verbose debug messages\n"
      << endl;

  Params::usage(out);

  out << endl;

}
