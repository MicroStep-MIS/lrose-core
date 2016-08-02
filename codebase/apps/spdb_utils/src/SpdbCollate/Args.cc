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
// Args.cc
//
// Command line args
//
// Mike Dixon, RAP, NCAR,
// P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// November 1999
//
//////////////////////////////////////////////////////////

#include "Args.hh"
#include "Params.hh"
#include <string.h>
#include <toolsa/umisc.h>
using namespace std;

// constructor

Args::Args()

{
  TDRP_init_override(&override);
}

// destructor

Args::~Args()

{
  TDRP_free_override(&override);
}

// parse

int Args::parse(int argc, char **argv, string &prog_name)

{

  int iret = 0;
  char tmp_str[256];

  // loop through args
  
  for (int i =  1; i < argc; i++) {

    if (!strcmp(argv[i], "--") ||
	!strcmp(argv[i], "-h") ||
	!strcmp(argv[i], "-help") ||
	!strcmp(argv[i], "-man")) {
      
      _usage(prog_name, cout);
      exit (0);
      
    } else if (!strcmp(argv[i], "-debug")) {
      
      sprintf(tmp_str, "debug = DEBUG_NORM;");
      TDRP_add_override(&override, tmp_str);
      
    } else if (!strcmp(argv[i], "-verbose")) {
      
      sprintf(tmp_str, "debug = DEBUG_VERBOSE;");
      TDRP_add_override(&override, tmp_str);
      
    } else if (!strcmp(argv[i], "-start")) {
      
      if (i < argc - 1) {
	date_time_t start;
	if (sscanf(argv[++i], "%d %d %d %d %d %d",
		   &start.year, &start.month, &start.day,
		   &start.hour, &start.min, &start.sec) != 6) {
	  iret = -1;
	} else {
	  sprintf(tmp_str,
		  "start_time = { %.4d, %.2d, %.2d, %.2d, %.2d, %.2d };",
		  start.year, start.month, start.day,
		  start.hour, start.min, start.sec);
	  TDRP_add_override(&override, tmp_str);
	}
      } else {
	iret = -1;
      }
	
    } else if (!strcmp(argv[i], "-end")) {
      
      if (i < argc - 1) {
	date_time_t end;
	if (sscanf(argv[++i], "%d %d %d %d %d %d",
		   &end.year, &end.month, &end.day,
		   &end.hour, &end.min, &end.sec) != 6) {
	  iret = -1;
	} else {
	  sprintf(tmp_str,
		  "end_time = { %.4d, %.2d, %.2d, %.2d, %.2d, %.2d };",
		  end.year, end.month, end.day,
		  end.hour, end.min, end.sec);
	  TDRP_add_override(&override, tmp_str);
	}
      } else {
	iret = -1;
      }
      
    } else if (!strcmp(argv[i], "-interval")) {
      
      if (i < argc - 1) {
	
	int interval;
	if (sscanf(argv[++i], "%d", &interval) != 1) {
	  iret = -1;
	} else {
	  sprintf(tmp_str, "data_interval = %d;", interval);
	  TDRP_add_override(&override, tmp_str);
	}
      } else {
	iret = -1;
      }
	
    } // if
    
  } // i

  if (iret) {
    _usage(prog_name, cerr);
  }

  return (iret);
    
}

void Args::_usage(string &prog_name, ostream &out)
{

  out << "Usage: " << prog_name << " [options as below]\n"
      << "options:\n"
      << "       [ --, -h, -help, -man ] produce this list.\n"
      << "       [ -debug ] print debug messages\n"
      << "       [ -end \"yyyy mm dd hh mm ss\"] end time\n"
      << "       [ -interval ?] data interval - secs\n"
      << "       [ -start \"yyyy mm dd hh mm ss\"] start time\n"
      << "       [ -verbose ] print verbose debug messages\n"
      << endl;
  
  Params::usage(out);

}







