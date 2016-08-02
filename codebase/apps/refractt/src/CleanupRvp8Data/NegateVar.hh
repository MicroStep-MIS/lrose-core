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
 *   $Date: 2016/03/07 18:17:27 $
 *   $Id: NegateVar.hh,v 1.2 2016/03/07 18:17:27 dixon Exp $
 *   $Revision: 1.2 $
 *   $State: Exp $
 */
 
/**-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**/

/************************************************************************
 * NegateVar: Class for storing information about variables to negate.
 *
 * RAP, NCAR, Boulder CO
 *
 * July 2006
 *
 * Nancy Rehak
 *
 ************************************************************************/

#ifndef NegateVar_H
#define NegateVar_H

#include <iostream>
#include <string>

using namespace std;


class NegateVar
{
  
public:

  ////////////////////
  // Public members //
  ////////////////////

  string varName;
  string fillAttrName;
  

  ////////////////////
  // Public methods //
  ////////////////////

  /*********************************************************************
   * Constructors
   */

  NegateVar(const string &var_name = "",
	    const string &fill_attr_name = "") :
    varName(var_name),
    fillAttrName(fill_attr_name)
  {
  }
  

  /*********************************************************************
   * Destructor
   */

  ~NegateVar()
  {
  }


  //////////////////////////
  // Input/output methods //
  //////////////////////////

  /*********************************************************************
   * print() - Print the negate variable information to cerr.
   */

  inline void print() const
  {
    cerr << "   Variable: " << varName
	 << ", Fill attr: " << fillAttrName << endl;
  }

};

#endif
