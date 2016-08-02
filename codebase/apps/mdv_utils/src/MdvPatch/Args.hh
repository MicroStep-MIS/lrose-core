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
/////////////////////////////////////////////////////////////
// $Id: Args.hh,v 1.2 2016/03/04 02:22:12 dixon Exp $
//
// Command line args for MdvPatch
//
// Yan Chen, RAL, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// March 2008
//
/////////////////////////////////////////////////////////////

#ifndef ARGS_HH
#define ARGS_HH

#include <iostream>
#include <string>
#include <tdrp/tdrp.h>

using namespace std;

class Args {
  
public:

  // constructor

  Args ();

  // Destructor

  ~Args();
  
  // parse the command line
  // Returns 0 on success, -1 on failure

  int parse (int argc, char **argv, string &prog_name);

  // usage

  void usage(ostream &out);

  // public data
  
  tdrp_override_t override;

protected:
  
private:

  string _progName;
  
};

#endif



