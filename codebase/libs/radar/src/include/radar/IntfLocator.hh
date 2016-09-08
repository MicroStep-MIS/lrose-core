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
// IntfLocator.hh
//
// Mike Dixon, EOL, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// Sept 2016
//
///////////////////////////////////////////////////////////////
//
// Find gates contaminated with RLAN interference
//
///////////////////////////////////////////////////////////////

#ifndef IntfLocator_H
#define IntfLocator_H

#include <pthread.h>
#include <vector>
#include <radar/InterestMap.hh>
#include <toolsa/TaArray.hh>
using namespace std;

class IntfLocator {
  
public:

  // constructor
  
  IntfLocator();

  // destructor
  
  ~IntfLocator();

  // set debugging state

  void setDebug(bool state) { _debug = state; }

  //////////////////////////////////////////////////////

  // set size of kernel for computing stats

  void setNGatesKernel(int val) {
    _nGatesKernel = val;
  }

  // set interest maps for rlan location

  void setRlanInterestMapPhaseNoise
    (const vector<InterestMap::ImPoint> &pts,
     double weight);
  
  void setRlanInterestMapSnrDMode
    (const vector<InterestMap::ImPoint> &pts,
     double weight);
  
  void setRlanInterestMapNcpMean
    (const vector<InterestMap::ImPoint> &pts,
     double weight);
  
  void setRlanInterestMapWidthMean
    (const vector<InterestMap::ImPoint> &pts,
     double weight);
  
  void setRlanInterestThreshold(double val);

  // set radar props

  void setRadarHtM(double val) { _radarHtM = val; }
  void setWavelengthM(double val) { _wavelengthM = val; }
  
  ///////////////////////////////////////////////////////////////
  // set the ray properties
  // must be called before locate()
  // Set the nyquist if it is known.
  
  void setRayProps(time_t timeSecs, 
                   double nanoSecs,
                   double elevation, 
                   double azimuth,
                   int nGates,
                   double startRangeKm,
                   double gateSpacingKm);

  ///////////////////////////////////////////////////////////////
  // set the missing value for fields
  
  void setFieldMissingVal(double val) { _missingVal = val; }
  
  ///////////////////////////////////////////////////////////////
  // set the fields as available
  
  void setDbzField(double *vals, double noiseDbzAt100km);
  void setVelField(double *vals, double nyquist = -9999.0);
  void setWidthField(double *vals);
  void setNcpField(double *vals);
  void setSnrField(double *vals);
  void setZdrField(double *vals);

  //////////////////////////////////////////////
  // perform rlan location
  //
  // Must call setRayProps first, and set the fields.
  //
  // Min fields required:
  //
  //   vel
  //   ncp or width
  //   snr or dbz
  
  int rlanLocate();

  // get input fields - after setting ray props and fields

  const double *getDbz() const { return _dbz; }
  const double *getVel() const { return _vel; }
  const double *getPhase() const { return _phase; }
  const double *getWidth() const { return _width; }
  const double *getNcp() const { return _ncp; }
  const double *getSnr() const { return _snr; }
  const double *getZdr() const { return _zdr; }

  // get results - after running locate
  // these arrays span the gates from 0 to nGates-1

  const bool *getRlanFlag() const { return _rlanFlag; }
  const bool *getNoiseFlag() const { return _noiseFlag; }
  const bool *getSignalFlag() const { return _signalFlag; }
  
  const double *getAccumPhaseChange() const { return _accumPhaseChange; }
  const double *getPhaseNoise() const { return _phaseNoise; }
  
  const double *getSnrMode() const { return _snrMode; }
  const double *getSnrDMode() const { return _snrDMode; }
  const double *getZdrMode() const { return _zdrMode; }
  const double *getZdrDMode() const { return _zdrDMode; }
  const double *getNcpMean() const { return _ncpMean; }
  const double *getWidthMean() const { return _widthMean; }

  const double *getPhaseNoiseInterest() const { return _phaseNoiseInterest; }
  const double *getNcpMeanInterest() const { return _ncpMeanInterest; }
  const double *getWidthMeanInterest() const { return _widthMeanInterest; }
  const double *getSnrDModeInterest() const { return _snrDModeInterest; }

  ////////////////////////////////////
  // print parameters for debugging
  
  void printParams(ostream &out);

protected:
private:

  // debugging
  
  bool _debug;

  // radar properties

  double _radarHtM;
  double _wavelengthM;

  // ray properties

  time_t _timeSecs;
  double _nanoSecs;
  double _elevation;
  double _azimuth;
  int _nGates;

  double _startRangeKm;
  double _gateSpacingKm;

  double _nyquist;

  // field missing value

  double _missingVal;

  // arrays for input and computed data
  // and pointers to those arrays

  TaArray<double> _dbz_;
  double *_dbz;
  bool _dbzAvail;
  
  TaArray<double> _vel_;
  double *_vel;
  bool _velAvail;
  
  TaArray<double> _phase_;
  double *_phase;
  bool _phaseAvail;
  
  TaArray<double> _width_;
  double *_width;
  bool _widthAvail;

  TaArray<double> _ncp_;
  double *_ncp;
  bool _ncpAvail;

  TaArray<double> _snr_;
  double *_snr;
  bool _snrAvail;
  
  TaArray<double> _zdr_;
  double *_zdr;
  bool _zdrAvail;
  
  // results

  TaArray<bool> _rlanFlag_;
  bool *_rlanFlag;

  TaArray<bool> _noiseFlag_;
  bool *_noiseFlag;

  TaArray<bool> _signalFlag_;
  bool *_signalFlag;

  TaArray<double> _accumPhaseChange_;
  double *_accumPhaseChange;

  TaArray<double> _phaseNoise_;
  double *_phaseNoise;

  TaArray<double> _snrMode_;
  double *_snrMode;

  TaArray<double> _snrDMode_;
  double *_snrDMode;

  TaArray<double> _zdrMode_;
  double *_zdrMode;

  TaArray<double> _zdrDMode_;
  double *_zdrDMode;

  TaArray<double> _ncpMean_;
  double *_ncpMean;

  TaArray<double> _widthMean_;
  double *_widthMean;

  TaArray<double> _phaseNoiseInterest_;
  double *_phaseNoiseInterest;

  TaArray<double> _ncpMeanInterest_;
  double *_ncpMeanInterest;

  TaArray<double> _widthMeanInterest_;
  double *_widthMeanInterest;

  TaArray<double> _snrDModeInterest_;
  double *_snrDModeInterest;

  // gate limits for computing stats along a ray

  vector<size_t> _startGate;
  vector<size_t> _endGate;

  // fuzzy logic for rlan detection

  int _nGatesKernel;

  InterestMap *_rlanInterestMapPhaseNoise;
  InterestMap *_rlanInterestMapNcpMean;
  InterestMap *_rlanInterestMapWidthMean;
  InterestMap *_rlanInterestMapSnrDMode;

  double _rlanWeightPhaseNoise;
  double _rlanWeightSnrDMode;
  double _rlanWeightNcpMean;
  double _rlanWeightWidthMean;
  double _rlanInterestThreshold;

  // private methods
  
  void _computeSnrFromDbz(double noiseDbzAt100km);
  double _computePhaseNoise(int startGate, int endGate);
  void _computeDeltaMode(const string &fieldName,
                         const double *vals, double *mode, double *dMode);
  void _computeSdevInRange(const double *vals, double *sdevs);
  void _computeMeanInRange(const double *vals, double *means);
  double _computeMean(const vector<double> &vals);
  double _computeMedian(const vector<double> &vals);
  void _createDefaultInterestMaps();

};

#endif
