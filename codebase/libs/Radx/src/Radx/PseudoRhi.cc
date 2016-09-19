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
// PseudoRhi.cc
//
// RadxRay objects from PPI volume combined in a stack one
// above the other to form a pseudo RHI
//
// Mike Dixon, EOL, NCAR
// P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// Sept 2016
//
///////////////////////////////////////////////////////////////

#include <Radx/PseudoRhi.hh>
#include <Radx/RadxRay.hh>
#include <cmath>
using namespace std;

//////////////
// Constructor

PseudoRhi::PseudoRhi()
  
{
  _init();
}

/////////////////////////////
// Copy constructor
//

PseudoRhi::PseudoRhi(const PseudoRhi &rhs)
     
{
  _init();
  _copy(rhs);
}

/////////////
// destructor

PseudoRhi::~PseudoRhi()

{
  clear();
}

/////////////////////////////
// Assignment
//

PseudoRhi &PseudoRhi::operator=(const PseudoRhi &rhs)
{
  return _copy(rhs);
}

/////////////////////////////////////////////////////////
// initialize data members

void PseudoRhi::_init()
  
{
  _debug = false;
  clear();
}

//////////////////////////////////////////////////
// copy - used by copy constructor and operator =
//

PseudoRhi &PseudoRhi::_copy(const PseudoRhi &rhs)
  
{
  
  if (&rhs == this) {
    return *this;
  }

  _lowLevelAzimuth = rhs._lowLevelAzimuth;
  _meanAzimuth = rhs._meanAzimuth;
  
  // rays
  
  clearRays();
  for (size_t ii = 0; ii < rhs._rays.size(); ii++) {
    RadxRay *ray = new RadxRay(*rhs._rays[ii]);
    addRay(ray);
  }

  return *this;
  
}

/////////////////////////////////////////////////////////
// clear the data in the object

void PseudoRhi::clear()
  
{
  _clearRays();
  _lowLevelAzimuth = 0.0;
  _meanAzimuth = 0.0;
}

/////////////////////////////////////////////////////////
// clear rays

void PseudoRhi::_clearRays()
  
{

  // delete rays

  for (size_t ii = 0; ii < _rays.size(); ii++) {
    RadxRay::deleteIfUnused(_rays[ii]);
  }
  _rays.clear();

}

///////////////
// set debug

void PseudoRhi::setDebug(bool val) {
  _debug = val;
}

////////////////////////////////////////////////////////////////
// add a ray

void PseudoRhi::addRay(RadxRay *ray)
  
{
  _rays.push_back(ray);
  ray->addClient();  // take responsibility for ray memory
}

////////////////////////////////////////////////////////////////
/// compute the max number of gates
/// Also determines if number of gates vary by ray.
/// After this call, use the following to get the results:
///   size_t getMaxNGates() - see RadxPacking.
///   bool nGatesVary() - see RadxPacking

void PseudoRhi::computeMaxNGates() const
  
{
  _maxNGates = 0;
  _nGatesVary = false;
  size_t prevNGates = 0;
  for (size_t iray = 0; iray < _rays.size(); iray++) {
    RadxRay &ray = *_rays[iray];
    size_t rayNGates = ray.getNGates();
    if (rayNGates > _maxNGates) {
      _maxNGates = rayNGates;
    }
    if (iray > 0) {
      if (rayNGates != prevNGates) {
        _nGatesVary = true;
      }
    }
    prevNGates = rayNGates;
  }
}

/////////////////////////////////////////////////////////
// print

void PseudoRhi::print(ostream &out) const
  
{
  out << "=============== PseudoRhi ===============" << endl;
  out << "  lowLevelAzimuth: " << _lowLevelAzimuth << endl;
  out << "  meanAzimuth: " << _meanAzimuth << endl;
  out << "  maxNGates: " << _maxNGates << endl;
  out << "  nGatesVary: " << _nGatesVary << endl;
  out << "  n rays: " << _rays.size() << endl;
}

///////////////////////////////////
// print with ray meta data

void PseudoRhi::printWithRayMetaData(ostream &out) const

{
  
  print(out);
  
  // print rays
  
  for (size_t ii = 0; ii < _rays.size(); ii++) {
    _rays[ii]->print(cout);
  }

}

///////////////////////////////////
// print with ray summary

void PseudoRhi::printRaySummary(ostream &out) const

{
  
  print(out);
  
  // print rays

  out << "================ RAY SUMMARY =================" << endl;
  for (size_t ii = 0; ii < _rays.size(); ii++) {
    _rays[ii]->printSummary(cout);
  }

}

//////////////////////////  
// print with field data

void PseudoRhi::printWithFieldData(ostream &out) const

{
  
  print(out);

  // check if rays have fields

  bool raysHaveFields = false;
  for (size_t ii = 0; ii < _rays.size(); ii++) {
    if (_rays[ii]->getFields().size() > 0) {
      raysHaveFields = true;
      break;
    }
  }

  // print rays

  if (raysHaveFields) {
    for (size_t ii = 0; ii < _rays.size(); ii++) {
      _rays[ii]->printWithFieldData(cout);
    }
  } else {
    for (size_t ii = 0; ii < _rays.size(); ii++) {
      _rays[ii]->print(cout);
    }
  }

}
  
//////////////////////////////////////////////////////////  
/// Compute the mean angle from the rays

void PseudoRhi::computeMeanAngleFromRays()

{
  
  // sum up (x,y) coords of measured angles
  
  double sumx = 0.0;
  double sumy = 0.0;
    
  for (size_t iray = 0; iray < _rays.size(); iray++) {
      
    const RadxRay *ray = _rays[iray];
    double angle = ray->getAzimuthDeg();
    double sinVal, cosVal;
    Radx::sincos(angle * Radx::DegToRad, sinVal, cosVal);
    sumy += sinVal;
    sumx += cosVal;
    
  } // iray

  // compute mean angle, to use as fixed angle

  _meanAzimuth = atan2(sumy, sumx) * Radx::RadToDeg;

}

