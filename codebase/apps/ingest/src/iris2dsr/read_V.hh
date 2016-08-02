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


# ifndef    READ_V_H
# define    READ_V_H

#include <vector>

#include "aBeam.hh"
#include "Params.hh"
using namespace std;


class read_V {
  
public:

  //
  // Constructor. Loads the file, gets beam data in
  // memory as floats.
  //
  read_V(Params *P, char *baseName, int tiltNum, double nyquistVel,
	 double origGateSpacingKm, double origFirstGateKm);

  //
  // Destructor. Does little, but avoids default destructor.
  //
  ~read_V();

  //
  // Get number of beams loaded from file.
  //
  int getNBeams();

  //
  // Get ith beam
  //
  aBeam getBeam(int beamNum );

protected:
private:

  double _getAngle(si16 bin2val);
  double _nyquistVel;


  Params *_params;

  vector <aBeam> _allBeams;

};


# endif
