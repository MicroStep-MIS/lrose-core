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

// RCS info
//   $Author: dixon $
//   $Locker:  $
//   $Date: 2016/03/07 01:50:09 $
//   $Id: StnInterpField.cc,v 1.2 2016/03/07 01:50:09 dixon Exp $
//   $Revision: 1.2 $
//   $State: Exp $
//
 
/**-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**/
/*********************************************************************
 * StnInterpField: Base class for interpolated fields based on surface
 *                 station data.
 *
 * RAP, NCAR, Boulder CO
 *
 * September 2007
 *
 * Nancy Rehak
 *
 *********************************************************************/

#include "StnInterpField.hh"

using namespace std;


/*********************************************************************
 * Constructors
 */

StnInterpField::StnInterpField(const string &field_name,
			       const string &field_units,
			       Interpolater *interpolater,
			       const bool output_field_flag,
			       const bool debug_flag) :
  InterpField(field_name, field_units, interpolater,
	      output_field_flag, debug_flag)
{
}

  
/*********************************************************************
 * Destructor
 */

StnInterpField::~StnInterpField()
{
}


/*********************************************************************
 * addObs() - Add the given observation to the intepolated field.
 *
 * Returns true on success, false on failure.
 */

bool StnInterpField::addObs(const station_report_t &report,
			    const float *interp_distance_grid)
{
  float obs_value = _calcValue(report);
  
  _interpolater->addObs(obs_value, report.lat, report.lon,
			STATION_NAN, interp_distance_grid);
  
  return true;
}


/**********************************************************************
 *              Protected/Private Member Functions                    *
 **********************************************************************/

