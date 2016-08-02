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
///////////////////////////////////////////////////////////////////
//
// Variable data for multiple linear regression 
//
// $Id: MultRegVar.cc,v 1.7 2016/03/07 01:33:50 dixon Exp $
//
///////////////////////////////////////////////////////////////////
#include <values.h>
#include <toolsa/DateTime.hh>
#include "MultRegVar.hh"
#include "MosCalibration.hh"
using namespace std;

//
// Constants
//
const double MultRegVar::MISSING_VAL = MAXDOUBLE;

MultRegVar::MultRegVar() 
{
   vecLen = MISSING_VAL;
   stdDev = MISSING_VAL;
}

void
MultRegVar::addVal( double inputVal ) 
{
   values.push_back( inputVal );
   vecLen = MISSING_VAL;
   stdDev = MISSING_VAL;
}

void
MultRegVar::clear() 
{
   values.erase( values.begin(), values.end() );
   vecLen = MISSING_VAL;
   stdDev = MISSING_VAL;
}

double
MultRegVar::getVal( int idex ) 
{
   if( idex < 0 || idex > (int) values.size() - 1 )
      return( MISSING_VAL );
   
   return( values[idex] );
}

double
MultRegVar::getVecLen() 
{
   if( vecLen == MISSING_VAL ) {
      findVecLen();
   }
   
   return( vecLen );
}

void
MultRegVar::findVecLen() 
{
   double sumSq = 0.0;
   
   vector< double >::iterator it;
   
   for( it = values.begin(); it != values.end(); it++ ) {
      if( (*it) != MISSING_VAL )
         sumSq += (*it) * (*it);
   }
   
   vecLen = sqrt( sumSq );
}

double
MultRegVar::getStdDev() 
{
   if( stdDev == MISSING_VAL ) {
      findStdDev();
   }
   
   return( stdDev );
}

void
MultRegVar::findStdDev() 
{
   int    count = 0;
   double sumY  = 0.0;
   double sumSq = 0.0;

   vector< double >::iterator it;
   
   for( it = values.begin(); it != values.end(); it++ ) {
      if( (*it) != MISSING_VAL ) {
         sumY  += (*it);
         sumSq += (*it) * (*it);
         count++;
      }
   }
   
   if( count > 0 ) {
      stdDev = ( sumSq - (sumY * sumY / count) ) / ( count - 1 );
   }
   else {
      stdDev = 0.0;
   }
}

