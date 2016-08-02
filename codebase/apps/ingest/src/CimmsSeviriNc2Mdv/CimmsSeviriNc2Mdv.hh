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
 * @file CimmsSeviriNc2Mdv.hh
 *
 * @class CimmsSeviriNc2Mdv
 *
 * CimmsSeviriNc2Mdv program object.
 *  
 * @date 11/13/2008
 *
 */

#ifndef CimmsSeviriNc2Mdv_HH
#define CimmsSeviriNc2Mdv_HH

#include <string>
#include <sys/time.h>
#include <vector>

#include <dsdata/DsTrigger.hh>
#include <Mdv/MdvxPjg.hh>
#include <toolsa/DateTime.hh>

#include "Args.hh"
#include "Params.hh"
#include "GridHandler.hh"
#include "NetcdfFile.hh"

using namespace std;

/** 
 * @class CimmsSeviriNc2Mdv
 */

class CimmsSeviriNc2Mdv
{
 public:

  ////////////////////
  // Public members //
  ////////////////////

  /**
   * @brief Flag indicating whether the program status is currently okay.
   */

  bool okay;


  ////////////////////
  // Public methods //
  ////////////////////

  //////////////////////////////
  // Constructors/Destructors //
  //////////////////////////////

  /**
   * @brief Destructor
   */

  ~CimmsSeviriNc2Mdv(void);
  

  /**
   * @brief Retrieve the singleton instance of this class.
   *
   * @param[in] argc Number of command line arguments.
   * @param[in] argv List of command line arguments.
   *
   * @return Returns a pointer to the class singleton instance.
   */

  static CimmsSeviriNc2Mdv *Inst(int argc, char **argv);


  /**
   * @brief Retrieve the singleton instance of this class.
   *
   * @return Returns a pointer to the class singleton instance.
   */

  static CimmsSeviriNc2Mdv *Inst();
  

  /**
   * @brief Initialize the local data.
   *
   * @return Returns true if the initialization was successful, false
   *         otherwise.
   */

  bool init();
  

  /////////////////////
  // Running methods //
  /////////////////////

  /**
   * @brief Run the program.
   */

  void run();
  

 private:

  /////////////////////
  // Private members //
  /////////////////////

  /**
   * @brief Singleton instance pointer.
   */

  static CimmsSeviriNc2Mdv *_instance;
  
  /**
   * @brief Programname.
   */

  char *_progName;

  /**
   * @brief Command line arguments.
   */

  Args *_args;

  /**
   * @brief Parameter file parameters.
   */

  Params *_params;
  
  /**
   * @brief Data triggering object.
   */

  DsTrigger *_dataTrigger;
  
  /**
   * @brief Output projection.
   */

  MdvxPjg _outputProj;
  
  /**
   * @brief The data grid handler.
   */

  GridHandler *_gridHandler;
  

  /////////////////////
  // Private methods //
  /////////////////////

  /**
   * @brief Constructor
   *
   * @param[in] argc Number of command line arguments.
   * @param[in] argv List of command line arguments.
   *
   * @note Private because this is a singleton object.
   */

  CimmsSeviriNc2Mdv(int argc, char **argv);
  

  /**
   * @brief Initialize the data grid handler.
   *
   * @return Returns true on success, false on failure.
   */

  bool _initGridHandler();
  

  /**
   * @brief Initialize the output projection.
   *
   * @return Returns true on success, false on failure.
   */

  bool _initOutputProjection();
  

  /**
   * @brief Initialize the data trigger.
   *
   * @return Returns true on success, false on failure.
   */

  bool _initTrigger();
  

  /**
   * @brief Process the given file.
   *
   * @param[in] input_file_path Path of input file to process.
   *
   * @return Returns true on success, false on failure.
   */

  bool _processFile(const string &input_file_path);
  

  /**
   * @brief Update the field header for the TPW field based on information
   *        in the netCDF file.
   *
   * @param[in,out] field_hdr Mdv field header to update.
   * @param[in] nc_file netCDF file.
   * @param[in] input_file_path Input file path.
   * @param[in] tpw_units TPW field units string.
   *
   * @return Returns true on success, false on failure.
   */

  bool _updateFieldHeader(Mdvx::field_header_t &field_hdr,
			  const NetcdfFile &nc_file,
			  const string &input_file_path,
			  const string &tpw_units) const;
  

  /**
   * @brief Update the master header in the given MDV file using information
   *        from the given netCDF file and file path.
   *
   * @param[out] mdvx MDV file whose master header needs to be updated.
   * @param[in] nc_file netCDF file.
   * @param[in] input_file_path Input file path.
   *
   * @return Returns true on success, false on failure.
   */

  bool _updateMasterHeader(Mdvx &mdvx,
			   const NetcdfFile &nc_file,
			   const string &input_file_path) const;
  

};


#endif
