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

/* RCS info
 *   $Author: dixon $
 *   $Locker:  $
 *   $Date: 2016/03/04 02:22:15 $
 *   $Id: Args.hh,v 1.3 2016/03/04 02:22:15 dixon Exp $
 *   $Revision: 1.3 $
 *   $State: Exp $
 */
 
/**-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**/

/************************************************************************
 * Args.hh : header file for the Args class.
 *
 * RAP, NCAR, Boulder CO
 *
 * February 2000
 *
 * Nancy Rehak
 *
 ************************************************************************/

#ifndef Args_HH
#define Args_HH

/*
 **************************** includes **********************************
 */

#include <cstdio>

#include <tdrp/tdrp.h>
using namespace std;

/*
 ******************************* defines ********************************
 */


/*
 ******************************* structures *****************************
 */

/*
 ************************* global variables *****************************
 */

/*
 ***************************** function prototypes **********************
 */

/*
 ************************* class definitions ****************************
 */

class Args
{
 public:

  ////////////////////
  // Public members //
  ////////////////////

  // TDRP overrides specified in the command line arguments.

  tdrp_override_t override;
  

  ////////////////////
  // Public methods //
  ////////////////////

  ////////////////////////////////
  // Constructors & destructors //
  ////////////////////////////////

  // Constructor

  Args(int argc, char **argv, char *prog_name);
  
  // Destructor

  ~Args(void);
  

  ////////////////////
  // Access methods //
  ////////////////////

  time_t getStartTime()
  {
    return _startTime;
  }
  
  time_t getEndTime()
  {
    return _endTime;
  }
  

 private:

  /////////////////////
  // Private members //
  /////////////////////

  // Start and end times for archive mode

  time_t _startTime;
  time_t _endTime;


  /////////////////////
  // Private methods //
  /////////////////////

  // Parse the time strings for archive mode

  time_t _parseTime(char *time_str, char *prog_name, char *label);
  
  // Print the usage for this program.

  void _usage(char *prog_name,
	      FILE *stream);
  
  // Return the class name for error messages.

  static const char *_className(void)
  {
    return("Args");
  }
  
};


#endif
