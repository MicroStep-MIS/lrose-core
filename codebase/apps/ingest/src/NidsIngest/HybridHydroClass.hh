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
 * @file HybridHydroClass.hh
 * @brief Class for decoding Graphic Product Message Description Block for 
 *       Hybrid Hydrometeor Class. Product HHC code 177. 
 * 
 * @class HybridHydroClass
 * @brief Class for decoding Graphic Product Message Description Block for 
 *        Hybrid Hydrometeor Class. Product HHC code 177.  
 */

#ifndef HYBRID_HYDRO_CLASS_H
#define HYBRID_HYDRO_CLASS_H

#include <dataport/port_types.h>
#include "Product.hh"
#include <cmath>

using namespace std;

class HybridHydroClass: public Product {
  
public:

  /**
   * Constructor
   * Initializes public and private members
   * @params[in] filePtr  Pointer to file containing radar data
   * @params[in] byteSwapFlag  Flag to indicate byte swapping the various 
   *                           message components for handling endian issues 
   * @params[in] debugFlag  Flag to indicate debug messaging
   */
  HybridHydroClass (FILE *filePtr, bool byteSwapFlag, bool debugFlag);
  
  /**
   * Destructor
   */
  ~HybridHydroClass ();

  /** 
   * Converts byte data to float data. 
   *  val returned =( x-_bias)/_scale;
   * @param[in] x  Input byte
   * @return decoded float value
   */
  fl32 convertData(ui08 x);

   /**
   * Converts short data to float data.
   * Not used by this product
   * @return 0
   */
   fl32 convertData(ui16 x) { return missingDataVal; }

 
  /** 
   * Not defined for this product
   */
  void convertRLEData(ui08 x, ui08 &run, fl32 &val) { val = missingDataVal;}

  /**
   * Calculate product dependent values. See Interface Control Document(ICD) 
   * Table V, Note 1 of Figure 3-6 message 6 for each product.
   */
  void calcDependentVals();

  /**
   * Print the product dependent information for debugging purposes
   */
  void printProdDependentVals();

  /**
   * @return 1 of product is compressed and 0 otherwise. 
   */
  ui16 isCompressed() {return _isCompressed; }
   
  /**
   * @return size in bytes of uncompressed product. This is only valid if
   *         isCompressed returns 1. Else the uncompressed product size is 
   *         known by the file size.
   */
  long int getUncompProdSize() {return _uncompProdSize;}

  /**
   * @return elevation angle.
   */
  float getElevAngle() {return 0;}

protected:
  
private:

  /**
   * Mode Filter Size
   * See ICD Table V
   * Stored in hw47
   */ 
  ui16 _modeFilterSize;

  /**
   * Hybrid Rate Percent Bins Filled
   * See ICD Table V
   * Stored in hw48
   */
  fl32 _pctBinsFilled;
  
  /**
   * Highest Elevation Used
   * See ICD Table V
   * Stored in hw49
   */
  fl32 _highestElev;

  /**
   * Flag to indicate compression of radial data
   * See ICD Table V
   * Stored in hw51
   */
  ui16 _isCompressed;
  
  /**
   * If data are compressed, this is the uncompressed data size in bytes
   * See ICD Table V
   * Stored in hw52 and hw53
   */
  ui32 _uncompProdSize;
 
};

#endif





