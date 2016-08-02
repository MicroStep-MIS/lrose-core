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
// PPIField.hh
//
// PPIField object
//
// Mike Dixon, RAP, NCAR
// P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// Dec 2000
//
///////////////////////////////////////////////////////////////

#ifndef PPIField_HH
#define PPIField_HH

#include <vector>
#include <string>
#include <iostream>
#include "Params.hh"
using namespace std;

class Beam;
class sRadl;
class ScanParams;

////////////////////////
// This class

class PPIField {
  
public:

  // constructor

  PPIField(const Params &params);

  // destructor
  
  ~PPIField();

  // add a beam
  
  void addBeam(const Params &params,
	       const sRadl *radial, const ScanParams &sparams,
	       bool isBinary, double target_elev);

  // print

  void print(ostream &out);

  // print full
  
  void printFull(ostream &out);

  // rjp 11 Sep 2006 add convertRangeResolution for Beijing data
  // convertNexradResolution

  void convertNexradResolution(int maxGatesLimit);

  // data

  const Params &_params;
  string fieldName;
  time_t time;
  int scanNum;
  int maxGates;
  double rangeRes;
  double startRange;
  vector<Beam *> beams;
  
protected:

private:
  
};

#endif
