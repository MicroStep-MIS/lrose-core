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
 *   $Date: 2016/03/07 18:17:26 $
 *   $Id: CalcMoisture.hh,v 1.3 2016/03/07 18:17:26 dixon Exp $
 *   $Revision: 1.3 $
 *   $State: Exp $
 */
 
/**-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**/

/************************************************************************
 * CalcMoisture: CalcMoisture program object.
 *
 * RAP, NCAR, Boulder CO
 *
 * June 2006
 *
 * Nancy Rehak
 *
 ************************************************************************/

#ifndef CalcMoisture_HH
#define CalcMoisture_HH

#include <string>

#include <dsdata/DsTrigger.hh>
#include <Mdv/MdvxField.hh>
#include <toolsa/DateTime.hh>

#include "Args.hh"
#include "Params.hh"

using namespace std;


class CalcMoisture
{
 public:

  ////////////////////
  // Public members //
  ////////////////////

  // Flag indicating whether the program status is currently okay.

  bool okay;


  ////////////////////
  // Public methods //
  ////////////////////

  //////////////////////////////
  // Constructors/Destructors //
  //////////////////////////////

  /*********************************************************************
   * Destructor
   */

  ~CalcMoisture(void);
  

  /*********************************************************************
   * Inst() - Retrieve the singleton instance of this class.
   */

  static CalcMoisture *Inst(int argc, char **argv);
  static CalcMoisture *Inst();
  

  /*********************************************************************
   * init() - Initialize the local data.
   *
   * Returns true if the initialization was successful, false otherwise.
   */

  bool init();
  

  /////////////////////
  // Running methods //
  /////////////////////

  /*********************************************************************
   * run() - run the program.
   */

  void run();
  

 private:

  /////////////////////
  // Private members //
  /////////////////////

  // Singleton instance pointer

  static CalcMoisture *_instance;
  
  // Program parameters.

  char *_progName;
  Args *_args;
  Params *_params;
  
  // Triggering object

  DsTrigger *_dataTrigger;
  

  /////////////////////
  // Private methods //
  /////////////////////

  /*********************************************************************
   * Constructor -- private because this is a singleton object
   */

  CalcMoisture(int argc, char **argv);
  

  /*********************************************************************
   * _calcDewPointField() - Calculate the dew point field from the given
   *                        data.
   *
   * Returns a pointer to the dew point field on success, 0 on failure.
   */

  MdvxField *_calcDewPointField(const MdvxField &e_field) const;
  

  /*********************************************************************
   * _calcDewPointField() - Calculate the relative humidity field from
   *                        the given dewpoint data.
   *
   * Returns a pointer to the RH field on success, 0 on failure.
   */

  MdvxField *_calcRelativeHumidityField(const MdvxField &dp_field,
                                        const double temp_k) const;

  /*********************************************************************
   * _calcWaterVaporField() - Calculate the water vapor field from the
   *                          given data.
   *
   * Returns a pointer to the water vapor field on success, 0 on failure.
   */

  MdvxField *_calcWaterVaporField(const MdvxField &n_field,
				  const double temp_k,
				  const double press_mb) const;
  

  /*********************************************************************
   * _getTempPress() - Get the mean temperature (in K) and pressure (in mb)
   *                   values using the stations specifiec in the parameter
   *                   file.
   *
   * Returns true on success, false on failure.
   */

  bool _getTempPress(const DateTime &data_time,
		     double &temp_k, double &press_mb);
  

  /*********************************************************************
   * _initTrigger() - Initialize the data trigger.
   *
   * Returns true on success, false on failure.
   */

  bool _initTrigger(void);
  

  /*********************************************************************
   * _processData() - Process data for the given trigger time.
   *
   * Returns true on success, false on failure.
   */

  bool _processData(const DateTime &trigger_time);
  

};


#endif
