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
// Nancy Rehak, RAP, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// April 1998
//
//////////////////////////////////////////////////////////

#include <stdio.h>

#include <toolsa/os_config.h>
#include <tdrp/tdrp.h>
#include <toolsa/str.h>
#include <toolsa/umisc.h>

#include "Args.hh"

/**************************************************************
 * Constructor
 */

Args::Args(int argc, char **argv, char *prog_name)
{
  char tmp_str[BUFSIZ];

  // intialize

  okay = true;

  startTime = -1;
  endTime = -1;
  
  TDRP_init_override(&override);

  // loop through args
  
  for (int i =  1; i < argc; i++)
  {
    if (STRequal_exact(argv[i], "--") ||
	STRequal_exact(argv[i], "-h") ||
	STRequal_exact(argv[i], "-help") ||
	STRequal_exact(argv[i], "-man"))
    {
      _usage(prog_name, stdout);
      exit(0);
    }
    else if (STRequal_exact(argv[i], "-debug"))
    {
      sprintf(tmp_str, "debug_level = DEBUG_NORM;");
      TDRP_add_override(&override, tmp_str);
    }
    else if (STRequal_exact(argv[i], "-endtime"))
    {
      if (i < argc - 1)
      {
	if ((endTime = _parseTime(prog_name, argv[i+1], "endtime")) < 0)
	{
	  okay = FALSE;
	}
      }
      else
      {
	okay = FALSE;
      }

    }	
    else if (STRequal_exact(argv[i], "-mode"))
    {
      if (i < argc - 1)
      {
	sprintf(tmp_str, "mode = %s;", argv[++i]);
	TDRP_add_override(&override, tmp_str);
      }
      else
      {
	okay = FALSE;
      }
    }
    else if (STRequal_exact(argv[i], "-starttime"))
    {
      if (i < argc - 1)
      {
	if ((startTime = _parseTime(prog_name, argv[i+1], "starttime")) < 0)
	{
	  okay = FALSE;
	}
      }
      else
      {
	okay = FALSE;
      }

    }	
    
  } // i

  if (!okay)
    _usage(prog_name, stderr);
    
}


/**********************************************************************
 * Destructor
 */

Args::~Args(void)
{
  TDRP_free_override(&override);
}
  

/**************************************************************
 * PRIVATE MEMBER FUNCTIONS
 **************************************************************/

/**************************************************************
 * _parseTime()
 */

time_t Args::_parseTime(char *prog_name, char *time_str, char *label)
{
  date_time_t dtime;

  if (sscanf(time_str, "%d/%d/%d_%d:%d:%d",
	     &dtime.year,
	     &dtime.month,
	     &dtime.day,
	     &dtime.hour,
	     &dtime.min,
	     &dtime.sec) != 6)
  {
    fprintf(stderr, "ERROR - %s:_parseTime()\n", prog_name);
    fprintf(stderr, "Incorrect format for %s time.\n", label);
    fprintf(stderr, "Trying to decode '%s'\n", time_str);
    fprintf(stderr, "Format is yyyy/mm/dd_hh:mm:ss\n");
    return (-1);
  }
    
  /*
   * check that time is valid
   */
  
  if (!uvalid_datetime(&dtime))
  {
    fprintf(stderr, "ERROR - %s:parse_args\n", prog_name);
    fprintf(stderr, "Invalid reference date and time.\n");
    fprintf(stderr, "Trying to decode '%s'\n", time_str);
    return (-1);
  }
    
  uconvert_to_utime(&dtime);

  return (dtime.unix_time);
}


/**************************************************************
 * _usage()
 */

void Args::_usage(char *prog_name, FILE *out)
{
  fprintf(out, "%s%s%s%s",
	  "Usage: ", prog_name, " [options as below]\n",
	  "options:\n"
	  "       [ --, -h, -help, -man ] produce this list.\n"
	  "       [ -debug ] print debug messages\n"
	  "       [ -endtime yyyy/mm/dd_hh:mm:ss ] end time (ARCHIVE mode)\n"
	  "       [ -mode ?] ARCHIVE or REALTIME\n"
	  "       [ -starttime yyyy/mm/dd_hh:mm:ss ] start time (ARCHIVE mode)\n"
	  "\n");
  
  TDRP_usage(out);
}
