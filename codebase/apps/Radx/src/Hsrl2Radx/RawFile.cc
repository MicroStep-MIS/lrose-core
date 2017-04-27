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
// RawFile.cc
//
// UW Raw HSRL NetCDF data
//
// Mike Dixon, EOL, NCAR
// P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// Feb 2017
//
///////////////////////////////////////////////////////////////

#include "RawFile.hh"
#include <Radx/RadxTime.hh>
#include <Radx/RadxVol.hh>
#include <Radx/RadxField.hh>
#include <Radx/RadxRay.hh>
#include <Radx/RadxGeoref.hh>
#include <Radx/RadxSweep.hh>
#include <Radx/RadxRcalib.hh>
#include <Radx/RadxPath.hh>
#include <Radx/RadxArray.hh>
#include <Radx/RadxGeoref.hh>
#include <Radx/RadxCfactors.hh>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>
using namespace std;

//////////////
// Constructor

RawFile::RawFile(const Params &params) :
        _params(params)
  
{

  clear();

}

/////////////
// destructor

RawFile::~RawFile()

{
  clear();
}

/////////////////////////////////////////////////////////
// clear the data in the object

void RawFile::clear()
  
{

  clearErrStr();
  
  _file.close();

  _timeDim = NULL;
  _timeVecDim = NULL;
  _binCountDim = NULL;

  _nTimesInFile = 0;
  _timeVecSize = 0;
  _nBinsInFile = 0;

  _machType.clear();
  _hostName.clear();
  _userName.clear();
  _gitCommit.clear();
  _hsrlVersion = -9999;
  _dataAdded.clear();
  _sourceSoftware.clear();

  _timeVar = NULL;
  _dataTimes.clear();
  _dTimes.clear();
  
  _telescopeLockedVar = NULL;
  _telescopeDirectionVar = NULL;

  _latitudeVar = NULL;
  _longitudeVar = NULL;
  _altitudeVar = NULL;
  _headingVar = NULL;
  _gndSpeedVar = NULL;
  _vertVelVar = NULL;
  _pitchVar = NULL;
  _rollVar = NULL;

  _telescopeLocked.clear();
  _telescopeDirection.clear();

  _latitude.clear();
  _longitude.clear();
  _altitude.clear();
  _heading.clear();
  _gndSpeed.clear();
  _vertVel.clear();
  _pitch.clear();
  _roll.clear();

  _instrumentType = Radx::INSTRUMENT_TYPE_LIDAR;
  _platformType = Radx::PLATFORM_TYPE_AIRCRAFT;
  _primaryAxis = Radx::PRIMARY_AXIS_Y_PRIME;

  _rawGateSpacingKm = _params.raw_bin_spacing_km;
  // _gateSpacingKm = _rawGateSpacingKm * cos(4.0 * Radx::DegToRad);
  _gateSpacingKm = _rawGateSpacingKm;
  if (_params.combine_bins_on_read) {
    _gateSpacingKm *= _params.n_bins_per_gate;
  }
  _startRangeKm = _params.raw_bin_start_range_km;

  _rays.clear();
  
}

////////////////////////////////////////////////////////////
// Check if this is a CfRadial file
// Returns true on success, false on failure

bool RawFile::isRawHsrlFile(const string &path)
  
{

  clear();
  
  // open file

  if (_file.openRead(path)) {
    if (_params.debug >= Params::DEBUG_VERBOSE) {
      cerr << "DEBUG - not Raw HSRL file" << endl;
      cerr << _file.getErrStr() << endl;
    }
    return false;
  }

  // read dimensions
  
  if (_readDimensions()) {
    _file.close();
    if (_params.debug >= Params::DEBUG_VERBOSE) {
      cerr << "DEBUG - not Raw HSRL file" << endl;
      cerr << _errStr << endl;
    }
    return false;
  }

  // file has the correct dimensions, so it is a CfRadial file

  _file.close();
  return true;

}

////////////////////////////////////////////////
// get the date and time from a dorade file path
// returns 0 on success, -1 on failure

int RawFile::getTimeFromPath(const string &path, RadxTime &rtime)

{

  RadxPath rpath(path);
  const string &fileName = rpath.getFile();
  
  // find first digit in entry name - if no digits, return now

  const char *start = NULL;
  for (size_t ii = 0; ii < fileName.size(); ii++) {
    if (isdigit(fileName[ii])) {
      start = fileName.c_str() + ii;
      break;
    }
  }
  if (!start) return -1;
  const char *end = start + strlen(start);
  
  // iteratively try getting the date and time from the string
  // moving along by one character at a time
  
  while (start < end - 6) {
    int year, month, day, hour, min, sec;
    if (sscanf(start, "%4d%2d%2d_%2d%2d%2d",
               &year, &month, &day, &hour, &min, &sec) == 6) {
      if (year < 1900 || month < 1 || month > 12 || day < 1 || day > 31) {
        return -1;
      }
      if (hour < 0 || hour > 23 || min < 0 || min > 59 || sec < 0 || sec > 59) {
        return -1;
      }
      rtime.set(year, month, day, hour, min, sec);
      return 0;
    }
    start++;
  }
  
  return -1;
  
}

////////////////////////////////////////////////////////////
// Read in data from specified path, load up volume object.
//
// Returns 0 on success, -1 on failure
//
// Use getErrStr() if error occurs

int RawFile::readFromPath(const string &path,
                          RadxVol &vol)
  
{
  
  if (_params.debug) {
    cerr << "Reading file: " << path << endl;
  }

  string errStr("ERROR - RawFile::readFromPath");

  _readVol = &vol;

  // clear tmp rays
  
  _nTimesInFile = 0;
  _nBinsInFile = 0;
  _rays.clear();

  // open file

  if (_file.openRead(path)) {
    _addErrStr(_file.getErrStr());
    return -1;
  }
  
  // read dimensions
  
  if (_readDimensions()) {
    _addErrStr(errStr);
    return -1;
  }

  // read global attributes
  
  if (_readGlobalAttributes()) {
    _addErrStr(errStr);
    return -1;
  }

  // read time variable
  
  if (_readTimes()) {
    _addErrStr(errStr);
    return -1;
  }
  
  // read in ray metadata variables
  
  if (_readRayVariables()) {
    _addErrStr(errStr);
    return -1;
  }
  
  // create the rays, filling out the metadata
  
  if (_createRays(path)) {
    _addErrStr(errStr);
    return -1;
  }
  
  // add field variables to file rays
  
  if (_readFieldVariables()) {
    _addErrStr(errStr);
    return -1;
  }

  // load the data into the read volume

  _loadReadVolume();

  // close file
  
  _file.close();

  // clean up

  _rays.clear();
  _dataTimes.clear();
  _dTimes.clear();

  return 0;

}

///////////////////////////////////
// read in the dimensions

int RawFile::_readDimensions()

{

  // read required dimensions
  
  if (_file.readDim("time", _timeDim) == 0) {
    _nTimesInFile = _timeDim->size();
  } else {
    _addErrStr("ERROR - RawFile::_readDimensions()");
    _addErrStr("  Cannot find 'time' dimension");
    return -1;
  }
  

  if (_file.readDim("time_vector", _timeVecDim) == 0) {
    _timeVecSize = _timeVecDim->size();
  } else {
    _addErrStr("ERROR - RawFile::_readDimensions()");
    _addErrStr("  Cannot find 'time_vector' dimension");
    return -1;
  }
  
  if (_file.readDim("bincount", _binCountDim) == 0) {
    _nBinsInFile = _binCountDim->size();
  } else {
    _addErrStr("ERROR - RawFile::_readDimensions()");
    _addErrStr("  Cannot find 'bincount' dimension");
    return -1;
  }

  _nPoints = _nTimesInFile * _nBinsInFile;
  _nBinsPerGate = 1;
  if (_params.combine_bins_on_read) {
    _nBinsPerGate = _params.n_bins_per_gate;
  }
  _nGates = _nBinsInFile / _nBinsPerGate;
  
  return 0;

}

///////////////////////////////////
// read the global attributes

int RawFile::_readGlobalAttributes()

{

  _machType.clear();
  _hostName.clear();
  _userName.clear();
  _gitCommit.clear();
  _hsrlVersion = -9999;
  _sourceSoftware.clear();

  for (int ii = 0; ii < _file.getNcFile()->num_atts(); ii++) {
    
    NcAtt* att = _file.getNcFile()->get_att(ii);
    
    if (att == NULL) {
      continue;
    }

    if (!strcmp(att->name(), "NCUTIL_Machtype")) {
      _machType = NetcdfClassic::asString(att);
    }

    if (!strcmp(att->name(), "NCUTIL_Hostname")) {
      _hostName = NetcdfClassic::asString(att);
    }

    if (!strcmp(att->name(), "NCUTIL_Username")) {
      _userName = NetcdfClassic::asString(att);
    }

    if (!strcmp(att->name(), "NCUTIL_HSRL_GIT_COMMIT")) {
      _gitCommit = NetcdfClassic::asString(att);
    }

    if (!strcmp(att->name(), "DATA_HSRLVersion")) {
      _hsrlVersion = att->as_int(0);
    }

    if (!strcmp(att->name(), "DATA_Added")) {
      _dataAdded = NetcdfClassic::asString(att);
    }

    if (!strcmp(att->name(), "DATA_SourceSoftware")) {
      _sourceSoftware = NetcdfClassic::asString(att);
    }

    // Caller must delete attribute

    delete att;
    
  } // ii

  if (_params.debug >= Params::DEBUG_VERBOSE) {

    cerr << "Gobal attr machType: " << _machType << endl;
    cerr << "Gobal attr hostName: " << _hostName << endl;
    cerr << "Gobal attr userName: " << _userName << endl;
    cerr << "Gobal attr gitCommit: " << _gitCommit << endl;
    cerr << "Gobal attr hsrlVersion: " << _hsrlVersion << endl;
    cerr << "Gobal attr sourceSoftware: " << _sourceSoftware << endl;
    
  }

  return 0;

}

///////////////////////////////////
// read the times

int RawFile::_readTimes()

{

  _dataTimes.clear();
  _dTimes.clear();

  // read the time variable

  _timeVar = _file.getNcFile()->get_var("DATA_time");
  if (_timeVar == NULL) {
    _addErrStr("ERROR - RawFile::_readTimes");
    _addErrStr("  Cannot find DATA_time variable");
    _addErrStr(_file.getNcError()->get_errmsg());
    return -1;
  }
  if (_timeVar->num_dims() < 2) {
    _addErrStr("ERROR - RawFile::_readTimes");
    _addErrStr("  time variable has no dimensions");
    return -1;
  }
  NcDim *timeDim = _timeVar->get_dim(0);
  NcDim *timeVecDim = _timeVar->get_dim(1);
  if (timeDim != _timeDim || timeVecDim != _timeVecDim) {
    _addErrStr("ERROR - RawFile::_readTimes");
    _addErrStr("  DATA_time has incorrect dimensions");
    _addErrStr("  Should be (time, time_vector)");
    return -1;
  }

  // read in time 2D array

  short *timeData = new short[_nTimesInFile * _timeVecSize];
  if (!_timeVar->get(timeData, _nTimesInFile, _timeVecSize)) {
    _addErrStr("ERROR - RawFile::_readTimes");
    _addErrStr("  Cannot read DATA_time 2D array");
    _addErrStr(_file.getNcError()->get_errmsg());
    delete[] timeData;
    return -1;
  }

  for (size_t ii = 0; ii < _nTimesInFile; ii++) {
    short *tdat = timeData + ii * _timeVecSize;
    int year = tdat[0];
    int month = tdat[1];
    int day = tdat[2];
    int hour = tdat[3];
    int min = tdat[4];
    int sec = tdat[5];
    int milliSec = tdat[6];
    int microSec = tdat[7];
    double fracSec = milliSec / 1.0e3 + microSec / 1.0e6;
    RadxTime thisTime(year, month, day, hour, min, sec, fracSec);
    if (_params.debug >= Params::DEBUG_EXTRA) {
      cerr << "  Ray time: " << thisTime.asString(6) << endl;
    }
    _dataTimes.push_back(thisTime);
    _dTimes.push_back(thisTime.asDouble());
  }
  
  delete[] timeData;
  return 0;

}

///////////////////////////////////
// clear the ray variables

void RawFile::_clearRayVariables()

{

  _telescopeLocked.clear();
  _telescopeDirection.clear();
  _latitude.clear();
  _longitude.clear();
  _altitude.clear();
  _heading.clear();
  _gndSpeed.clear();
  _vertVel.clear();
  _pitch.clear();
  _roll.clear();

}

///////////////////////////////////
// read in ray variables

int RawFile::_readRayVariables()

{

  _clearRayVariables();
  int iret = 0;

  _readRayVar(_telescopeLockedVar, "TelescopeLocked", _telescopeLocked);
  if (_telescopeLocked.size() < _nTimesInFile) {
    _addErrStr("ERROR - TelescopeLocked variable required");
    iret = -1;
  }

  _readRayVar(_telescopeDirectionVar, "TelescopeDirection", _telescopeDirection);
  if (_telescopeDirection.size() < _nTimesInFile) {
    _addErrStr("ERROR - TelescopeDirection variable required");
    iret = -1;
  }

  _readRayVar(_latitudeVar, "iwg1_Lat", _latitude);
  if (_latitude.size() < _nTimesInFile) {
    _addErrStr("ERROR - iwg1_Lat variable required");
    iret = -1;
  }

  _readRayVar(_longitudeVar, "iwg1_Lon", _longitude);
  if (_longitude.size() < _nTimesInFile) {
    _addErrStr("ERROR - iwg1_Lon variable required");
    iret = -1;
  }

  _readRayVar(_altitudeVar, "iwg1_GPS_MSL_Alt", _altitude);
  if (_altitude.size() < _nTimesInFile) {
    _addErrStr("ERROR - iwg1_GPS_MSL_Alt variable required");
    iret = -1;
  }

  _readRayVar(_altitudeVar, "iwg1_True_Hdg", _heading);
  if (_heading.size() < _nTimesInFile) {
    _addErrStr("ERROR - iwg1_True_Hdg variable required");
    iret = -1;
  }

  _readRayVar(_gndSpeedVar, "iwg1_Grnd_Spd", _gndSpeed, false);
  _readRayVar(_vertVelVar, "iwg1_Vert_Velocity", _vertVel, false);

  _readRayVar(_pitchVar, "iwg1_Pitch", _pitch);
  if (_pitch.size() < _nTimesInFile) {
    _addErrStr("ERROR - iwg1_Pitch variable required");
    iret = -1;
  }

  _readRayVar(_rollVar, "iwg1_Roll", _roll);
  if (_roll.size() < _nTimesInFile) {
    _addErrStr("ERROR - iwg1_Roll variable required");
    iret = -1;
  }

  _readRayVar(_pollAngleVar, "polarization", _polAngle);
  _readRayVar(_totalEnergyVar, "total_energy", _totalEnergy);

  if (iret) {
    _addErrStr("ERROR - RawFile::_readRayVariables");
    return -1;
  }

  return 0;

}

///////////////////////////////////
// read a ray variable - double
// side effects: set var, vals

int RawFile::_readRayVar(NcVar* &var, const string &name,
                         vector<double> &vals, bool required)
  
{

  vals.clear();

  // get var

  var = _getRayVar(name, required);
  if (var == NULL) {
    if (!required) {
      for (size_t ii = 0; ii < _nTimesInFile; ii++) {
        vals.push_back(Radx::missingMetaDouble);
      }
      clearErrStr();
      return 0;
    } else {
      _addErrStr("ERROR - RawFile::_readRayVar");
      return -1;
    }
  }

  // load up data

  double *data = new double[_nTimesInFile];
  double *dd = data;
  int iret = 0;
  if (var->get(data, _nTimesInFile)) {
    for (size_t ii = 0; ii < _nTimesInFile; ii++, dd++) {
      vals.push_back(*dd);
    }
  } else {
    if (!required) {
      for (size_t ii = 0; ii < _nTimesInFile; ii++) {
        vals.push_back(Radx::missingMetaDouble);
      }
      clearErrStr();
    } else {
      _addErrStr("ERROR - RawFile::_readRayVar");
      _addErrStr("  Cannot read variable: ", name);
      _addErrStr(_file.getNcError()->get_errmsg());
      iret = -1;
    }
  }
  delete[] data;
  return iret;

}

///////////////////////////////////
// read a ray variable - float
// side effects: set var, vals

int RawFile::_readRayVar(NcVar* &var, const string &name,
                         vector<float> &vals, bool required)
  
{

  vals.clear();

  // get var

  var = _getRayVar(name, required);
  if (var == NULL) {
    if (!required) {
      for (size_t ii = 0; ii < _nTimesInFile; ii++) {
        vals.push_back(Radx::missingMetaFloat);
      }
      clearErrStr();
      return 0;
    } else {
      _addErrStr("ERROR - RawFile::_readRayVar");
      return -1;
    }
  }

  // load up data

  float *data = new float[_nTimesInFile];
  float *dd = data;
  int iret = 0;
  if (var->get(data, _nTimesInFile)) {
    for (size_t ii = 0; ii < _nTimesInFile; ii++, dd++) {
      vals.push_back(*dd);
    }
  } else {
    if (!required) {
      for (size_t ii = 0; ii < _nTimesInFile; ii++) {
        vals.push_back(Radx::missingMetaFloat);
      }
      clearErrStr();
    } else {
      _addErrStr("ERROR - RawFile::_readRayVar");
      _addErrStr("  Cannot read variable: ", name);
      _addErrStr(_file.getNcError()->get_errmsg());
      iret = -1;
    }
  }
  delete[] data;
  return iret;

}

///////////////////////////////////
// read a ray variable - double
// side effects: set vals only

int RawFile::_readRayVar(const string &name,
                         vector<double> &vals, bool required)
{
  NcVar *var;
  return _readRayVar(var, name, vals, required);
}

///////////////////////////////////
// read a ray variable - integer
// side effects: set var, vals

int RawFile::_readRayVar(NcVar* &var, const string &name,
                         vector<int> &vals, bool required)
  
{

  vals.clear();

  // get var
  
  var = _getRayVar(name, required);
  if (var == NULL) {
    if (!required) {
      for (size_t ii = 0; ii < _nTimesInFile; ii++) {
        vals.push_back(Radx::missingMetaInt);
      }
      clearErrStr();
      return 0;
    } else {
      _addErrStr("ERROR - RawFile::_readRayVar");
      return -1;
    }
  }

  // load up data

  int *data = new int[_nTimesInFile];
  int *dd = data;
  int iret = 0;
  if (var->get(data, _nTimesInFile)) {
    for (size_t ii = 0; ii < _nTimesInFile; ii++, dd++) {
      vals.push_back(*dd);
    }
  } else {
    if (!required) {
      for (size_t ii = 0; ii < _nTimesInFile; ii++) {
        vals.push_back(Radx::missingMetaInt);
      }
      clearErrStr();
    } else {
      _addErrStr("ERROR - RawFile::_readRayVar");
      _addErrStr("  Cannot read variable: ", name);
      _addErrStr(_file.getNcError()->get_errmsg());
      iret = -1;
    }
  }
  delete[] data;
  return iret;

}

///////////////////////////////////
// read a ray variable - integer
// side effects: set vals only

int RawFile::_readRayVar(const string &name,
                         vector<int> &vals, bool required)
{
  NcVar *var;
  return _readRayVar(var, name, vals, required);
}

///////////////////////////////////////////
// read a ray variable - boolean
// side effects: set var, vals

int RawFile::_readRayVar(NcVar* &var, const string &name,
                         vector<bool> &vals, bool required)
  
{
  
  vals.clear();
  
  // get var
  
  var = _getRayVar(name, false);
  if (var == NULL) {
    if (!required) {
      for (size_t ii = 0; ii < _nTimesInFile; ii++) {
        vals.push_back(false);
      }
      clearErrStr();
      return 0;
    } else {
      _addErrStr("ERROR - RawFile::_readRayVar");
      return -1;
    }
  }

  // load up data
  
  int *data = new int[_nTimesInFile];
  int *dd = data;
  int iret = 0;
  if (var->get(data, _nTimesInFile)) {
    for (size_t ii = 0; ii < _nTimesInFile; ii++, dd++) {
      if (*dd == 0) {
        vals.push_back(false);
      } else {
        vals.push_back(true);
      }
    }
  } else {
    for (size_t ii = 0; ii < _nTimesInFile; ii++) {
      vals.push_back(false);
    }
    clearErrStr();
  }
  delete[] data;
  return iret;

}

///////////////////////////////////
// read a ray variable - boolean
// side effects: set vals only

int RawFile::_readRayVar(const string &name,
                         vector<bool> &vals, bool required)
{
  NcVar *var;
  return _readRayVar(var, name, vals, required);
}

///////////////////////////////////
// get a ray variable by name
// returns NULL on failure

NcVar* RawFile::_getRayVar(const string &name, bool required)

{

  // get var
  
  NcVar *var = _file.getNcFile()->get_var(name.c_str());
  if (var == NULL) {
    if (required) {
      _addErrStr("ERROR - RawFile::_getRayVar");
      _addErrStr("  Cannot read variable, name: ", name);
      _addErrStr(_file.getNcError()->get_errmsg());
    }
    return NULL;
  }

  // check time dimension
  
  if (var->num_dims() < 1) {
    if (required) {
      _addErrStr("ERROR - RawFile::_getRayVar");
      _addErrStr("  variable name: ", name);
      _addErrStr("  variable has no dimensions");
    }
    return NULL;
  }
  NcDim *timeDim = var->get_dim(0);
  if (timeDim != _timeDim) {
    if (required) {
      _addErrStr("ERROR - RawFile::_getRayVar");
      _addErrStr("  variable name: ", name);
      _addErrStr("  variable has incorrect dimension, dim name: ", 
                 timeDim->name());
      _addErrStr("  should be: ", "time");
    }
    return NULL;
  }

  return var;

}

///////////////////////////////////
// create the rays to be read in
// and set meta data

int RawFile::_createRays(const string &path)

{

  // compile a list of the rays to be read in
  
  _rays.clear();
  RadxCfactors corr;

  for (size_t ii = 0; ii < _nTimesInFile; ii++) {

    // new ray

    RadxRay *ray = new RadxRay;
    ray->setRangeGeom(_startRangeKm, _gateSpacingKm);
    ray->setTime(_dataTimes[ii]);
    
    // sweep info

    ray->setVolumeNumber(-9999);
    ray->setSweepNumber(0);
    ray->setSweepMode(Radx::SWEEP_MODE_POINTING);
    ray->setPrtMode(Radx::PRT_MODE_FIXED);
    ray->setTargetScanRateDegPerSec(0.0);
    ray->setIsIndexed(false);

    // georeference
    
    RadxGeoref geo;

    if (_telescopeDirection[ii] == 1) {

      // pointing up

      geo.setRotation(-4.0);
      geo.setTilt(0.0);
      ray->setAzimuthDeg(0.0);
      ray->setElevationDeg(90.0);
      ray->setFixedAngleDeg(90.0);
      
    } else {
      
      // pointing down
      
      geo.setRotation(184.0);
      geo.setTilt(0.0);
      ray->setAzimuthDeg(0.0);
      ray->setElevationDeg(-90.0);
      ray->setFixedAngleDeg(-90.0);
      
    }
    
    geo.setRoll(_roll[ii]);
    geo.setPitch(_pitch[ii]);
    geo.setHeading(_heading[ii]);
    geo.setDrift(0.0); // do not have drift in the file
    
    geo.setLatitude(_latitude[ii]);
    geo.setLongitude(_longitude[ii]);
    geo.setAltitudeKmMsl(_altitude[ii] / 1000.0);

    geo.setVertVelocity(_vertVel[ii]);

    double sinVal, cosVal;
    sincos(_heading[ii] * Radx::DegToRad, &sinVal, &cosVal);
    geo.setEwVelocity(_gndSpeed[ii] * sinVal);
    geo.setNsVelocity(_gndSpeed[ii] * cosVal);
    
    ray->setGeoref(geo);

    // compute az/el from geo
    
    double azimuth, elevation;
    _computeRadarAngles(geo, corr, azimuth, elevation);
    ray->setAzimuthDeg(azimuth);
    ray->setElevationDeg(elevation);

    // other metadata - overloading
    
    ray->setMeasXmitPowerDbmH(_totalEnergy[ii]);
    ray->setEstimatedNoiseDbmHc(_polAngle[ii]);
    
    ray->setNSamples(2000);
    // hard coded 2000 as replacement for DATA_shot_count from raw file
    
    // add to ray vector
    
    _rays.push_back(ray);

  } // ii

  return 0;

}

/////////////////////////////////////////////////////////
// load up the read volume with the data from this object

void RawFile::_loadReadVolume()
{

  _readVol->clear();

  _readVol->setOrigFormat("HSRL-RAW");
  _readVol->setVolumeNumber(-9999);
  _readVol->setInstrumentType(_instrumentType);
  _readVol->setInstrumentName("HSRL");
  _readVol->setSiteName("GV");
  _readVol->setPlatformType(_platformType);
  _readVol->setPrimaryAxis(_primaryAxis);
  
  _readVol->addFrequencyHz(Radx::LIGHT_SPEED / 538.0e-9);
  _readVol->addFrequencyHz(Radx::LIGHT_SPEED / 1064.0e-9);
  
  _readVol->setLidarConstant(-9999.0);
  _readVol->setLidarPulseEnergyJ(-9999.0);
  _readVol->setLidarPeakPowerW(-9999.0);
  _readVol->setLidarApertureDiamCm(-9999.0);
  _readVol->setLidarApertureEfficiency(-9999.0);
  _readVol->setLidarFieldOfViewMrad(-9999.0);
  _readVol->setLidarBeamDivergenceMrad(-9999.0);

  _readVol->setTitle("NCAR HSRL");
  _readVol->setSource("HSRL realtime software");
  _readVol->setHistory("Converted from RAW NetCDF files");
  _readVol->setInstitution("NCAR");
  _readVol->setReferences("University of Wisconsin");
  _readVol->setComment("");
  _readVol->setDriver("Hsrl2Radx");
  _readVol->setCreated(_dataAdded);
  _readVol->setStatusXml("");
  
  _readVol->setScanName("Vert");
  _readVol->setScanId(0);

  if (_latitude.size() > 0) {
    for (size_t ii = 0; ii < _latitude.size(); ii++) {
      if (_latitude[ii] > -9990) {
        _readVol->setLatitudeDeg(_latitude[ii]);
        break;
      }
    }
  }
  if (_longitude.size() > 0) {
    for (size_t ii = 0; ii < _longitude.size(); ii++) {
      if (_longitude[ii] > -9990) {
        _readVol->setLongitudeDeg(_longitude[ii]);
        break;
      }
    }
  }
  if (_altitude.size() > 0) {
    for (size_t ii = 0; ii < _altitude.size(); ii++) {
      if (_altitude[ii] > -9990) {
        _readVol->setAltitudeKm(_altitude[ii] / 1000.0);
        break;
      }
    }
  }

  _readVol->setRangeGeom(_startRangeKm, _gateSpacingKm);

  for (size_t ii = 0; ii < _rays.size(); ii++) {
    _readVol->addRay(_rays[ii]);
  }

  // memory responsibility has passed to the volume object, so clear
  // the vectors without deleting the objects to which they point

  _rays.clear();

  // load the sweep information from the rays

  _readVol->loadSweepInfoFromRays();
  
  // load the volume information from the rays

  _readVol->loadVolumeInfoFromRays();
  
}

////////////////////////////////////////////
// read the field variables

int RawFile::_readFieldVariables()

{

  // loop through the variables, adding data fields as appropriate
  
  for (int ivar = 0; ivar < _file.getNcFile()->num_vars(); ivar++) {
    
    NcVar* var = _file.getNcFile()->get_var(ivar);
    if (var == NULL) {
      continue;
    }
    
    int numDims = var->num_dims();
    // we need fields with 2 dimensions
    if (numDims != 2) {
      continue;
    }

    // check that we have the correct dimensions
    NcDim* timeDim = var->get_dim(0);
    NcDim* bincountDim = var->get_dim(1);
    if (timeDim != _timeDim || bincountDim != _binCountDim) {
      continue;
    }
    
    // check the type
    NcType ftype = var->type();
    if (ftype != ncInt) {
      // not a valid type for field data
      continue;
    }

    // set names, units, etc

    string name = var->name();
    if (_params.debug >= Params::DEBUG_VERBOSE) {
      cerr << "DEBUG - RawFile::_readFieldVariables" << endl;
      cerr << "  -->> adding field: " << name << endl;
    }

    string longName;
    NcAtt *longNameAtt = var->get_att("long_name");
    if (longNameAtt != NULL) {
      longName = NetcdfClassic::asString(longNameAtt);
      delete longNameAtt;
    }
    
    string units = "counts";

    // load in the data

    if (_addCountFieldToRays(var, name, units, longName)) {
      _addErrStr("ERROR - RawFile::_readFieldVariables");
      _addErrStr("  cannot read field name: ", name);
      _addErrStr(_file.getNcError()->get_errmsg());
      return -1;
    }

  } // ivar

  return 0;

}

//////////////////////////////////////////////////////////////
// Add si32 field to rays
// The _rays array has previously been set up by _createRays()
// Returns 0 on success, -1 on failure

int RawFile::_addCountFieldToRays(NcVar* var,
                                  const string &name,
                                  const string &units,
                                  const string &longName)
  
{

  // get int data from array
  
  RadxArray<Radx::si32> idata_;
  Radx::si32 *idata = idata_.alloc(_nPoints);
  int iret = !var->get(idata, _nTimesInFile, _nBinsInFile);
  if (iret) {
    return -1;
  }
  // for (size_t ii = 0; ii < _nPoints; ii++) {
  //   idata[ii] = ii % 50;
  // }

  // set up float array

  RadxArray<Radx::fl32> fcounts_;
  Radx::fl32 *fcounts = fcounts_.alloc(_nGates);
  
  // loop through the rays
  
  for (size_t iray = 0; iray < _rays.size(); iray++) {

    // get int counts for ray
    
    int startIndex = iray * _nBinsInFile;
    Radx::si32 *icounts = idata + startIndex;

    // compute mean counts per gate

    size_t ibin = 0;
    for (size_t igate = 0; igate < _nGates; igate++) {
      fcounts[igate] = 0.0;
      for (size_t ii = 0; ii < _nBinsPerGate; ii++, ibin++) {
        fcounts[igate] += icounts[ibin];
      }
      //fcounts[igate] /= (double) _nBinsPerGate;

    }
    
    RadxField *field =
      _rays[iray]->addField(name, units, _nGates,
                            Radx::missingFl32,
                            fcounts,
                            true);
    
    field->setLongName(longName);
    field->setRangeGeom(_startRangeKm, _gateSpacingKm);
    
    // add db of same field
    
    for (size_t igate = 0; igate < _nGates; igate++) {
      if (fcounts[igate] > 0) {
        fcounts[igate] = 10.0 * log10(fcounts[igate]);
      } else {
        fcounts[igate] = Radx::missingFl32;
      }
    }

    string dbName = "db_";
    dbName += name;
    string dbUnits = "db_counts";
    string dbLongName = "db_";
    dbLongName += longName;
    
    RadxField *dbField =
      _rays[iray]->addField(dbName, dbUnits, _nGates,
                            Radx::missingFl32,
                            fcounts,
                            true);
    
    dbField->setLongName(dbLongName);
    dbField->setRangeGeom(_startRangeKm, _gateSpacingKm);

  }
  
  return 0;
  
}


///////////////////////////////////////////////
// add labelled integer value to error string,
// with optional following carriage return.

void RawFile::_addErrInt(string label, int iarg, bool cr)
{
  Radx::addErrInt(_errStr, label, iarg, cr);
}

///////////////////////////////////////////////
// add labelled double value to error string,
// with optional following carriage return.
// Default format is %g.

void RawFile::_addErrDbl(string label, double darg,
                          string format, bool cr)
  
{
  Radx::addErrDbl(_errStr, label, darg, format, cr);
}

////////////////////////////////////////
// add labelled string to error string
// with optional following carriage return.

void RawFile::_addErrStr(string label, string strarg, bool cr)

{
  Radx::addErrStr(_errStr, label, strarg, cr);
}

void RawFile::_clearRays()
{
  for (int ii = 0; ii < (int) _rays.size(); ii++) {
    delete _rays[ii];
  }
  _rays.clear();
}

///////////////////////////////////////////////////////////////////
// compute the true azimuth, elevation, etc. from platform
// parameters using Testud's equations with their different
// definitions of rotation angle, etc.
//
// see Wen-Chau Lee's paper
// "Mapping of the Airborne Doppler Radar Data"

void RawFile::_computeRadarAngles(RadxGeoref &georef,
                                  RadxCfactors &corr,
                                  double &azimuthDeg,
                                  double &elevationDeg)
  
{
  
  double R = (georef.getRoll() + corr.getRollCorr()) * Radx::DegToRad;
  double P = (georef.getPitch() + corr.getPitchCorr()) * Radx::DegToRad;
  double H = (georef.getHeading() + corr.getHeadingCorr()) * Radx::DegToRad;
  double D = (georef.getDrift() + corr.getDriftCorr()) * Radx::DegToRad;
  double T = H + D;
  
  double sinP = sin(P);
  double cosP = cos(P);
  double sinD = sin(D);
  double cosD = cos(D);
  
  double theta_a = 
    (georef.getRotation() + corr.getRotationCorr()) * Radx::DegToRad;
  double tau_a =
    (georef.getTilt() + corr.getTiltCorr()) * Radx::DegToRad;
  double sin_tau_a = sin(tau_a);
  double cos_tau_a = cos(tau_a);
  double sin_theta_rc = sin(theta_a + R); /* roll corrected rotation angle */
  double cos_theta_rc = cos(theta_a + R); /* roll corrected rotation angle */
  
  double xsubt = (cos_theta_rc * sinD * cos_tau_a * sinP
                  + cosD * sin_theta_rc * cos_tau_a
                  -sinD * cosP * sin_tau_a);
  
  double ysubt = (-cos_theta_rc * cosD * cos_tau_a * sinP
                  + sinD * sin_theta_rc * cos_tau_a
                  + cosP * cosD * sin_tau_a);
  
  double zsubt = (cosP * cos_tau_a * cos_theta_rc
                  + sinP * sin_tau_a);
  
  double lambda_t = atan2(xsubt, ysubt);
  double azimuthRad = fmod(lambda_t + T, M_PI * 2.0);
  double elevationRad = asin(zsubt);
  
  elevationDeg = elevationRad * Radx::RadToDeg;
  azimuthDeg = azimuthRad * Radx::RadToDeg;
  if (azimuthDeg < 0) {
    azimuthDeg += 360.0;
  }
  
}

