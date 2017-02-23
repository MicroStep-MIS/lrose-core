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
//
// June 2015
//
///////////////////////////////////////////////////////////////

#include "Hsrl2Radx.hh"
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
#include <physics/IcaoStdAtmos.hh>
#include "MslFile.hh"
#include "RawFile.hh"

#include <fstream>
#include <sstream>
#include <string>

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

  if (_params.override_instrument_location) {
    if (_params.instrument_latitude_deg < -900 ||
        _params.instrument_longitude_deg < -900 ||
        _params.instrument_altitude_meters < -900) {
      cerr << "ERROR: " << _progName << endl;
      cerr << "  Problem with command line or TDRP parameters." << endl;
      cerr << "  You have chosen to override radar location" << endl;
      cerr << "  You must override latitude, longitude and altitude" << endl;
      cerr << "  You must override all 3 values." << endl;
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

  if (_params.mode == Params::ARCHIVE) {
    return _runArchive();
  } else if (_params.mode == Params::FILELIST) {
    return _runFilelist();
  } else {
    if (_params.latest_data_info_avail) {
      return _runRealtimeWithLdata();
    } else {
      return _runRealtimeNoLdata();
    }
  }
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

  // remove unwanted fields

  if (_params.exclude_specified_fields) {
    for (int ii = 0; ii < _params.excluded_fields_n; ii++) {
      if (_params.debug) {
        cerr << "Removing field name: " << _params._excluded_fields[ii] << endl;
      }
      vol.removeField(_params._excluded_fields[ii]);
    }
  }

  // convert to floats

  vol.convertToFl32();

  // override radar location if requested
  
  if (_params.override_instrument_location) {
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
    
  // apply angle offsets

  if (_params.apply_azimuth_offset) {
    if (_params.debug) {
      cerr << "NOTE: applying azimuth offset (deg): " 
           << _params.azimuth_offset << endl;
    }
    vol.applyAzimuthOffset(_params.azimuth_offset);
  }
  if (_params.apply_elevation_offset) {
    if (_params.debug) {
      cerr << "NOTE: applying elevation offset (deg): " 
           << _params.elevation_offset << endl;
    }
    vol.applyElevationOffset(_params.elevation_offset);
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

  // vol.printWithRayMetaData(cerr);

  // set field type, names, units etc
  
  _convertFields(vol);

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

  if (!_params.write_other_fields_unchanged) {

    if (_params.set_output_fields) {
      for (int ii = 0; ii < _params.output_fields_n; ii++) {
        file.addReadField(_params._output_fields[ii].input_field_name);
      }
    }

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
// rename fields as required

void Hsrl2Radx::_convertFields(RadxVol &vol)
{

  if (!_params.set_output_fields) {
    return;
  }

  for (int ii = 0; ii < _params.output_fields_n; ii++) {

    const Params::output_field_t &ofld = _params._output_fields[ii];
    
    string iname = ofld.input_field_name;
    string oname = ofld.output_field_name;
    string lname = ofld.long_name;
    string sname = ofld.standard_name;
    string ounits = ofld.output_units;
    
  }

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

  // set netcdf format - used for CfRadial

  switch (_params.netcdf_style) {
    case Params::NETCDF4_CLASSIC:
      file.setNcFormat(RadxFile::NETCDF4_CLASSIC);
      break;
    case Params::NC64BIT:
      file.setNcFormat(RadxFile::NETCDF_OFFSET_64BIT);
      break;
    case Params::NETCDF4:
      file.setNcFormat(RadxFile::NETCDF4);
      break;
    default:
      file.setNcFormat(RadxFile::NETCDF_CLASSIC);
  }

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
    
  // set global attributes as needed
  
  _setGlobalAttr(vol);

  // read in calibration files

  // this block should not be here, it means every file being processed has to read the entirety of the calibration files independantly, need to find where to better put it so calibration files are only read once. 

  bool debug = true;// use while developing to give specific output information relating to reading in cal files
  bool doneDebug = false;// use while developing when done looking at debug info for a particular cal file
  const char* baseline = "/h/eol/brads/git/hsrl_configuration/projDir/calfiles/baseline_correction_20150601T0000.blc";
  const char* diffDefGeo = "/h/eol/brads/git/hsrl_configuration/projDir/calfiles/diff_default_geofile_20120201T0000.geo";
  const char* geoDef = "/h/eol/brads/git/hsrl_configuration/projDir/calfiles/geofile_default_20150601T0000.geo";
  const char* afterpulse = "/h/eol/brads/git/hsrl_configuration/projDir/calfiles/afterpulse_default_20061001T0000.ap";
  const char* calvals = "/h/eol/brads/git/hsrl_configuration/projDir/calfiles/calvals_gvhsrl.txt";
  _readBaselineCorrection(baseline, doneDebug);
  _readDiffDefaultGeo(diffDefGeo, doneDebug);
  _readGeofileDefault(geoDef, doneDebug);
  _readAfterPulse(afterpulse, doneDebug);
  _readCalvals(calvals, debug);

  // add in height, temperature and pressure fields

  _addEnvFields(vol);
  
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

void Hsrl2Radx::_addEnvFields(RadxVol &vol)
{

  IcaoStdAtmos stdAtmos;

  // loop through the rays

  vector<RadxRay *> rays = vol.getRays();
  for(size_t iray = 0; iray < rays.size(); iray++) {

    RadxRay *ray = rays[iray];
    const RadxGeoref *geo = ray->getGeoreference();
    if (!geo) {
      continue;
    }

    double altitudeKm = geo->getAltitudeKmMsl();
    double elevDeg = ray->getElevationDeg();

    size_t nGates = ray->getNGates();

    RadxArray<Radx::fl32> htKm_, tempK_, presHpa_;
    Radx::fl32 *htKm = htKm_.alloc(nGates);
    Radx::fl32 *tempK = tempK_.alloc(nGates);
    Radx::fl32 *presHpa = presHpa_.alloc(nGates);

    double sinEl = sin(elevDeg * Radx::DegToRad);
    double startRangeKm = ray->getStartRangeKm();
    double gateSpacingKm = ray->getGateSpacingKm();

    double rangeKm = startRangeKm;
    for (size_t igate = 0; igate < nGates; igate++, rangeKm += gateSpacingKm) {
      htKm[igate] = altitudeKm + rangeKm * sinEl;
      double htM = htKm[igate] * 1000.0;
      tempK[igate] = stdAtmos.ht2temp(htM);
      presHpa[igate] = stdAtmos.ht2pres(htM);
    }

    RadxField *htField =
      ray->addField("height", "km", nGates, Radx::missingFl32, htKm, true);
    htField->setLongName("height_MSL");
    htField->setRangeGeom(startRangeKm, gateSpacingKm);

    RadxField *tempField =
      ray->addField("temperature", "K", nGates, Radx::missingFl32, tempK, true);
    tempField->setLongName("temperaure_from_std_atmos");
    tempField->setRangeGeom(startRangeKm, gateSpacingKm);

    RadxField *presField =
      ray->addField("pressure", "HPa", nGates, Radx::missingFl32, presHpa, true);
    presField->setLongName("pressure_from_std_atmos");
    presField->setRangeGeom(startRangeKm, gateSpacingKm);

  } // iray
  

}



//////////////////////////////////////////////////
// read in variables from calibration files
// stores them in a multidimentional array



//void Hsrl2Radx::_ReadCalvals(string pathToCalValsFile, time_t time)
void Hsrl2Radx::_readBaselineCorrection(const char* file, bool debug)
{
  if(debug)
    cout<< "in _readBaselineCorrection"<<'\n';
  
  std::ifstream infile(file);
  std::string line;
  vector<double> vec_binnum;
  vector<double> vec_combined_hi;
  vector<double> vec_combined_lo;
  vector<double> vec_molecular;
  vector<double> vec_crosspol;
  vector<double> vec_mol_I2A;
  vector<double> vec_comb_1064;

  while (std::getline(infile, line))
    {
      std::stringstream ss;
      ss << line;
      if(debug)
	cout<<line<<'\n';
      
      string test = "#";
            
      if(!(line.substr(0, test.length())==test))//comments at begining of file begin with #, ignore those lines and process the rest. 
	{
	  double binnum; 
	  ss>>binnum;
	  vec_binnum.push_back(binnum);
	  if(debug)
	    cout<<"binnum="<<binnum<<'\n';

	  double combined_hi; 
	  ss>>combined_hi;
	  vec_combined_hi.push_back(combined_hi);
	  if(debug)
	    cout<<"combined_hi="<<combined_hi<<'\n';

	  double combined_lo; 
	  ss>>combined_lo;
	  vec_combined_lo.push_back(combined_lo);
	  if(debug)
	    cout<<"combined_lo="<<combined_lo<<'\n';

	  double molecular; 
	  ss>>molecular;
	  vec_molecular.push_back(molecular);
	  if(debug)
	    cout<<"molecular="<<molecular<<'\n';

	  double crosspol; 
	  ss>>crosspol;
	  vec_crosspol.push_back(crosspol);
	  if(debug)
	    cout<<"crosspol="<<crosspol<<'\n';

	  double mol_I2A; 
	  ss>>mol_I2A;
	  vec_mol_I2A.push_back(mol_I2A);
	  if(debug)
	    cout<<"mol_I2A="<<mol_I2A<<'\n';

	  double comb_1064; 
	  ss>>comb_1064;
	  vec_comb_1064.push_back(comb_1064);
	  if(debug)
	    cout<<"comb_1064="<<comb_1064<<'\n';
	}
    }
}

void Hsrl2Radx::_readDiffDefaultGeo(const char* file, bool debug)
{
  if(debug)
    cout<< "in _readDiffDefaultGeo"<<'\n';
  std::ifstream infile(file);
  std::string line;
  vector<double> vec_altitudes;
  vector<double> vec_comb_himol;
  vector<double> vec_comb_lomol;
  vector<double> vec_scomb_himol;
  vector<double> vec_scomb_lomol;

  while (std::getline(infile, line))
    {
      std::stringstream ss;
      ss << line;
      if(debug)
	cout<<line<<'\n';

      string test = "#";
            
      if(!(line.substr(0, test.length())==test))//comments at begining of file begin with #, ignore those lines and process the rest. 
	{
	  double altitudes; 
	  ss>>altitudes;
	  vec_altitudes.push_back(altitudes);
	  if(debug)
	    cout<<"altitudes="<<altitudes<<'\n';
	  
	  double comb_himol; 
	  ss>>comb_himol;
	  vec_comb_himol.push_back(comb_himol);
	  if(debug)
	    cout<<"comb_himol="<<comb_himol<<'\n';
	  
	  double comb_lomol; 
	  ss>>comb_lomol;
	  vec_comb_lomol.push_back(comb_lomol);
	  if(debug)
	    cout<<"comb_lomol="<<comb_lomol<<'\n';
	  	  	  
	  double scomb_himol; 
	  ss>>scomb_himol;
	  vec_scomb_himol.push_back(scomb_himol);
	  if(debug)
	    cout<<"scomb_himol="<<scomb_himol<<'\n';
	  
	  double scomb_lomol; 
	  ss>>scomb_lomol;
	  vec_scomb_lomol.push_back(scomb_lomol);
	  if(debug)
	    cout<<"scomb_lomol="<<scomb_lomol<<'\n';
	  
	}
    }
} 

void Hsrl2Radx::_readGeofileDefault(const char* file, bool debug)
{
  if(debug)
    cout<< "in _readGeofileDefault"<<'\n';
  std::ifstream infile(file);
  std::string line;
  vector<double> vec_range;
  vector<double> vec_geo_corr;

  while (std::getline(infile, line))
    {
      std::stringstream ss;
      ss << line;
      if(debug)
	cout<<line<<'\n';
       
      string test = "#";
            
      if(!(line.substr(0, test.length())==test))//comments at begining of file begin with #, ignore those lines and process the rest. 
	{
	  double range; 
	  ss>>range;
	  vec_range.push_back(range);
	  if(debug)
	    cout<<"range="<<range<<'\n';

	  double geo_corr; 
	  ss>>geo_corr;
	  vec_geo_corr.push_back(geo_corr);
	  if(debug)
	    cout<<"geo_corr="<<geo_corr<<'\n';
	  
	}
    }
} 

void Hsrl2Radx::_readAfterPulse(const char* file, bool debug)
{
  if(debug)
    cout<< "in _readAfterPulse"<<'\n';
  std::ifstream infile(file);
  std::string line;
  vector<double> vec_bin;
  vector<double> vec_mol;
  vector<double> vec_comb;
  vector<double> vec_crossPol;
  vector<double> vec_refftMol;
  vector<double> vec_imfftMol;
  vector<double> vec_refftComb;
  vector<double> vec_imfftComb;
  vector<double> vec_refftCPol;
  vector<double> vec_imfftCPol;


  while (std::getline(infile, line))
    {
      std::stringstream ss;
      ss << line;
      if(debug)
	cout<<line<<'\n';
       
      string test = "#";
            
      if(!(line.substr(0, test.length())==test))//comments at begining of file begin with #, ignore those lines and process the rest. 
	{
	  double bin; 
	  ss>>bin;
	  vec_bin.push_back(bin);
	  if(debug)
	    cout<<"bin="<<bin<<'\n';

	  double mol; 
	  ss>>mol;
	  vec_mol.push_back(mol);
	  if(debug)
	    cout<<"mol="<<mol<<'\n';

	  double comb; 
	  ss>>comb;
	  vec_comb.push_back(comb);
	  if(debug)
	    cout<<"comb="<<comb<<'\n';

	  double crossPol; 
	  ss>>crossPol;
	  vec_crossPol.push_back(crossPol);
	  if(debug)
	    cout<<"crossPol="<<crossPol<<'\n';

	  double refftMol; 
	  ss>>refftMol;
	  vec_refftMol.push_back(refftMol);
	  if(debug)
	    cout<<"refftMol="<<refftMol<<'\n';

	  double imfftMol; 
	  ss>>imfftMol;
	  vec_imfftMol.push_back(imfftMol);
	  if(debug)
	    cout<<"imfftMol="<<imfftMol<<'\n';

	  double refftComb; 
	  ss>>refftComb;
	  vec_refftComb.push_back(refftComb);
	  if(debug)
	    cout<<"refftComb="<<refftComb<<'\n';

	  double imfftComb; 
	  ss>>imfftComb;
	  vec_imfftComb.push_back(imfftComb);
	  if(debug)
	    cout<<"imfftComb="<<imfftComb<<'\n';

	  double refftCPol; 
	  ss>>refftCPol;
	  vec_refftCPol.push_back(refftCPol);
	  if(debug)
	    cout<<"refftCPol="<<refftCPol<<'\n';

	  double imfftCPol; 
	  ss>>imfftCPol;
	  vec_imfftCPol.push_back(imfftCPol);
	  if(debug)
	    cout<<"imfftCPol="<<imfftCPol<<'\n';

	}
    }
} 

void Hsrl2Radx::_readCalvals(const char* file, bool debug)
{
  if(debug)
    cout<< "in _readCalvals"<<'\n';
  std::ifstream infile(file);
  std::string line;
  while (std::getline(infile, line))
    {
      std::stringstream ss;
      ss << line;
      
      string test = "#";
      //fix the following conditional to look for # anywhere..... 
      if( !(line.substr(0, test.length())==test) && ( (line.length()<=3) || (line.length()>3) && !(line.substr(3, test.length())==test) ) && ( (line.length()<=4) || (line.length()>4) && !(line.substr(4, test.length())==test) ) ) //comments at begining of file begin with #, ignore those lines and process the rest. In the calvals file some comments are tabbed in three or four spaces and need to check for that. 
	{
	  if(debug)
	    cout<<line<<'\n';
	}
    }
} 
