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
// Hsrl2Radx.cc
//
// Mike Dixon, EOL, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
// June 2015
// 
// Brad Schoenrock, EOL, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
// updated Mar 2017
//
///////////////////////////////////////////////////////////////

#include "Hsrl2Radx.hh"
#include "Names.hh"
#include <Radx/Radx.hh>
#include <Radx/RadxVol.hh>
#include <Radx/RadxRay.hh>
#include <Radx/RadxField.hh>
#include <Radx/RadxGeoref.hh>
#include <Radx/RadxTime.hh>
#include <Radx/RadxTimeList.hh>
#include <Radx/RadxPath.hh>
#include <Radx/RadxArray.hh>
#include <Radx/NcfRadxFile.hh>
#include <dsserver/DsLdataInfo.hh>
#include <didss/DsInputPath.hh>
#include <toolsa/TaXml.hh>
#include <toolsa/pmu.h>
#include <Fmq/DsFmq.hh>
#include <radar/HsrlRawRay.hh>

#include <cmath>  
#include <fstream>
#include <sstream>
#include <string>

#include "MslFile.hh"
#include "RawFile.hh"
#include "CalReader.hh"
#include "FullCals.hh"
#include "DerFieldCalcs.hh"
#include "OutputFmq.hh"

using namespace std;

// Constructor

Hsrl2Radx::Hsrl2Radx(int argc, char **argv)
  
{

  OK = TRUE;

  // set programe name

  _progName = "Hsrl2Radx";
  
  // parse command line args
  
  if (_args.parse(argc, argv, _progName)) {
    cerr << "ERROR: " << _progName << endl;
    cerr << "Problem with command line args." << endl;
    OK = FALSE;
    return;
  }
  
  // get TDRP params
  
  _paramsPath = (char *) "unknown";
  if (_params.loadFromArgs(argc, argv,
			   _args.override.list, &_paramsPath)) {
    cerr << "ERROR: " << _progName << endl;
    cerr << "Problem with TDRP parameters." << endl;
    OK = FALSE;
    return;
  }

  // check on overriding radar location

  if (!_params.read_georef_data_from_aircraft_system) {
    if (_params.instrument_latitude_deg < -900 ||
        _params.instrument_longitude_deg < -900 ||
        _params.instrument_altitude_meters < -900) {
      cerr << "ERROR: " << _progName << endl;
      cerr << "  Problem with command line or TDRP parameters." << endl;
      cerr << "  You do not have aircraft data position information" << endl;
      cerr << "  Therefore you must set latitude, longitude and altitude" << endl;
      cerr << "  You must set all 3 values." << endl;
      OK = FALSE;
    }
  }

}

// destructor

Hsrl2Radx::~Hsrl2Radx()

{

  // unregister process

  PMU_auto_unregister();

}

//////////////////////////////////////////////////
// Run

int Hsrl2Radx::Run()
{
  // reading in calibration files here ----- 
  
  _cals.readDeadTimeHi(_params.calvals_gvhsrl_path, 
                       _params.combined_hi_dead_time_name);
  _cals.readDeadTimeLo(_params.calvals_gvhsrl_path, 
                       _params.combined_lo_dead_time_name);
  _cals.readDeadTimeCross(_params.calvals_gvhsrl_path, 
                          _params.cross_pol_dead_time_name);
  _cals.readDeadTimeMol(_params.calvals_gvhsrl_path, 
                        _params.molecular_dead_time_name);
  _cals.readBinWidth(_params.calvals_gvhsrl_path, 
                     _params.bin_width_name); 
  _cals.readScanAdj(_params.calvals_gvhsrl_path, 
                    _params.scan_adjustment_name); 


  _cals.readBLCor(_params.baseline_calibration_path);
  _cals.readDiffDGeoCor(_params.diff_default_geofile_path);
  _cals.readGeoDefCor(_params.geofile_default_path);
  _cals.readAfPulCor(_params.afterpulse_default_path);
    
  //done reading calibration files. 

  if (_params.mode == Params::ARCHIVE) {
    return _runArchive();
  } else if (_params.mode == Params::FILELIST) {
    return _runFilelist();
  } else if (_params.mode == Params::REALTIME_FMQ) {
    return _runRealtimeFmq();
  } else if (_params.mode == Params::REALTIME_FILE) {
    if (_params.latest_data_info_avail) {
      return _runRealtimeWithLdata();
    } else {
      return _runRealtimeNoLdata();
    }
  }

  // will not reach here
  return -1;

}

//////////////////////////////////////////////////
// Run in filelist mode

int Hsrl2Radx::_runFilelist()
{

  // loop through the input file list

  int iret = 0;

  for (int ii = 0; ii < (int) _args.inputFileList.size(); ii++) {

    string inputPath = _args.inputFileList[ii];
    if (_processFile(inputPath)) {
      iret = -1;
    }

  }

  return iret;

}

//////////////////////////////////////////////////
// Run in archive mode

int Hsrl2Radx::_runArchive()
{

  // get the files to be processed

  RadxTimeList tlist;
  tlist.setDir(_params.input_dir);
  tlist.setModeInterval(_args.startTime, _args.endTime);
  if (tlist.compile()) {
    cerr << "ERROR - Hsrl2Radx::_runArchive()" << endl;
    cerr << "  Cannot compile time list, dir: " << _params.input_dir << endl;
    cerr << "  Start time: " << RadxTime::strm(_args.startTime) << endl;
    cerr << "  End time: " << RadxTime::strm(_args.endTime) << endl;
    cerr << tlist.getErrStr() << endl;
    return -1;
  }

  const vector<string> &paths = tlist.getPathList();
  if (paths.size() < 1) {
    cerr << "ERROR - Hsrl2Radx::_runArchive()" << endl;
    cerr << "  No files found, dir: " << _params.input_dir << endl;
    return -1;
  }
  
  // loop through the input file list
  
  int iret = 0;
  for (size_t ii = 0; ii < paths.size(); ii++) {
    if (_processFile(paths[ii])) {
      iret = -1;
    }
  }

  return iret;

}

//////////////////////////////////////////////////
// Run in realtime mode with latest data info

int Hsrl2Radx::_runRealtimeWithLdata()
{

  // init process mapper registration

  PMU_auto_init(_progName.c_str(), _params.instance,
                PROCMAP_REGISTER_INTERVAL);

  // watch for new data to arrive

  LdataInfo ldata(_params.input_dir,
                  _params.debug >= Params::DEBUG_VERBOSE);
  if (strlen(_params.search_ext) > 0) {
    ldata.setDataFileExt(_params.search_ext);
  }
  
  int iret = 0;
  int msecsWait = _params.wait_between_checks * 1000;
  while (true) {
    ldata.readBlocking(_params.max_realtime_data_age_secs,
                       msecsWait, PMU_auto_register);
    const string path = ldata.getDataPath();
    if (_processFile(path)) {
      iret = -1;
    }
  }

  return iret;

}

//////////////////////////////////////////////////
// Run in realtime mode without latest data info

int Hsrl2Radx::_runRealtimeNoLdata()
{

  // init process mapper registration

  PMU_auto_init(_progName.c_str(), _params.instance,
                PROCMAP_REGISTER_INTERVAL);
  
  // Set up input path

  DsInputPath input(_progName,
		    _params.debug >= Params::DEBUG_VERBOSE,
		    _params.input_dir,
		    _params.max_realtime_data_age_secs,
		    PMU_auto_register,
		    _params.latest_data_info_avail,
		    false);

  input.setFileQuiescence(_params.file_quiescence);
  input.setSearchExt(_params.search_ext);
  input.setRecursion(_params.search_recursively);
  input.setMaxRecursionDepth(_params.max_recursion_depth);
  input.setMaxDirAge(_params.max_realtime_data_age_secs);

  int iret = 0;

  while(true) { // how does this loop end? --Brad

    // check for new data
    
    char *path = input.next(false);
    
    if (path == NULL) {
      
      // sleep a bit
      
      PMU_auto_register("Waiting for data");
      umsleep(_params.wait_between_checks * 1000);

    } else {

      // process the file

      if (_processFile(path)) {
        iret = -1;
      }
      
    }

  } // while

  return iret;

}

//////////////////////////////////////////////////
// Run in realtime mode,
// reads raw rays from an FMQ
// writes moments to and FMQ

int Hsrl2Radx::_runRealtimeFmq()
{
  
  // init process mapper registration
  
  PMU_auto_init(_progName.c_str(), _params.instance,
                PROCMAP_REGISTER_INTERVAL);

  // create output queue
  
  OutputFmq outputFmq(_progName, _params);
  if (!outputFmq.constructorOK) {
    cerr << "ERROR - Hsrl2Radx::_runRealtimeFmq" << endl;
    cerr << "  Cannot init FMQ for writing" << endl;
    cerr << "  Fmq: " << _params.output_fmq_url << endl;
    return -1;
  }

  // create input queue

  int iret = 0;
  DsFmq inputFmq;
  
  // loop

  while (true) {
    
    PMU_auto_register("Opening input FMQ");
    
    if (_params.debug) {
      cerr << "  Opening Fmq: " << _params.input_fmq_path << endl;
    }
    
    inputFmq.setHeartbeat(PMU_auto_register);
    int msecsSleepBlocking = 100;
    
    if (inputFmq.initReadBlocking(_params.input_fmq_path,
                                  "Hsrl2Radx",
                                  _params.debug >= Params::DEBUG_EXTRA,
                                  Fmq::END,
                                  msecsSleepBlocking)) {
      cerr << "ERROR - Hsrl2Radx::_runRealtimeFmq" << endl;
      cerr << "  Cannot init FMQ for reading" << endl;
      cerr << "  Fmq: " << _params.input_fmq_path << endl;
      cerr << inputFmq.getErrStr() << endl;
      umsleep(1000);
      iret = -1;
      continue;
    }
    
    // read data from the incoming FMQ and process it
    
    if (_readFmq(inputFmq, outputFmq)) {
      inputFmq.closeMsgQueue();
      iret = -1;
    }
    
  } // while(true)

  return iret;

}

///////////////////////////////////////////////////
// read data from the incoming FMQ and process it,
// writing to the output FMQ

int Hsrl2Radx::_readFmq(DsFmq &inputFmq,
                        OutputFmq &outputFmq)
  
{

  // read data from the input queue
  // convert it and write it to the output queue
  
  int64_t rayCount = 0;

  while (true) {
    
    PMU_auto_register("Reading data");
    
    // we need a new message
    // blocking read registers with Procmap while waiting
    
    if (inputFmq.readMsgBlocking()) {
      cerr << "ERROR - Hsrl2Radx::_readInputFmq()" << endl;
      cerr << "  Cannot read message from FMQ" << endl;
      cerr << "  Fmq: " << _params.input_fmq_path << endl;
      return -1;
    }
    
    if (_params.debug >= Params::DEBUG_VERBOSE) {
      cerr << "Got message from FMQ, len: " << inputFmq.getMsgLen() << endl;
    }
    
    // deserialize the raw ray

    HsrlRawRay rawRay;
    rawRay.deserialize(inputFmq.getMsg(), inputFmq.getMsgLen());
    
    if (_params.debug >= Params::DEBUG_EXTRA) {
      rawRay.printTcpHdr(cerr);
      rawRay.printMetaData(cerr);
    }

    // convert incoming raw ray into RadxRay
    
    RadxRay *radxRay = _convertRawToRadx(rawRay);

    // add environment fields
    
    _addEnvFields(radxRay);

    // add moments

    _addDerivedFields(radxRay);

    // write params to FMQ every n rays

    if (rayCount % _params.nrays_for_params == 0) {
      outputFmq.writeParams(radxRay);
    }

    // write ray to the output FMQ

    outputFmq.writeRay(radxRay);
    
    // clean up

    delete radxRay;
    rayCount++;

  } // while

  return 0;

}

//////////////////////////////////////////////////
// Convert raw ray to Radx Ray

RadxRay *Hsrl2Radx::_convertRawToRadx(HsrlRawRay &rawRay)

{

  // create ray
  
  RadxRay *ray = new RadxRay;

  // range geom & ngates

  double gateSpacingKm = _params.raw_bin_spacing_km;
  double startRangeKm = _params.raw_bin_start_range_km;
  ray->setRangeGeom(startRangeKm, gateSpacingKm);
  ray->setNGates(rawRay.getNGates());

  // time
  
  time_t secs = rawRay.getTimeSecs();
  double nanoSecs = rawRay.getSubSecs() * 1.0e9;
  ray->setTime(secs, nanoSecs);
  
  // sweep info
  
  ray->setVolumeNumber(-1);
  ray->setSweepNumber(-1);
  ray->setSweepMode(Radx::SWEEP_MODE_POINTING);
  ray->setPrtMode(Radx::PRT_MODE_FIXED);
  ray->setTargetScanRateDegPerSec(0.0);
  ray->setIsIndexed(false);
  
  if (rawRay.getTelescopeDirn() == 1) {
      // pointing up
    ray->setAzimuthDeg(0.0);
    ray->setElevationDeg(90.0);
    ray->setFixedAngleDeg(90.0);
  } else {
    // pointing down
    ray->setAzimuthDeg(0.0);
    ray->setElevationDeg(-90.0);
    ray->setFixedAngleDeg(-90.0);
  }

#ifdef NOT_YET

  // TODO - read aircraft location etc from UDP
  // georeference
  
  if (!_params.read_georef_data_from_aircraft_system) {

    setLatitude(_latitude);
    setLongitude(_longitude);
    setAltitudeKmMsl(_altitude / 1000.0);

  } else {
    
    RadxGeoref geo;
    
    if (_telescopeDirection == 1) {
      
      // pointing up
      
      geo.setRotation(-4.0);
      geo.setTilt(0.0);
      
    } else {
      
      // pointing down
      
      geo.setRotation(184.0);
      geo.setTilt(0.0);
      
    }
    
    geo.setRoll(_roll);
    geo.setPitch(_pitch);
    geo.setHeading(_heading);
    geo.setDrift(0.0); // do not have drift in the file
    geo.setVertVelocity(_vertVel);
    
    geo.setLatitude(_latitude);
    geo.setLongitude(_longitude);
    geo.setAltitudeKmMsl(_altitude / 1000.0);
    
    double sinVal, cosVal;
    sincos(_heading * Radx::DegToRad, &sinVal, &cosVal);
    geo.setEwVelocity(_gndSpeed * sinVal);
    geo.setNsVelocity(_gndSpeed * cosVal);
    
    ray->setGeoref(geo);

  }
    
  // compute az/el from geo
  
  double azimuth, elevation;
  _computeRadarAngles(geo, corr, azimuth, elevation);
  ray->setAzimuthDeg(azimuth);
  ray->setElevationDeg(elevation);

#endif
  
  // other metadata - overloading
  
  ray->setMeasXmitPowerDbmH(rawRay.getTotalEnergy());
  ray->setEstimatedNoiseDbmHc(rawRay.getPolAngle());
  // hard coded 2000 as replacement for DATA_shot_count from raw file
  ray->setNSamples(2000);

  // add the fields
  
  _addRawFieldToRay(ray, startRangeKm, gateSpacingKm,
                    Names::CombinedHighCounts,
                    "counts",
                    Names::lidar_copolar_combined_backscatter_photon_count,
                    rawRay.getNGates(),
                    &rawRay.getCombinedHi()[0]);

  _addRawFieldToRay(ray, startRangeKm, gateSpacingKm,
                    Names::CombinedLowCounts,
                    "counts",
                    Names::lidar_copolar_combined_backscatter_photon_count,
                    rawRay.getNGates(),
                    &rawRay.getCombinedLo()[0]);

  _addRawFieldToRay(ray, startRangeKm, gateSpacingKm,
                    Names::MolecularCounts,
                    "counts",
                    Names::lidar_copolar_molecular_backscatter_photon_count,
                    rawRay.getNGates(),
                    &rawRay.getMolecular()[0]);

  _addRawFieldToRay(ray, startRangeKm, gateSpacingKm,
                    Names::CrossPolarCounts,
                    "counts",
                    Names::lidar_crosspolar_combined_backscatter_photon_count,
                    rawRay.getNGates(),
                    &rawRay.getCross()[0]);
  

  // return

  return ray;

}

//////////////////////////////////////////////////////////////
// Add fl32 field to rays

void Hsrl2Radx::_addRawFieldToRay(RadxRay *ray,
                                  double startRangeKm,
                                  double gateSpacingKm,
                                  const string &name,
                                  const string &units,
                                  const string &standardName,
                                  int nGates,
                                  const Radx::fl32 *fcounts)
  
{

  // create the field
  
  RadxField *field =
    ray->addField(name, units, nGates,
                  Radx::missingFl32,
                  fcounts,
                  true);
  
  field->setStandardName(standardName);
  field->setLongName(standardName);
  field->setRangeGeom(startRangeKm, gateSpacingKm);
  
  // // add db of same field
  
  // RadxArray<Radx::fl32> dbcounts_;
  // Radx::fl32 *dbcounts = dbcounts_.alloc(nGates);

  // for (int igate = 0; igate < nGates; igate++) {
  //   if (fcounts[igate] > 0) {
  //     dbcounts[igate] = 10.0 * log10(fcounts[igate]);
  //   } else {
  //     dbcounts[igate] = Radx::missingFl32;
  //   }
  // }

  // string dbName = "db_";
  // dbName += name;
  // string dbUnits = "db_counts";
  // string dbLongName = "db_";
  // dbLongName += longName;
  
  // RadxField *dbField =
  //   ray->addField(dbName, dbUnits, nGates,
  //                 Radx::missingFl32,
  //                 fcounts,
  //                 true);
  
  // dbField->setStandardName(standardName);
  // dbField->setLongName(dbLongName);
  // dbField->setRangeGeom(startRangeKm, gateSpacingKm);
  
}

//////////////////////////////////////////////////
// Process a file
// Returns 0 on success, -1 on failure

int Hsrl2Radx::_processFile(const string &readPath)
{


  PMU_auto_register("Processing file");
  
  if (_params.debug) {
    cerr << "INFO - Hsrl2Radx::_processFile" << endl;
    cerr << "  Input path: " << readPath << endl;
  }

  // check if this is a CfRadial file

  NcfRadxFile file;
  if (file.isCfRadial(readPath)) {
    return _processUwCfRadialFile(readPath);
  } else {
    return _processUwRawFile(readPath);
  }

}

//////////////////////////////////////////////////
// Process a UofWisc pseudo CfRadial file
// Returns 0 on success, -1 on failure

int Hsrl2Radx::_processUwCfRadialFile(const string &readPath)
{

  PMU_auto_register("Processing UW CfRadial file");

  if (_params.debug) {
    cerr << "INFO - Hsrl2Radx::_processUwCfRadialFile" << endl;
    cerr << "  UW CfRadial file: " << readPath << endl;
  }

  MslFile inFile(_params);
  _setupRead(inFile);
  
  // read in file

  RadxVol vol;
  if (inFile.readFromPath(readPath, vol)) {
    cerr << "ERROR - Hsrl2Radx::Run" << endl;
    cerr << inFile.getErrStr() << endl;
    return -1;
  }
  _readPaths = inFile.getReadPaths();
  if (_params.debug >= Params::DEBUG_VERBOSE) {
    for (size_t ii = 0; ii < _readPaths.size(); ii++) {
      cerr << "  ==>> read in file: " << _readPaths[ii] << endl;
    }
  }

  // convert to floats

  vol.convertToFl32();

  // override radar location if requested
  
  if (!_params.read_georef_data_from_aircraft_system) {
    vol.overrideLocation(_params.instrument_latitude_deg,
                         _params.instrument_longitude_deg,
                         _params.instrument_altitude_meters / 1000.0);
  }
    
  // override radar name and site name if requested
  
  if (_params.override_instrument_name) {
    vol.setInstrumentName(_params.instrument_name);
  }
  if (_params.override_site_name) {
    vol.setSiteName(_params.site_name);
  }
    
  // override start range and/or gate spacing

  if (_params.override_start_range || _params.override_gate_spacing) {
    _overrideGateGeometry(vol);
  }

  // set the range relative to the instrument location

  if (_params.set_range_relative_to_instrument) {
    _setRangeRelToInstrument(inFile, vol);
  }
  
  // set vol values

  vol.loadSweepInfoFromRays();
  vol.loadVolumeInfoFromRays();

  // set global attributes as needed

  _setGlobalAttr(vol);

  // write the file

  if (_writeVol(vol)) {
    cerr << "ERROR - Hsrl2Radx::_processFile" << endl;
    cerr << "  Cannot write volume to file" << endl;
    return -1;
  }

  return 0;

}

//////////////////////////////////////////////////
// set up read

void Hsrl2Radx::_setupRead(MslFile &file)
{

  if (_params.debug >= Params::DEBUG_VERBOSE) {
    file.setDebug(true);
  }
  if (_params.debug >= Params::DEBUG_EXTRA) {
    file.setVerbose(true);
  }

  if (_params.debug >= Params::DEBUG_EXTRA) {
    file.printReadRequest(cerr);
  }

}

//////////////////////////////////////////////////
// override the gate geometry

void Hsrl2Radx::_overrideGateGeometry(RadxVol &vol)

{

  vector<RadxRay *> &rays = vol.getRays();
  
  for (size_t iray = 0; iray < rays.size(); iray++) {
    
    RadxRay *ray = rays[iray];

    double startRange = ray->getStartRangeKm();
    double gateSpacing = ray->getGateSpacingKm();

    if (_params.override_start_range) {
      startRange = _params.start_range_km;
    }
  
    if (_params.override_gate_spacing) {
      gateSpacing = _params.gate_spacing_km;
    }

    ray->setRangeGeom(startRange, gateSpacing);

  } // iray

}

//////////////////////////////////////////////////
// set the range relative to the instrument location

void Hsrl2Radx::_setRangeRelToInstrument(MslFile &file,
                                         RadxVol &vol)

{

  // telescope direction - 0 is down, 1 is up

  const vector<int> &telescopeDirection = file.getTelescopeDirection();

  vector<RadxRay *> &rays = vol.getRays();
  
  for (size_t iray = 0; iray < rays.size(); iray++) {
    
    const RadxRay *ray = rays[iray];
    const RadxGeoref *georef = ray->getGeoreference();
    double altitudeKm = georef->getAltitudeKmMsl();

    int nGatesIn = ray->getNGates();
    double gateSpacingKm = ray->getGateSpacingKm();
    int altitudeInGates = (int) floor(altitudeKm / gateSpacingKm + 0.5);
    if (altitudeInGates > nGatesIn - 1) {
      altitudeInGates = nGatesIn - 1;
    }

    int telDirn = telescopeDirection[iray];
    bool isUp = (telDirn == _params.telescope_direction_is_up);

    int nGatesValid = nGatesIn;
    if (isUp) {
      nGatesValid = nGatesIn - altitudeInGates;
    } else {
      nGatesValid = altitudeInGates + 1;
    }
    if (nGatesValid < 0) {
      nGatesValid = 0;
    }
    if (nGatesValid > nGatesIn) {
      nGatesValid = nGatesIn;
    }

    vector<RadxField *> fields = ray->getFields();
    for (size_t ifield = 0; ifield < fields.size(); ifield++) {
      RadxField *field = fields[ifield];
      const Radx::fl32 *dataIn = field->getDataFl32();
      RadxArray<Radx::fl32> dataOut_;
      Radx::fl32 *dataOut = dataOut_.alloc(nGatesIn);
      for (int ii = 0; ii < nGatesIn; ii++) {
        dataOut[ii] = field->getMissingFl32();
      }
      if (isUp) {
        for (int ii = 0; ii < nGatesValid; ii++) {
          dataOut[ii] = dataIn[altitudeInGates + ii];
        }
      } else {
        for (int ii = 0; ii < nGatesValid; ii++) {
          dataOut[ii] = dataIn[altitudeInGates - ii];
        }
      }
      field->setDataFl32(nGatesIn, dataOut, true);
    } // ifield
    
  } // iray

}
  
//////////////////////////////////////////////////
// set up write

void Hsrl2Radx::_setupWrite(RadxFile &file)
{

  if (_params.debug) {
    file.setDebug(true);
  }
  if (_params.debug >= Params::DEBUG_EXTRA) {
    file.setVerbose(true);
  }

  if (_params.output_filename_mode == Params::START_TIME_ONLY) {
    file.setWriteFileNameMode(RadxFile::FILENAME_WITH_START_TIME_ONLY);
  } else if (_params.output_filename_mode == Params::END_TIME_ONLY) {
    file.setWriteFileNameMode(RadxFile::FILENAME_WITH_END_TIME_ONLY);
  } else {
    file.setWriteFileNameMode(RadxFile::FILENAME_WITH_START_AND_END_TIMES);
  }

  file.setWriteCompressed(true);
  file.setCompressionLevel(_params.compression_level);

  // set output format

  file.setFileFormat(RadxFile::FILE_FORMAT_CFRADIAL);
  file.setNcFormat(RadxFile::NETCDF4);

  if (strlen(_params.output_filename_prefix) > 0) {
    file.setWriteFileNamePrefix(_params.output_filename_prefix);
  }

  file.setWriteInstrNameInFileName(_params.include_instrument_name_in_file_name);
  file.setWriteSubsecsInFileName(_params.include_subsecs_in_file_name);
  file.setWriteHyphenInDateTime(_params.use_hyphen_in_file_name_datetime_part);

}

//////////////////////////////////////////////////
// set selected global attributes

void Hsrl2Radx::_setGlobalAttr(RadxVol &vol)
{

  vol.setDriver("Hsrl2Radx(NCAR)");

  if (strlen(_params.version_override) > 0) {
    vol.setVersion(_params.version_override);
  }

  if (strlen(_params.title_override) > 0) {
    vol.setTitle(_params.title_override);
  }

  if (strlen(_params.institution_override) > 0) {
    vol.setInstitution(_params.institution_override);
  }

  if (strlen(_params.references_override) > 0) {
    vol.setReferences(_params.references_override);
  }

  if (strlen(_params.source_override) > 0) {
    vol.setSource(_params.source_override);
  }

  if (strlen(_params.history_override) > 0) {
    vol.setHistory(_params.history_override);
  }

  if (strlen(_params.author_override) > 0) {
    vol.setAuthor(_params.author_override);
  }

  if (strlen(_params.comment_override) > 0) {
    vol.setComment(_params.comment_override);
  }

  RadxTime now(RadxTime::NOW);
  vol.setCreated(now.asString());

}

//////////////////////////////////////////////////
// write out the volume

int Hsrl2Radx::_writeVol(RadxVol &vol)
{

  // output file

  NcfRadxFile outFile;
  _setupWrite(outFile);
  
  if (_params.output_filename_mode == Params::SPECIFY_FILE_NAME) {

    string outPath = _params.output_dir;
    outPath += PATH_DELIM;
    outPath += _params.output_filename;

    // write to path
  
    if (outFile.writeToPath(vol, outPath)) {
      cerr << "ERROR - Hsrl2Radx::_writeVol" << endl;
      cerr << "  Cannot write file to path: " << outPath << endl;
      cerr << outFile.getErrStr() << endl;
      return -1;
    }
      
  } else {

    // write to dir
  
    if (outFile.writeToDir(vol, _params.output_dir,
                           _params.append_day_dir_to_output_dir,
                           _params.append_year_dir_to_output_dir)) {
      cerr << "ERROR - Hsrl2Radx::_writeVol" << endl;
      cerr << "  Cannot write file to dir: " << _params.output_dir << endl;
      cerr << outFile.getErrStr() << endl;
      return -1;
    }

  }

  string outputPath = outFile.getPathInUse();

  // write latest data info file if requested 
  if (_params.write_latest_data_info) {
    DsLdataInfo ldata(_params.output_dir);
    if (_params.debug >= Params::DEBUG_VERBOSE) {
      ldata.setDebug(true);
    }
    string relPath;
    RadxPath::stripDir(_params.output_dir, outputPath, relPath);
    ldata.setRelDataPath(relPath);
    ldata.setWriter(_progName);
    if (ldata.write(vol.getEndTimeSecs())) {
      cerr << "WARNING - Hsrl2Radx::_writeVol" << endl;
      cerr << "  Cannot write latest data info file to dir: "
           << _params.output_dir << endl;
    }
  }

  return 0;

}

//////////////////////////////////////////////////
// Process a UofWisc raw netcdf file
// Returns 0 on success, -1 on failure

int Hsrl2Radx::_processUwRawFile(const string &readPath)
{

  PMU_auto_register("Processing UW Raw file");
  
  if (_params.debug) {
    cerr << "INFO - Hsrl2Radx::_processUwRawFile" << endl;
    cerr << "  UW raw file: " << readPath << endl;
  }

  RawFile inFile(_params);
  
  // read in file
  RadxVol vol;
  if (inFile.readFromPath(readPath, vol)) {
    cerr << "ERROR - Hsrl2Radx::Run" << endl;
    cerr << inFile.getErrStr() << endl;
    return -1;
  }
  
  // override radar name and site name if requested
  if (_params.override_instrument_name) {
    vol.setInstrumentName(_params.instrument_name);
  }
  if (_params.override_site_name) {
    vol.setSiteName(_params.site_name);
  }
  vector<RadxRay *> rays = vol.getRays();
  
  // set global attributes as needed

  _setGlobalAttr(vol);

  // add in height, temperature and pressure fields

  for(size_t iray = 0; iray < rays.size(); iray++) {
    _addEnvFields(rays[iray]);
  }
  
  // add corrections and derived data outputs  
  // loop through the rays
  
  for(size_t iray = 0; iray < rays.size(); iray++) {
    _addDerivedFields(rays[iray]);
  }
    
  // write the file
  if (_writeVol(vol)) {
    cerr << "ERROR - Hsrl2Radx::_processFile" << endl;
    cerr << "  Cannot write volume to file" << endl;
    return -1;
  }

  return 0;

}


//////////////////////////////////////////////////
// Add in the height, pressure and temperature
// environmental fields

void Hsrl2Radx::_addEnvFields(RadxRay *ray)
{
  
  double altitudeKm = _params.instrument_altitude_meters / 1000.0;
  const RadxGeoref *geo = ray->getGeoreference();
  if (geo) {
    altitudeKm = geo->getAltitudeKmMsl();
  }
  
  double elevDeg = ray->getElevationDeg();
  size_t nGates = ray->getNGates();
  
  RadxArray<Radx::fl32> htMeters_, tempK_, presHpa_;
  Radx::fl32 *htMeters = htMeters_.alloc(nGates);
  Radx::fl32 *tempK = tempK_.alloc(nGates);
  Radx::fl32 *presHpa = presHpa_.alloc(nGates);
  
  double sinEl = sin(elevDeg * Radx::DegToRad);
  double startRangeKm = ray->getStartRangeKm();
  double gateSpacingKm = ray->getGateSpacingKm();
  
  double rangeKm = startRangeKm;
  for (size_t igate = 0; igate < nGates; igate++, rangeKm += gateSpacingKm) {
    double htm = (altitudeKm + rangeKm * sinEl) * 1000.0;
    htMeters[igate] = htm;
    tempK[igate] = _stdAtmos.ht2temp(htm);
    presHpa[igate] = _stdAtmos.ht2pres(htm);
  }

  RadxField *htField =
    ray->addField(Names::Height, "m", nGates, Radx::missingFl32, htMeters, true);
  htField->setStandardName(Names::height_above_mean_sea_level);
  htField->setLongName(Names::height_above_mean_sea_level);
  htField->setRangeGeom(startRangeKm, gateSpacingKm);

  RadxField *tempField =
    ray->addField(Names::Temperature, "K", nGates, Radx::missingFl32, tempK, true);
  tempField->setStandardName(Names::air_temperature);
  tempField->setLongName(Names::air_temperature);
  tempField->setRangeGeom(startRangeKm, gateSpacingKm);

  RadxField *presField =
    ray->addField(Names::Pressure, "HPa", nGates, Radx::missingFl32, presHpa, true);
  presField->setStandardName(Names::air_pressure);
  presField->setLongName(Names::air_pressure);
  presField->setRangeGeom(startRangeKm, gateSpacingKm);
    
}

//////////////////////////////////////////////////
// Add in the HSRL moments

void Hsrl2Radx::_addDerivedFields(RadxRay *ray)
{

  // ray info
    
  size_t nGates = ray->getNGates();
  double startRangeKm = ray->getStartRangeKm();
  double gateSpacingKm = ray->getGateSpacingKm();

  double power = ray->getMeasXmitPowerDbmH();
  double shotCount = ray->getNSamples();

  // environmental fields

  RadxField *htField = ray->getField(Names::Height);
  RadxField *tempField = ray->getField(Names::Temperature);
  RadxField *presField = ray->getField(Names::Pressure);
  assert(htField != NULL);
  assert(tempField != NULL);
  assert(presField != NULL);

  const Radx::fl32 *htMeters = htField->getDataFl32();
  const Radx::fl32 *tempK = tempField->getDataFl32();
  const Radx::fl32 *presHpa = presField->getDataFl32();

  // for (size_t ii = 0; ii < ray->getNFields(); ii++) {
  //   cerr << "1111111111 name: " << ray->getFields()[ii]->getName() << endl;
  // }

  // get raw data fields

  const RadxField *hiField = ray->getField(Names::CombinedHighCounts);
  assert(hiField != NULL);

  const RadxField *loField = ray->getField(Names::CombinedLowCounts);
  assert(loField != NULL);

  const RadxField *molField = ray->getField(Names::MolecularCounts);
  assert(molField != NULL);

  const RadxField *crossField = ray->getField(Names::CrossPolarCounts);
  assert(crossField != NULL);

  //holds derived data fields
    
  const Radx::fl32 *hiData = hiField->getDataFl32();
  const Radx::fl32 *loData = loField->getDataFl32();
  const Radx::fl32 *crossData = crossField->getDataFl32();
  const Radx::fl32 *molData = molField->getDataFl32();
  vector< Radx::fl32> hiDataVec;
  vector< Radx::fl32> loDataVec;
  vector< Radx::fl32> crossDataVec;
  vector< Radx::fl32> molDataVec;
  vector< Radx::fl32> htMVec;
  vector< Radx::fl32> tempKVec;
  vector< Radx::fl32> presHpaVec;
  
  for(size_t igate = 0; igate < nGates; igate++) {
    hiDataVec.push_back(hiData[igate]);
    loDataVec.push_back(loData[igate]);
    crossDataVec.push_back(crossData[igate]);
    molDataVec.push_back(molData[igate]);
    htMVec.push_back(htMeters[igate]* 1000.0);
    tempKVec.push_back(tempK[igate]);
    presHpaVec.push_back(presHpa[igate]);
  }
  
	
  // create object for derived fields

  DerFieldCalcs calcs(_params, _cals, nGates, hiDataVec, loDataVec, crossDataVec, 
                      molDataVec, htMVec, tempKVec, presHpaVec, 
                      shotCount, power);

  // calculate the fields

  calcs.computeDerived();

  // load fields with the results

  RadxField *volDepolField =
    ray->addField(Names::VolumeDepolRatio, "", nGates, Radx::missingFl32, 
                  calcs.getVolDepol().data(), true);
  volDepolField->setStandardName(Names::lidar_volume_depolarization_ratio);
  volDepolField->setLongName(Names::lidar_volume_depolarization_ratio);
  volDepolField->setRangeGeom(startRangeKm, gateSpacingKm);
        
  RadxField *backscatRatioField =
    ray->addField(Names::BackScatterRatio, "", nGates, Radx::missingFl32, 
                  calcs.getBackscatRatio().data(), true);
  backscatRatioField->setStandardName(Names::lidar_backscatter_ratio);
  backscatRatioField->setLongName(Names::lidar_backscatter_ratio);
  backscatRatioField->setRangeGeom(startRangeKm, gateSpacingKm);
    
  RadxField *partDepolField =
    ray->addField(Names::ParticleDepolRatio, "", nGates, Radx::missingFl32,
                  calcs.getPartDepol().data(), true);
  partDepolField->setLongName(Names::lidar_particle_depolarization_ratio);
  partDepolField->setStandardName(Names::lidar_particle_depolarization_ratio);
  partDepolField->setRangeGeom(startRangeKm, gateSpacingKm);
    
  RadxField *backscatCoeffField =
    ray->addField(Names::BackScatterCoeff, "m-1.sr-1", nGates, Radx::missingFl32,
                  calcs.getBackscatCoeff().data(), true);
  backscatCoeffField->setStandardName(Names::lidar_backscatter_coefficient);
  backscatCoeffField->setLongName(Names::lidar_backscatter_coefficient);
  backscatCoeffField->setRangeGeom(startRangeKm, gateSpacingKm);
   
  RadxField *extinctionField =
    ray->addField(Names::Extinction, "m-1", nGates, Radx::missingFl32, 
                  calcs.getExtinction().data(), true);
  extinctionField->setStandardName(Names::lidar_extinction_coefficient);
  extinctionField->setLongName(Names::lidar_extinction_coefficient);
  extinctionField->setRangeGeom(startRangeKm, gateSpacingKm);

  RadxField *optDepthField =
    ray->addField(Names::OpticalDepth, "", nGates, Radx::missingFl32, 
                  calcs.getOpticalDepth().data(), true);
  optDepthField->setStandardName(Names::lidar_optical_depth);
  optDepthField->setLongName(Names::lidar_optical_depth);
  optDepthField->setRangeGeom(startRangeKm, gateSpacingKm);
       
}
