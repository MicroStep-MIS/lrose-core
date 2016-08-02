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
///////////////////////////////////////////////////////////
//
//  MM5Point - class containing data from an mm5 record
//
//  $Id: MM5Point.cc,v 1.4 2016/03/07 01:33:51 dixon Exp $
//
///////////////////////////////////////////////////////////
#include <values.h>
#include "MM5Point.hh"
#include "MosVerify.hh"
using namespace std;

//
// Constants
//
const double MM5Point::MISSING_VAL = MAXDOUBLE;

MM5Point::MM5Point( time_t ptime, double lat, double lon ) 
{
   pointTime           = ptime;
   latitude            = lat;
   longitude           = lon;

   u                   = MISSING_VAL;
   v                   = MISSING_VAL;
   wspd                = MISSING_VAL;
   wdir                = MISSING_VAL;
   temperature         = MISSING_VAL;
   pressure            = MISSING_VAL;
   relativeHum         = MISSING_VAL;
   
}












