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
///////////////////////////////////////////////////////////////
// IwrfAcGeoref2Spdb.cc
//
// Mike Dixon, EOL, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// March 2016
//
///////////////////////////////////////////////////////////////
//
// AcGeoref2Spdb reads aircraft georeference data
// (posn, attitude, motion etc) from IWRF time series and
// netcdf files, and writes the data to SPDB.
//
///////////////////////////////////////////////////////////////

#include "AcGeoref2Spdb.hh"
#include "InputUdp.hh"
#include <map>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cerrno>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctime>
#include <toolsa/toolsa_macros.h>
#include <toolsa/TaArray.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/TaStr.hh>
#include <toolsa/str.h>
#include <toolsa/pmu.h>
#include <Radx/RadxFile.hh>
#include <Radx/RadxVol.hh>
#include <physics/thermo.h>

double AcGeoref2Spdb::_missingDbl = NAN;

using namespace std;

// Constructor

AcGeoref2Spdb::AcGeoref2Spdb(int argc, char **argv)
  
{

  isOK = true;
  _pulseCount = 0;
  _totalPulseCount = 0;
  _pulseReader = NULL;
  _prevPulseSeqNum = 0;

  // set programe name
  
  _progName = "AcGeoref2Spdb";

  // get command line args

  if (_args.parse(argc, argv, _progName)) {
    cerr << "ERROR: " << _progName << endl;
    cerr << "Problem with command line args" << endl;
    isOK = false;
    return;
  }

  // get TDRP params
  
  _paramsPath = (char *) "unknown";
  if (_params.loadFromArgs(argc, argv, _args.override.list,
			   &_paramsPath)) {
    cerr << "ERROR: " << _progName << endl;
    cerr << "Problem with TDRP parameters" << endl;
    isOK = false;
    return;
  }
  
  // create the pulse reader
  
  IwrfDebug_t iwrfDebug = IWRF_DEBUG_OFF;
  if (_params.debug >= Params::DEBUG_EXTRA) {
    iwrfDebug = IWRF_DEBUG_VERBOSE;
  } else if (_params.debug >= Params::DEBUG_VERBOSE) {
    iwrfDebug = IWRF_DEBUG_NORM;
  } 
    
  if (_params.input_mode == Params::IWRF_FMQ) {
    _pulseReader = new IwrfTsReaderFmq(_params.input_fmq_name,
				       iwrfDebug,
				       !_params.seek_to_end_of_input);
  } else {
    _pulseReader = new IwrfTsReaderFile(_args.inputFileList, iwrfDebug);
  }

  // init process mapper registration
  
  if (_params.reg_with_procmap) {
    PMU_auto_init((char *) _progName.c_str(),
                  _params.instance,
                  PROCMAP_REGISTER_INTERVAL);
  }
  
}

// destructor

AcGeoref2Spdb::~AcGeoref2Spdb()

{

  if (_pulseReader) {
    delete _pulseReader;
  }

  PMU_auto_unregister();

}

//////////////////////////////////////////////////
// Run

int AcGeoref2Spdb::Run ()
{

  if (_params.input_mode == Params::CFRADIAL) {
    
    return _runCfradialMode();
    
  } else if (_params.input_mode == Params::IWRF_FILE ||
             _params.input_mode == Params::IWRF_FMQ) {
    
    return _runIwrfMode();
    
  } else if (_params.input_mode == Params::RAF_NETCDF) {
    
    return _runRafNetcdfMode();

  } else if (_params.input_mode == Params::RAF_IWG1_UDP) {
    
    return _runRafIwg1UdpMode();

  } else {

    cerr << "ERROR - AcGeoref2Spdb::Run()" << endl;
    cerr << "  Bad input mode: " << _params.input_mode << endl;
    return -1;
    
  }

  return 0;

}

//////////////////////////////////////////////////
// Run in IWRF mode

int AcGeoref2Spdb::_runIwrfMode()
  
{

  iwrf_platform_georef_t iwrfGeoref;
  MEM_zero(iwrfGeoref);
  
  while (true) {
    
    // read next pulse
    
    IwrfTsPulse *pulse = _getNextPulse();
    if (pulse == NULL) {
      return 0;
    }

    // do we have a new georef?
    
    const iwrf_platform_georef_t &georef = pulse->getPlatformGeoref();
    if (iwrf_compare(georef, iwrfGeoref)) {

      // georef packet has changed
      
      iwrfGeoref = georef;

      // convert format

      ac_georef_t acGeoref;
      _convertFromIwrfGeoref(iwrfGeoref, acGeoref);

      // add chunk to spdb

      string callsign(_params.aircraft_callsign);
      string callsignLast4(callsign.substr(callsign.size() - 4));
      si32 dataType = Spdb::hash4CharsToInt32(callsignLast4.c_str());
      si32 dataType2 = acGeoref.time_nano_secs;
      time_t validTime = acGeoref.time_secs_utc;
      
      ac_georef_t copy = acGeoref;
      BE_from_ac_georef(&copy);
      _spdb.addPutChunk(dataType,
                        validTime,
                        validTime,
                        sizeof(ac_georef_t),
                        &copy,
                        dataType2);

      if (_params.debug >= Params::DEBUG_VERBOSE) {
        if (_spdb.nPutChunks() == 1) {
          fprintf(stderr, "Writing georef for time: %s\n",
                  DateTime::strm(validTime).c_str());
          fprintf(stderr, "url: %s\n", _params.output_spdb_url);
          ac_georef_print(stderr, " ", &acGeoref);
        }
      }

      // write out when we have enough chunks

      if (_spdb.nPutChunks() > 1000) {
        _doWrite();
      }

    } // if (iwrf_compare(georef, iwrfGeoref)) 
    
    delete pulse;
    
  } // while

  // write any remaining chunks

  _doWrite();

}

//////////////////////////////////////////////////
// Run in CFRADIAL mode

int AcGeoref2Spdb::_runCfradialMode()
  
{

  int iret = 0;
  
  for (int ii = 0; ii < (int) _args.inputFileList.size(); ii++) {
    string inputPath = _args.inputFileList[ii];
    if (_processCfRadialFile(inputPath)) {
      cerr << "ERROR - AcGeoref2Spdb::_runRafNetcdfMode()" << endl;
      cerr << "  Reading file path: " << inputPath << endl;
      iret = -1;
    }
  }

  return iret;

}

//////////////////////////////////////////////////
// Run in RAF NETCDF mode

int AcGeoref2Spdb::_runRafNetcdfMode()
  
{

  // create custom fields as required

  if (_params.nc_specify_custom_fields) {
    for (int ii = 0; ii < _params.nc_varname_custom_fields_n; ii++) {
      TaArray<double> array;
      _customFields.push_back(array);
    }
  }

  int iret = 0;

  for (int ii = 0; ii < (int) _args.inputFileList.size(); ii++) {
    string inputPath = _args.inputFileList[ii];
    if (_processRafNetcdfFile(inputPath)) {
      cerr << "ERROR - AcGeoref2Spdb::_runRafNetcdfMode()" << endl;
      cerr << "  Reading file path: " << inputPath << endl;
      iret = -1;
    }
  }

  return iret;

}

//////////////////////////////////////////////////
// Run in RAF IWG1 UDP mode

int AcGeoref2Spdb::_runRafIwg1UdpMode()
  
{

  // open socket

  int iret = 0;
  InputUdp udp;
  if (udp.openUdp(_params.iwg1_udp_port, 
                  _params.debug >= Params::DEBUG_NORM)) {
    cerr << "ERROR - AcGeoref2Spdb::_runRafIwg1UdpMode()" << endl;
    cerr << "  Cannot open UDP, port: " << _params.iwg1_udp_port << endl;
    return -1;
  }
  
  while (true) {

    if (udp.readPacket() < 0) {
      // close socket
      udp.closeUdp();
      // reopen socket
      if (udp.openUdp(_params.iwg1_udp_port, 
                      _params.debug >= Params::DEBUG_NORM)) {
        cerr << "ERROR - AcGeoref2Spdb::_runRafIwg1UdpMode()" << endl;
        cerr << "  Cannot open UDP, port: " << _params.iwg1_udp_port << endl;
        return -1;
      }
    }

    if (_params.debug >= Params::DEBUG_VERBOSE) {
      cerr << "Got IWG1 packet:" << endl;
      cerr << "  len: " << udp.getLen() << endl;
      cerr << "  contents: " << udp.getBuf() << endl;
    }

    _handleIwg1(udp.getBuf(), udp.getLen());

  } // while

  return iret;

}

////////////////////////////
// get next pulse
//
// returns NULL on failure

IwrfTsPulse *AcGeoref2Spdb::_getNextPulse() 

{
  
  IwrfTsPulse *pulse = NULL;
  
  while (pulse == NULL) {
    pulse = _pulseReader->getNextPulse(false);
    if (pulse == NULL) {
      if (_pulseReader->getTimedOut()) {
	cout << "# NOTE: read timed out, retrying ..." << endl;
	continue;
      }
      if (_pulseReader->endOfFile()) {
        cout << "# NOTE: end of file encountered" << endl;
      }
      return NULL;
    }
  }

  if (_pulseReader->endOfFile()) {
    cout << "# NOTE: end of file encountered" << endl;
  }

  // check for missing pulses
  
  si64 pulseSeqNum = pulse->getSeqNum();
  si64 nMissing = (pulseSeqNum - _prevPulseSeqNum) - 1;
  if (_params.debug >= Params::DEBUG_VERBOSE &&
      _prevPulseSeqNum != 0 && nMissing != 0) {
    cerr << "WARNING - AcGeoref2Spdb - n missing pulses: " << nMissing << endl;
    cerr << "  prev pulse seq num: " << _prevPulseSeqNum << endl;
    cerr << "  this pulse seq num: " << pulseSeqNum << endl;
    cerr << "  file: " << _pulseReader->getPathInUse() << endl;
  }
  _prevPulseSeqNum = pulseSeqNum;
  
  return pulse;

}

////////////////////////////////////////
// convert to ac_georef from iwrf georef

void AcGeoref2Spdb::_convertFromIwrfGeoref(const iwrf_platform_georef_t &iwrfGeoref,
                                           ac_georef_t &acGeoref)

{

  ac_georef_init(&acGeoref);

  acGeoref.time_secs_utc = iwrfGeoref.packet.time_secs_utc;
  acGeoref.time_nano_secs = iwrfGeoref.packet.time_nano_secs;

  acGeoref.longitude = iwrfGeoref.longitude;
  acGeoref.latitude = iwrfGeoref.latitude;
  acGeoref.altitude_msl_km = iwrfGeoref.altitude_msl_km;
  acGeoref.altitude_agl_km = iwrfGeoref.altitude_agl_km;
  acGeoref.ew_velocity_mps = iwrfGeoref.ew_velocity_mps;
  acGeoref.ns_velocity_mps = iwrfGeoref.ns_velocity_mps;
  acGeoref.vert_velocity_mps = iwrfGeoref.vert_velocity_mps;
  acGeoref.ew_horiz_wind_mps = iwrfGeoref.ew_horiz_wind_mps;
  acGeoref.ns_horiz_wind_mps = iwrfGeoref.ns_horiz_wind_mps;
  acGeoref.vert_wind_mps = iwrfGeoref.vert_wind_mps;
  acGeoref.heading_deg = iwrfGeoref.heading_deg;
  acGeoref.drift_angle_deg = iwrfGeoref.drift_angle_deg;
  acGeoref.track_deg = iwrfGeoref.track_deg;
  acGeoref.roll_deg = iwrfGeoref.roll_deg;
  acGeoref.pitch_deg = iwrfGeoref.pitch_deg;
  acGeoref.heading_rate_dps = iwrfGeoref.heading_rate_dps;
  acGeoref.pitch_rate_dps = iwrfGeoref.pitch_rate_dps;
  acGeoref.roll_rate_dps = iwrfGeoref.roll_rate_dps;
  acGeoref.drive_angle_1_deg = iwrfGeoref.drive_angle_1_deg;
  acGeoref.drive_angle_2_deg = iwrfGeoref.drive_angle_2_deg;

  STRncopy(acGeoref.callsign, _params.aircraft_callsign, AC_GEOREF_NBYTES_CALLSIGN);

  if (_params.debug >= Params::DEBUG_EXTRA) {
    ac_georef_print(stderr, " ", &acGeoref);
  }

}

////////////////////////////////////////
// convert to ac_georef from RadxGeoref

void AcGeoref2Spdb::_convertFromRadxGeoref(const RadxGeoref &radxGeoref,
                                           ac_georef_t &acGeoref)

{

  ac_georef_init(&acGeoref);

  acGeoref.time_secs_utc = radxGeoref.getTimeSecs();
  acGeoref.time_nano_secs = radxGeoref.getNanoSecs();

  acGeoref.longitude = radxGeoref.getLongitude();
  acGeoref.latitude = radxGeoref.getLatitude();
  acGeoref.altitude_msl_km = radxGeoref.getAltitudeKmMsl();
  acGeoref.altitude_agl_km = radxGeoref.getAltitudeKmAgl();
  acGeoref.ew_velocity_mps = radxGeoref.getEwVelocity();
  acGeoref.ns_velocity_mps = radxGeoref.getNsVelocity();
  acGeoref.vert_velocity_mps = radxGeoref.getVertVelocity();
  acGeoref.ew_horiz_wind_mps = radxGeoref.getEwWind();
  acGeoref.ns_horiz_wind_mps = radxGeoref.getNsWind();
  acGeoref.vert_wind_mps = radxGeoref.getVertWind();
  acGeoref.heading_deg = radxGeoref.getHeading();
  acGeoref.drift_angle_deg = radxGeoref.getDrift();
  acGeoref.track_deg = radxGeoref.getTrack();
  acGeoref.roll_deg = radxGeoref.getRoll();
  acGeoref.pitch_deg = radxGeoref.getPitch();
  acGeoref.heading_rate_dps = radxGeoref.getHeadingRate();
  acGeoref.pitch_rate_dps = radxGeoref.getPitchRate();
  acGeoref.roll_rate_dps = radxGeoref.getRollRate();
  acGeoref.drive_angle_1_deg = radxGeoref.getDriveAngle1();
  acGeoref.drive_angle_2_deg = radxGeoref.getDriveAngle2();

  STRncopy(acGeoref.callsign, _params.aircraft_callsign, AC_GEOREF_NBYTES_CALLSIGN);

}

//////////////////////////////////////////////////
// Process CFRADIAL file

int AcGeoref2Spdb::_processCfRadialFile(const string &path)
  
{
  
  if (_params.debug) {
    cerr << "Reading CfRadial file: " << path << endl;
  }

  // set up the read

  RadxFile inFile;
  if (_params.debug >= Params::DEBUG_VERBOSE) {
    inFile.setDebug(true);
  }
  if (_params.debug >= Params::DEBUG_EXTRA) {
    inFile.setVerbose(true);
  }
  inFile.addReadField(_params.cfradial_dbz_field_name);
  inFile.addReadField(_params.cfradial_vel_field_name);
  if (_params.debug >= Params::DEBUG_EXTRA) {
    inFile.printReadRequest(cerr);
  }

  // read in file

  RadxVol vol;
  if (inFile.readFromPath(path, vol)) {
    cerr << "ERROR - AcGeoref2Spdb::_processRafNetcdfFile" << endl;
    cerr << "  path: " << path << endl;
    cerr << inFile.getErrStr() << endl;
    return -1;
  }

  // get temps from status xml if available

  double cmigitsTemp = _missingDbl;
  double tailconeTemp = _missingDbl;
  if (_params.get_hcr_temps_from_cfradial) {
    _getHcrTempsFromStatusXml(vol.getStatusXml(), 
                              cmigitsTemp, tailconeTemp);
  }

  // clear spdb

  _spdb.clearPutChunks();

  // loop through rays

  const vector<RadxRay *> rays = vol.getRays();
  for (size_t iray = 0; iray < rays.size(); iray++) {

    // get ray
    RadxRay &ray = *rays[iray];

    // get georef for ray
    const RadxGeoref *radxGeoref = ray.getGeoreference();
    if (radxGeoref == NULL) {
      continue;
    }

    // convert to ac_georef
    ac_georef_t acGeoref;
    _convertFromRadxGeoref(*radxGeoref, acGeoref);

    // optionally add in HCR temps
    if (_params.get_hcr_temps_from_cfradial) {
      acGeoref.temp_c = cmigitsTemp;
      acGeoref.custom[0] = tailconeTemp;
    }

    // optionally add in surface vel
    if (_params.compute_surface_vel_in_cfradial) {
      double surfaceVel = _computeSurfaceVel(ray);
      acGeoref.custom[1] = surfaceVel;
    }

    if (_params.debug >= Params::DEBUG_EXTRA) {
      cerr << "=======================================" << endl;
      ac_georef_print(stderr, " ", &acGeoref);
    }

    acGeoref.custom[2] = ray.getAzimuthDeg();
    acGeoref.custom[3] = ray.getElevationDeg();
    acGeoref.custom[4] = radxGeoref->getRotation();
    acGeoref.custom[5] = radxGeoref->getTilt();

    // add to spdb
    string callsign(_params.aircraft_callsign);
    string callsignLast4(callsign.substr(callsign.size() - 4));
    si32 dataType = Spdb::hash4CharsToInt32(callsignLast4.c_str());
    si32 dataType2 = acGeoref.time_nano_secs;
    time_t validTime = acGeoref.time_secs_utc;
    ac_georef_t copy = acGeoref;
    BE_from_ac_georef(&copy);
    _spdb.addPutChunk(dataType,
                      validTime,
                      validTime,
                      sizeof(ac_georef_t),
                      &copy,
                      dataType2);
    if (_params.debug >= Params::DEBUG_VERBOSE) {
      if (_spdb.nPutChunks() == 1) {
        fprintf(stderr, "Writing georef for time: %s\n",
                DateTime::strm(validTime).c_str());
        fprintf(stderr, "url: %s\n", _params.output_spdb_url);
        ac_georef_print(stderr, " ", &acGeoref);
      }
    }

  } // iray

  // write out to data base
  
  _doWrite();

  return 0;

}

//////////////////////////////////////////////////
// Process RAF NETCDF file

int AcGeoref2Spdb::_processRafNetcdfFile(const string &path)
  
{

  if (_params.debug) {
    cerr << "Reading RAF netcdf file: " << path << endl;
  }

  // open file

  if (_file.openRead(path)) {
    cerr << "ERROR - AcGeoref2Spdb::_processRafNetcdfFile()" << endl;
    cerr << "  Reading file: " << path << endl;
    cerr << _file.getErrStr() << endl;
    return -1;
  }

  // read global attributes

  if (_readGlobalAttr()) {
    cerr << "ERROR - AcGeoref2Spdb::_processRafNetcdfFile()" << endl;
    cerr << "  Cannot read global attributes" << endl;
    return -1;
  }

  if (_params.debug) {
    cerr << "  _callSign: " << _callSign << endl;
    cerr << "  _projectName: " << _projectName << endl;
    cerr << "  _flightNum: " << _flightNum << endl;
  }

  // read in times

  if (_readTimes()) {
    cerr << "ERROR - AcGeoref2Spdb::_processRafNetcdfFile()" << endl;
    cerr << "  Cannot read times" << endl;
    return -1;
  }

  // read in time series variables

  if (_readTimeSeriesVars()) {
    cerr << "ERROR - AcGeoref2Spdb::_processRafNetcdfFile()" << endl;
    cerr << "  Cannot read time series variables" << endl;
    return -1;
  }

  // clear spdb

  _spdb.clearPutChunks();

  // loop through times

  time_t flight_time_start = -1;

  for (size_t itime = 0; itime < _nTimes; itime++) {
    
    // init
    
    ac_georef_t acGeoref;
    ac_georef_init(&acGeoref);
    
    // fill out struct

    acGeoref.time_secs_utc = _times[itime];
    acGeoref.time_nano_secs = 0;

    acGeoref.longitude = _lon.dat()[itime];
    acGeoref.latitude = _lat.dat()[itime];
    acGeoref.altitude_msl_km = _altMsl.dat()[itime] / 1000.0;
    acGeoref.altitude_agl_km = _missingDbl;
    acGeoref.altitude_pres_m = _altPres.dat()[itime];
    acGeoref.ew_velocity_mps = _ewVel.dat()[itime];
    acGeoref.ns_velocity_mps = _nsVel.dat()[itime];
    acGeoref.vert_velocity_mps = _vertVel.dat()[itime];
    acGeoref.ew_horiz_wind_mps = _ewWind.dat()[itime];
    acGeoref.ns_horiz_wind_mps = _nsWind.dat()[itime];
    acGeoref.vert_wind_mps = _vertWind.dat()[itime];
    acGeoref.heading_deg = _heading.dat()[itime];
    acGeoref.drift_angle_deg = _drift.dat()[itime];
    acGeoref.track_deg = _track.dat()[itime];
    acGeoref.roll_deg = _roll.dat()[itime];
    acGeoref.pitch_deg = _pitch.dat()[itime];
    acGeoref.heading_rate_dps = _missingDbl;
    acGeoref.pitch_rate_dps = _missingDbl;
    acGeoref.roll_rate_dps = _missingDbl;
    acGeoref.drive_angle_1_deg = _missingDbl;
    acGeoref.drive_angle_2_deg = _missingDbl;
    acGeoref.temp_c = _temp.dat()[itime];
    acGeoref.pressure_hpa = _pressure.dat()[itime];
    acGeoref.rh_percent = _rh.dat()[itime];
    double density = PHYprestemp2density(acGeoref.pressure_hpa, 
                                         acGeoref.temp_c + 273.15);
    acGeoref.density_kg_m3 = density;
    acGeoref.angle_of_attack_deg = _aoa.dat()[itime];
    acGeoref.ind_airspeed_mps = _ias.dat()[itime];
    acGeoref.true_airspeed_mps = _tas.dat()[itime];
    acGeoref.accel_normal = _accelNorm.dat()[itime];
    acGeoref.accel_latitudinal = _accelLat.dat()[itime];
    acGeoref.accel_longitudinal = _accelLon.dat()[itime];

    for (size_t ii = 0; ii < _customFields.size(); ii++) {
      acGeoref.custom[ii] = _customFields[ii].dat()[itime];
    }

    if (flight_time_start < 0) {
      if (acGeoref.true_airspeed_mps > 35.0) {
        flight_time_start = acGeoref.time_secs_utc;
      }
    }
    if (flight_time_start >= 0) {
      acGeoref.flight_time_secs = acGeoref.time_secs_utc - flight_time_start;
    } else {
      acGeoref.flight_time_secs = 0;
    }

    STRncopy(acGeoref.callsign, _callSign.c_str(), AC_GEOREF_NBYTES_CALLSIGN);
  
    // add to spdb

    string callSignLast4(_callSign.substr(_callSign.size() - 4));
    si32 dataType = Spdb::hash4CharsToInt32(callSignLast4.c_str());
    si32 dataType2 = acGeoref.time_nano_secs;
    time_t validTime = acGeoref.time_secs_utc;
    ac_georef_t copy = acGeoref;
    BE_from_ac_georef(&copy);
    _spdb.addPutChunk(dataType,
                      validTime,
                      validTime,
                      sizeof(ac_georef_t),
                      &copy,
                      dataType2);
    
  } // itime

  // write out to data base
  
  _doWrite();

  return 0;

  return 0;

}

///////////////////////////////////
// read the global attributes

int AcGeoref2Spdb::_readGlobalAttr()
  
{

  int iret = 0;

  _callSign.clear();
  _projectName.clear();
  _flightNum.clear();

  _file.readGlobAttr("Platform", _callSign);
  _file.readGlobAttr("ProjectName", _projectName);
  _file.readGlobAttr("FlightNumber", _flightNum);

  _timeCoordName.clear();
  _latCoordName.clear();
  _lonCoordName.clear();
  
  if (_file.readGlobAttr("time_coordinate", _timeCoordName)) {
    cerr << "ERROR - AcGeoref2Spdb::_readGlobalAttr()" << endl;
    cerr << "  Cannot find global attr: 'time_coordinate'" << endl;
    iret = -1;
  }

  if (_file.readGlobAttr("latitude_coordinate", _latCoordName)) {
    cerr << "ERROR - AcGeoref2Spdb::_readGlobalAttr()" << endl;
    cerr << "  Cannot find global attr: 'latitude_coordinate'" << endl;
    iret = -1;
  }

  if (_file.readGlobAttr("longitude_coordinate", _lonCoordName)) {
    cerr << "ERROR - AcGeoref2Spdb::_readGlobalAttr()" << endl;
    cerr << "  Cannot find global attr: 'longitude_coordinate'" << endl;
    iret = -1;
  }

  if (_params.debug) {
    cerr << "  _timeCoordName: " << _timeCoordName << endl;
    cerr << "  _latCoordName: " << _latCoordName << endl;
    cerr << "  _lonCoordName: " << _lonCoordName << endl;
  }

  return iret;

}

///////////////////////////////////
// read the times

int AcGeoref2Spdb::_readTimes()
  
{

  multimap<string, NcVar> allVars = _file.getNcFile()->getVars();
  multimap<string, NcVar>::iterator it;
  int count = 0;
  if (_params.debug >= Params::DEBUG_VERBOSE) {
    cerr << "======>> Variable list" << endl;
    for (it = allVars.begin(); it != allVars.end(); it++, count++) {
      cerr << "  count, name, varName: " << count << ", " 
           << it->first << ", " << it->second.getName() << endl;
    }
  }

  // read the time coordinate variable

  _timeVar = _file.getNcFile()->getVar(_timeCoordName);
  if (_timeVar.isNull()) {
    cerr << "ERROR - AcGeoref2Spdb::_readTimes" << endl;
    cerr << "  Cannot get time variable: " << _timeCoordName << endl;
    return -1;
  }

  if (_timeVar.getDimCount() != 1) {
    cerr << "ERROR - AcGeoref2Spdb::_readTimes" << endl;
    cerr << "  time variable should have 1 dimension" << endl;
    cerr << "  n dims found: " << _timeVar.getDimCount() << endl;
    return -1;
  }

  NcDim timeDim = _timeVar.getDim(0);
  if (timeDim.isNull()) {
    cerr << "ERROR - AcGeoref2Spdb::_readTimes" << endl;
    cerr << "  cannot read time dimension" << endl;
    return -1;
  }
  _nTimes = timeDim.getSize();

  // get units attribute
  
  NcVarAtt unitsAtt = _timeVar.getAtt("units");
  if (unitsAtt.isNull()) {
    cerr << "ERROR - AcGeoref2Spdb::_readTimes" << endl;
    cerr << "  cannot read time 'units' attribute" << endl;
    return -1;
  }
  string units;
  unitsAtt.getValues(units);
  units = _file.stripNulls(units);

  // set the time base from the units

  _timeBase.setFromW3c(units.c_str(), true);
  if (_params.debug) {
    cerr << "==>> nTimes: " << _nTimes << endl;
    cerr << "==>> Time units: " << units << endl;
    cerr << "==>> Time base: " << _timeBase.asString() << endl;
  }
  
  // set the time array
  
  _times = _times_.alloc(_nTimes);
  TaArray<int> dTimes_;
  int *dTimes = dTimes_.alloc(_nTimes);
  _timeVar.getVar(dTimes);

  for (size_t ii = 0; ii < _nTimes; ii++) {
    _times[ii] = _timeBase.utime() + dTimes[ii];
  }

  if (_params.debug) {
    cerr << "==>> startTime: " << DateTime::strm(_times[0]) << endl;
    cerr << "==>> endTime: " << DateTime::strm(_times[_nTimes-1]) << endl;
  }

  return 0;

}

///////////////////////////////////
// read the time series variables

int AcGeoref2Spdb::_readTimeSeriesVars()
  
{

  int iret = 0;

  if (_readTimeSeriesVar(_lat, _latCoordName)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_lon, _lonCoordName)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_altMsl, _params.nc_varname_altitude_msl)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_altPres, _params.nc_varname_altitude_pres)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_ewVel, _params.nc_varname_ew_velocity)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_nsVel, _params.nc_varname_ns_velocity)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_vertVel, _params.nc_varname_vert_velocity)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_ewWind, _params.nc_varname_ew_wind)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_nsWind, _params.nc_varname_ns_wind)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_vertWind, _params.nc_varname_vert_wind)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_heading, _params.nc_varname_heading)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_drift, _params.nc_varname_drift_angle)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_track, _params.nc_varname_track)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_roll, _params.nc_varname_roll)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_pitch, _params.nc_varname_pitch)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_temp, _params.nc_varname_temperature)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_pressure, _params.nc_varname_pressure)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_rh, _params.nc_varname_rh)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_aoa, _params.nc_varname_angle_of_attack)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_ias, _params.nc_varname_indicated_airspeed)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_tas, _params.nc_varname_true_airspeed)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_accelNorm, _params.nc_varname_normal_accel)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_accelLat, _params.nc_varname_lateral_accel)) {
    iret = -1;
  }
  if (_readTimeSeriesVar(_accelLon, _params.nc_varname_longitudinal_accel)) {
    iret = -1;
  }
  for (size_t ii = 0; ii < _customFields.size(); ii++) {
    if (_readTimeSeriesVar(_customFields[ii], _params._nc_varname_custom_fields[ii])) {
      iret = -1;
    }
  }

  return iret;

}
  
///////////////////////////////////
// read a time series variable

int AcGeoref2Spdb::_readTimeSeriesVar(TaArray<double> &array,
                                      string varName)
  
{

  // initialize

  _setTimeSeriesVarToMissing(array);

  // get the variable
  
  NcVar var = _file.getNcFile()->getVar(varName);
  if (var.isNull()) {
    cerr << "ERROR - AcGeoref2Spdb::_readTimeSeriesVar" << endl;
    cerr << "  Cannot find var: " << varName << endl;
    return -1;
  }

  // check num dims

  if (var.getDimCount() != 1) {
    cerr << "ERROR - AcGeoref2Spdb::_readTimeSeriesVar" << endl;
    cerr << "  variable should have 1 dimension: " << varName << endl;
    cerr << "  n dims found: " << var.getDimCount() << endl;
    return -1;
  }

  // get dim

  NcDim dim = var.getDim(0);
  if (dim.isNull()) {
    cerr << "ERROR - AcGeoref2Spdb::_readTimeSeriesVar" << endl;
    cerr << "  cannot get dimension for var: " << varName << endl;
    return -1;
  }

  // check dim len

  size_t len = dim.getSize();
  if (len != _nTimes) {
    cerr << "ERROR - AcGeoref2Spdb::_readTimeSeriesVar" << endl;
    cerr << "  Dimension does not match time dim, var: " << varName << endl;
    cerr << "  Dim len: " << len << endl;
    cerr << "  Time dim len: " << _nTimes << endl;
    return -1;
  }

  // get units
  
  NcVarAtt unitsAtt = var.getAtt("units");
  string units;
  if (!unitsAtt.isNull()) {
    unitsAtt.getValues(units);
    units = _file.stripNulls(units);
  }

  // var type

  NcType::ncType varType = var.getType().getTypeClass();

  // get fill value
  
  int ifill = -9999;
  float ffill = -9999.0f;
  double dfill = -9999.0;

  NcVarAtt fillValAtt = var.getAtt("_FillValue");
  if (fillValAtt.isNull()) {
    fillValAtt = var.getAtt("missing_value");
  }
  if (!fillValAtt.isNull()) {
    if (varType == NC_DOUBLE) {
      fillValAtt.getValues(&dfill);
    } else if (varType == NC_FLOAT) {
      fillValAtt.getValues(&ffill);
    } else if (varType == NC_INT) {
      fillValAtt.getValues(&ifill);
    }
  }

  // read in

  double *dArray = array.dat();
  if (varType == NC_DOUBLE) {
    var.getVar(dArray);
    for (size_t ii = 0; ii < _nTimes; ii++) {
      if (dArray[ii] == dfill) {
        dArray[ii] = _missingDbl;
      }
    }
  } else if (varType == NC_FLOAT) {
    TaArray<float> fArray_;
    float *fArray = fArray_.alloc(_nTimes);
    var.getVar(fArray);
    for (size_t ii = 0; ii < _nTimes; ii++) {
      if (fArray[ii] == ffill) {
        dArray[ii] = _missingDbl;
      } else {
        dArray[ii] = fArray[ii];
      }
    }
  } else if (varType == NC_INT) {
    TaArray<int> iArray_;
    int *iArray = iArray_.alloc(_nTimes);
    var.getVar(iArray);
    for (size_t ii = 0; ii < _nTimes; ii++) {
      if (iArray[ii] == ifill) {
        dArray[ii] = _missingDbl;
      } else {
        dArray[ii] = iArray[ii];
      }
    }
  } else {
    cerr << "ERROR - AcGeoref2Spdb::_readTimeSeriesVar" << endl;
    cerr << "  Var is not double, float or int, var: " << varName << endl;
    cerr << "  Setting to missing" << endl;
    return -1;
  }

  return 0;

}

///////////////////////////////////
// set time var to missing

void AcGeoref2Spdb::_setTimeSeriesVarToMissing(TaArray<double> &array)
  
{

  double *data = array.alloc(_nTimes);
  for (size_t ii = 0; ii < _nTimes; ii++) {
    data[ii] = _missingDbl;
  }

}

////////////////////////////////////////
// write to SPDB

int AcGeoref2Spdb::_doWrite()

{
  
  if (_spdb.put(_params.output_spdb_url,
                SPDB_AC_GEOREF_ID,
                SPDB_AC_GEOREF_LABEL)) {
    cerr << "ERROR - AcGeoref2Spdb::_doWrite()" << endl;
    cerr << _spdb.getErrStr() << endl;
    _spdb.clearPutChunks();
    return -1;
  }

  _spdb.clearPutChunks();
  return 0;

}

/////////////////////////////////////////////////
// Get temperatures from StatusXml if available

void AcGeoref2Spdb::_getHcrTempsFromStatusXml(const string &statusXml, 
                                              double &cmigitsTemp, 
                                              double &tailconeTemp)
  
{

  // get cmigits temp

  double val;
  if (TaXml::readDouble(statusXml, "CmigitsTemp", val) == 0) {
    cmigitsTemp = val;
  }
  if (TaXml::readDouble(statusXml, "TailconeTemp", val) == 0) {
    tailconeTemp = val;
  }

}

/////////////////////////////////////////////////////////////////////////
// compute surface velocity
//
// Sets vel to (0.0) if cannot determine valocity from surface.

double AcGeoref2Spdb::_computeSurfaceVel(RadxRay &ray)
  
{

  // init
  
  double surfaceVel = 0.0;

  // check elevation
  // cannot compute gnd vel if not pointing down
  
  double elev = ray.getElevationDeg();
  if (elev > -85 || elev < -95) {
    if (_params.debug >= Params::DEBUG_VERBOSE) {
      cerr << "Bad elevation for finding surface, time, elev(deg): "
           << ray.getRadxTime().asString() << ", "
           << elev << endl;
    }
    return surfaceVel;
  }

  // get dbz field

  RadxField *dbzField = ray.getField(_params.cfradial_dbz_field_name);
  if (dbzField == NULL) {
    cerr << "ERROR - HcrVelCorrect::_computeSurfaceVel" << endl;
    cerr << "  No dbz field found, field name: " << _params.cfradial_dbz_field_name << endl;
    return surfaceVel;
  }
  const Radx::fl32 *dbzArray = dbzField->getDataFl32();
  Radx::fl32 dbzMiss = dbzField->getMissingFl32();

  // get vel field

  RadxField *velField = ray.getField(_params.cfradial_vel_field_name);
  if (velField == NULL) {
    cerr << "ERROR - HcrVelCorrect::_computeSurfaceVel" << endl;
    cerr << "  No vel field found, field name: " << _params.cfradial_vel_field_name << endl;
    return surfaceVel;
  }
  const Radx::fl32 *velArray = velField->getDataFl32();
  Radx::fl32 velMiss = velField->getMissingFl32();
  
  // get gate at which max dbz occurs

  double range = dbzField->getStartRangeKm();
  double drange = dbzField->getGateSpacingKm();
  double dbzMax = -9999;
  int gateForMax = -1;
  double rangeToSurface = 0;
  double foundSurface = false;
  for (size_t igate = 0; igate < dbzField->getNPoints(); igate++, range += drange) {
    if (range < _params.min_range_to_surface_km) {
      continue;
    }
    Radx::fl32 dbz = dbzArray[igate];
    if (dbz != dbzMiss) {
      if (dbz > dbzMax) {
        dbzMax = dbz;
        gateForMax = igate;
        rangeToSurface = range;
        foundSurface = true;
      }
    }
  }

  // check for sufficient power

  if (foundSurface) {
    if (dbzMax < _params.min_dbz_for_surface_echo) {
      foundSurface = false;
      if (_params.debug) {
        cerr << "WARNING - HcrVelCorrect::_computeSurfaceVel" << endl;
        cerr << "  Ray at time: " << ray.getRadxTime().asString() << endl;
        cerr << "  Dbz max not high enough for surface detection: " << dbzMax << endl;
        cerr << "  Range to max dbz: " << rangeToSurface << endl;
      }
    }
  }

  size_t nEachSide = _params.ngates_for_surface_echo / 2;
  if (foundSurface) {
    for (size_t igate = gateForMax - nEachSide; igate <= gateForMax + nEachSide; igate++) {
      Radx::fl32 dbz = dbzArray[igate];
      if (dbz == dbzMiss) {
        foundSurface = false;
      }
      if (dbz < _params.min_dbz_for_surface_echo) {
        foundSurface = false;
      }
    }
  }
  
  // compute surface vel
  
  if (foundSurface) {
    double sum = 0.0;
    double count = 0.0;
    for (size_t igate = gateForMax - nEachSide; igate <= gateForMax + nEachSide; igate++) {
      Radx::fl32 vel = velArray[igate];
      if (vel == velMiss) {
        foundSurface = false;
      }
      sum += vel;
      count++;
    }
    surfaceVel = sum / count;
  }

  return surfaceVel;

}

/////////////////////////////////////////////////////////////////////////
// Handle IWG1 packet

int AcGeoref2Spdb::_handleIwg1(const char *buf, int bufLen)

{

  vector<string> toks;
  TaStr::tokenizeAllowEmpty(buf, ',', toks);

  cerr << "IWG1, ntoks: " << toks.size() << endl;
  for (size_t ii = 0; ii < toks.size(); ii++) {
    cerr << "IWG1, ii, tok: " << ii << ": " << toks[ii] << endl;
  }

  return 0;

}
