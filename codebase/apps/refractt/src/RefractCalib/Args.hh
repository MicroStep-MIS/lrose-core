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
/**
 *
 * @file Args.hh
 *
 * @class Args
 *
 * Class for processing the command line arguments.
 *  
 * @date 1/15/2009
 *
 */

#ifndef Args_HH
#define Args_HH

#include <stdio.h>
#include <string>
#include <time.h>

#include <tdrp/tdrp.h>

using namespace std;


/** 
 * @class Args
 */

class Args
{
 public:

  /**
   * @brief Constructor
   *
   * @param[in] argc Number of command line arguments.
   * @param[in] argv List of command line arguments.
   * @param[in] prog_name Program name.
   */

  Args(int argc, char **argv, char *prog_name);
  
  /**
   * @brief Destructor
   */

  ~Args(void);
  
  /**
   * @brief TDRP overrides specified in the command line arguments.
   */

  tdrp_override_t override;
  
 private:

  /**
   * @brief The program name for error messages.
   */

  string _progName;
  
  /**
   * @brief Print the usage for this program.
   *
   * @param[in,out] stream The output stream to use for the message.
   */

  void _usage(FILE *stream);
  
};


#endif
