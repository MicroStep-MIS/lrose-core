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
// FullCals.hh
//
// Holds full information on calibration of the HSRL for data processing
//
// Brad Schoenrock, EOL, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// Mar 2017
//
///////////////////////////////////////////////////////////////

#ifndef FullCals_HH
#define FullCals_HH

#include <Radx/RadxTime.hh>
#include "CalReader.hh"
#include <vector>

using namespace std;

class FullCals{
  
private:
  CalReader deadTimeHi, deadTimeLo, deadTimeCross, deadTimeMol, binWidth;
  //holds the deadtime calibration blocks from calvals file   
  
  //holds other calibration file data
  vector< vector<double> > blCor, diffDGeoCor, geoDefCor, afPulCor;
    
  //hold which spot in the vectors are the correct calibration point. 
  int hi_pos, lo_pos, cross_pos, mol_pos, bin_pos;
  
  RadxTime ti;

public:   
  FullCals();
  
  FullCals(CalReader dtHi, CalReader dtLo, 
	   CalReader dtCross, CalReader dtMol, 
	   CalReader binW, 
	   vector< vector<double> > blCorIn, 
	   vector< vector<double> > diffDGeoCorIn,
	   vector< vector<double> > geoDefCorIn, 
	   vector< vector<double> > afPulCorIn);
  
  void setDeadTimeHi(CalReader dtHi);
  void setDeadTimeLo(CalReader dtLo);
  void setDeadTimeCross(CalReader dtCross);
  void setDeadTimeMol(CalReader dtMol);
  void setBinWidth(CalReader binWidth); 

  void setDeadTimeHi(const char* file, const char* variable);
  void setDeadTimeLo(const char* file, const char* variable);
  void setDeadTimeCross(const char* file, const char* variable);
  void setDeadTimeMol(const char* file, const char* variable);
  void setBinWidth(const char* file, const char* variable); 

  void setBLCor(vector< vector<double> > blCor);
  void setDiffDGeoCor(vector< vector<double> > diffDGeoCor);
  void setGeoDefCor(vector< vector<double> > geoDefCor);
  void setAfPulCor(vector< vector<double> > afPulCor);

  void setBLCor(const char* file);
  void setDiffDGeoCor(const char* file);
  void setGeoDefCor(const char* file);
  void setAfPulCor(const char* file);
  
  CalReader getDeadTimeHi();
  CalReader getDeadTimeLo();
  CalReader getDeadTimeCross();
  CalReader getDeadTimeMol();
  CalReader getBinWidth(); 

  int getHiPos(); 
  int getLoPos(); 
  int getCrossPos(); 
  int getMolPos();
  int getBinPos();
  
  vector< vector<double> > getBLCor();
  vector< vector<double> > getDiffDGeoCor();
  vector< vector<double> > getGeoDefCor();
  vector< vector<double> > getAfPulCor();
  
  vector <vector<double> > readBaselineCorrection(const char* file);
  vector <vector<double> > readDiffDefaultGeo(const char* file);
  vector <vector<double> > readGeofileDefault(const char* file);
  vector <vector<double> > readAfterPulse(const char* file);

  ~FullCals();
  
};
#endif

