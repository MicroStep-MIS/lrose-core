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
///////////////////////////////////////////////////////////////////////////
// Grib2 to Netcdf Manager
//
// Using a list of fields to process, converts each field
// from grib2 to Netcdf format.
//
// Jason Craig
///////////////////////////////////////////////////////////////////////////

#include "Grib2Nc.hh"

#include <cstring>

#ifndef NOT_RAL
#include <toolsa/pmu.h>
#include <dsdata/DsFileListTrigger.hh>
#include <dsdata/DsLdataTrigger.hh>
#include <dsdata/DsInputDirTrigger.hh>
#include <dsdata/TriggerInfo.hh>
#endif
#include <toolsa/utim.h> // For unix_to_date
#include <euclid/Pjg.hh>
#include <euclid/PjgLc1Calc.hh>
#include <euclid/PjgLc2Calc.hh>
#include <euclid/PjgPolarStereoCalc.hh>
#include <euclid/PjgMercatorCalc.hh>
#include <grib2/GDS.hh>
#include <grib2/PDS.hh>
#include <grib2/DRS.hh>
#include <grib2/DS.hh>
#include <grib2/LatLonProj.hh>
#include <grib2/LambertConfProj.hh>
#include <grib2/PolarStereoProj.hh>
#include <grib2/MercatorProj.hh>
#include <grib2/GausLatLonProj.hh>
#include <grib2/Template5.2.hh>
#include <grib2/Template5.3.hh>

#include "Grib2toNc.hh"
#include "NcOutput.hh"
using namespace std;

//
// Constants
//
const float  Grib2Nc::FIELD_DATA_EPSILON = .0000001;
const double Grib2Nc::M_TO_KM = 0.001;
const double Grib2Nc::M_TO_FT = 3.2808399;
const double Grib2Nc::M_TO_KFT = .00328; 
const double Grib2Nc::M_TO_MI = 0.000621371192;
const double Grib2Nc::M_TO_100FT = .0328;
const double Grib2Nc::MPS_TO_KNOTS = 1.94;
const double Grib2Nc::PASCALS_TO_MBARS = 0.01;
const double Grib2Nc::KELVIN_TO_CELCIUS = -273.15;
const double Grib2Nc::KG_TO_G = 1000.0;
const double Grib2Nc::PERCENT_TO_FRAC = 0.01;
const double Grib2Nc::PASCAL_TO_HECTOPASCAL = 0.01;


Grib2Nc::Grib2Nc (Params &params)
{
  _paramsPtr = &params;
#ifndef NOT_RAL
  _dataTrigger = NULL;
#endif
  _outputFile = NULL;
  _Grib2File = NULL;
  _GribRecord = NULL;
}

Grib2Nc::~Grib2Nc()
{
  _gribFields.erase(_gribFields.begin(), _gribFields.end() );
#ifndef NOT_RAL
  delete _dataTrigger;
#endif
  if (_outputFile != (NcOutput *)NULL)
    delete _outputFile;
  if (_Grib2File != (Grib2::Grib2File *)NULL)
    delete _Grib2File;
}

int Grib2Nc::init(int nFiles, char** fileList, char* outputFile,
		  bool printVarList,  bool printsummary , bool printsections)
{
#ifdef NOT_RAL
  // External releases only file lists are supported
   if( nFiles > 0 ) {
     for (int i = 0; i < nFiles; ++i)
       _file_list.push_back(fileList[i]);
   }
#else
   if( nFiles > 0 ) {
     vector< string > file_list;
     for (int i = 0; i < nFiles; ++i)
       file_list.push_back(fileList[i]);
     
     DsFileListTrigger *data_trigger = new DsFileListTrigger();
     if (data_trigger->init(file_list) != 0)
     {
       cerr << "ERROR: Error initializing file list trigger" << endl;
       return RI_FAILURE;
     }
     
     _dataTrigger = data_trigger;

     _inputSuffix = "";
     _inputSubstring = "";
   }
   else {
     if (_paramsPtr->latest_data_info_avail) {
       DsLdataTrigger *data_trigger = new DsLdataTrigger();
       if (data_trigger->init(_paramsPtr->input_dir,
                              _paramsPtr->max_input_data_age,
                              PMU_auto_register) != 0)
       {
         cerr << "ERROR: Error initializing realtime trigger" << endl;
         return RI_FAILURE;
       }
       _dataTrigger = data_trigger;
       _inputSuffix = _paramsPtr->input_suffix;
       _inputSubstring = _paramsPtr->input_substring;
     } else {
       DsInputDirTrigger *data_trigger = new DsInputDirTrigger();
       if (data_trigger->init(_paramsPtr->input_dir,
                              _paramsPtr->input_substring,
                              !_paramsPtr->latest_file_only,
                              PMU_auto_register,
                              _paramsPtr->recursive_search,
                              "",
                              _paramsPtr->data_check_interval_secs) != 0)
       {
         cerr << "ERROR: Error initializing realtime trigger" << endl;
         return RI_FAILURE;
       }
       _dataTrigger = data_trigger;
       _inputSuffix = _paramsPtr->input_suffix;
       _inputSubstring = _paramsPtr->input_substring;
     }
   }
#endif

   _Grib2File   = new Grib2::Grib2File ();
   _printVarList = printVarList;
   _printSummary = printsummary;
   _printSections = printsections;

   _outputFile = new NcOutput( _paramsPtr, outputFile );

   //
   // Create a list of variables to process from the params request
   if (!_paramsPtr->process_everything) {
     _gribFields.insert( _gribFields.begin(), _paramsPtr->_output_fields,
			 (_paramsPtr->_output_fields)+(_paramsPtr->output_fields_n));
   }
   return( RI_SUCCESS );
   
}

int Grib2Nc::getData()
{
   string filePath;
   Grib2::Grib2Record::print_sections_t printSec;

   //
   // Create a grib2 Print sections object from the params request
   if(_paramsPtr->debug > 1 || _printSections ) {
     printSec.is = _paramsPtr->printSec_is;
     printSec.ids = _paramsPtr->printSec_ids;
     printSec.lus = _paramsPtr->printSec_lus;
     printSec.gds = _paramsPtr->printSec_gds;
     printSec.pds = _paramsPtr->printSec_pds;
     printSec.drs = _paramsPtr->printSec_drs;
     printSec.bms = _paramsPtr->printSec_bms;
     printSec.ds = _paramsPtr->printSec_ds;
   }

   //
   // Process each file
#ifdef NOT_RAL
   for(std::vector<string>::iterator file = _file_list.begin();
       file != _file_list.end(); file++) {

      filePath = *file;
#else
   TriggerInfo trigger_info;
   while (_dataTrigger->next(trigger_info) == 0) {

      filePath = trigger_info.getFilePath();
#endif
      
      // Check for appropriate substring and extension
      if (!_inputSubstring.empty() &&
	  filePath.find(_inputSubstring, 0) == string::npos)
	continue;
      
      if (!_inputSuffix.empty()) {
	size_t dotPos = filePath.rfind( "." );
	if(filePath.substr(dotPos+1) == _inputSuffix)
	  continue;
   }
      
      //
      // Inventory the file
#ifndef NOT_RAL
      PMU_auto_register( "Processing grib2 file" );
#endif
      cout << "Processing file " << filePath << endl << flush;
      if(_Grib2File->read(filePath) != Grib2::GRIB_SUCCESS)
	continue;

      //
      // Print the full contents of the grib file
      if(_paramsPtr->debug > 1 || _printSections ) 
	_Grib2File->printContents(stdout, printSec);

      if(_printSections ) 
	continue;

      //
      // Print only a summary of the grib file
      if (_paramsPtr->debug || _printSummary)
	_Grib2File->printSummary(stdout, _paramsPtr->debug);

      if(_printSummary)
	continue;

      //
      // Get the full list of fields just read in the inventory process
      if (_paramsPtr->process_everything || _printVarList) {
        _gribFields.erase(_gribFields.begin(), _gribFields.end());
	list <string> fieldList = _Grib2File->getFieldList();
	list <string>::const_iterator field;

	for (field = fieldList.begin(); field != fieldList.end(); ++field) {
	  list <string> fieldLevelList = _Grib2File->getFieldLevels(*(field));
	  list <string>::const_iterator level;

	  for (level = fieldLevelList.begin(); level != fieldLevelList.end(); ++level) {
	    Params::out_field_t out_field;
	    out_field.param = new char[50];
	    out_field.level = new char[50];
	    memcpy(out_field.param, (*(field)).c_str(), strlen((*(field)).c_str())+1);
	    memcpy(out_field.level, (*(level)).c_str(), strlen((*(level)).c_str())+1);
            out_field.vert_level_min = -1;
            out_field.vert_level_max = -1;
	    out_field.vert_level_dz = 1;
	    _gribFields.push_back(out_field);

	    if(_printVarList) {
	      vector<Grib2::Grib2Record::Grib2Sections_t> GribRecords = _Grib2File->getRecords(out_field.param, out_field.level);
	      char name[20];
	      sprintf(name, "%s %s", out_field.param, out_field.level);
	      printf("%-20s \t'%s' '%s'\n", name,
		     GribRecords[0].summary->longName.c_str(), GribRecords[0].summary->levelTypeLong.c_str());
	    }
	  }
	}
	if(_printVarList)
	  continue;
      }

      // Get the list of forecast times, usually there is just one
      // but not always.
      list <long int> forecastList = _Grib2File->getForecastList();
      list <long int>::const_iterator leadTime;

      // Loop over the lead times, each time will become a file
      for (leadTime = forecastList.begin(); leadTime != forecastList.end(); ++leadTime) {
        
        // check the lead time if required

        if (_paramsPtr->check_lead_time &&
            *leadTime > _paramsPtr->max_lead_time_secs) {
          continue;
        }

	if (_paramsPtr->debug)
	  cout << "Getting fields for forecast time of " << *leadTime << " seconds." << endl;

	// Loop over the list of fields to process
	// Keep track of the generate and forecast times

	time_t lastGenerateTime = 0;
#ifndef NOT_RAL
	PMU_auto_register( "Reading grib2 file" );
#endif
	_field = _gribFields.begin();
	while (_field != _gribFields.end()) {
	  if (_paramsPtr->debug)
	    cout << "Looking for field " <<  _field->param << " level  " << _field->level << endl;
	  
	  vector<Grib2::Grib2Record::Grib2Sections_t> GribRecords = _Grib2File->getRecords(_field->param, _field->level, *leadTime);
	  
	  if(GribRecords.size() > 1)
	    _sortByLevel(GribRecords.begin(), GribRecords.end());
	  
	  fl32 *fieldDataPtr = NULL;
	  fl32 *currDataPtr = NULL;

	  if (_paramsPtr->debug)
	    cout << "Found " << GribRecords.size() << " records." << endl;
	  if(GribRecords.size() >= MAX_VLEVELS) {
	    cerr << "ERROR: Too many fields for one record! " << GribRecords.size() << " > Max Vlevels" << endl;
	    return( RI_FAILURE );
	  }

	  //
	  // Set requested vertical level bounds
	  int levelMin = 0;
	  if(_field->vert_level_min >= 0)
	    levelMin = _field->vert_level_min;

	  int levelMax;
	  if( _field->vert_level_max < 0 ) {
	    levelMax = GribRecords.size() - 1;
          } else if( _field->vert_level_max  < (int) GribRecords.size() - 1) {
	    levelMax = _field->vert_level_max;
	  } else {
	    levelMax = GribRecords.size() - 1;
          }

	  size_t levelDz = 1;
	  if( _field->vert_level_dz > 1)
	    levelDz =  _field->vert_level_dz;

	  //
	  // Loop over requested vertical levels in each field
	  for(int levelNum = levelMin; levelNum <= levelMax; levelNum+=levelDz) {
	    
	    _GribRecord = &(GribRecords[levelNum]);

	    if (_paramsPtr->debug) {
	      cout <<  _GribRecord->summary->name.c_str() << " ";
	      cout <<  _GribRecord->summary->longName.c_str() << " ";
	      cout <<  _GribRecord->summary->units.c_str() << " ";
	      cout <<  _GribRecord->summary->category << " ";
	      cout <<  _GribRecord->summary->paramNumber << " ";
	      cout <<  _GribRecord->summary->levelType.c_str() << " ";
	      cout <<  _GribRecord->summary->levelVal;
	      cout <<  endl;
	    }

	    //
	    // Create the field info for the first level
	    if (levelNum == levelMin) {
	      
	      if ( _setFieldInfo() != RI_SUCCESS ) {
		cerr << "WARNING: File " << filePath << " not processed." << endl << flush;
		_outputFile->clear();
		_Grib2File->clearInventory();
		return( RI_FAILURE );
	      }

	      _convertGribLevel2CFLevel(_GribRecord->summary->levelType, _GribRecord->summary->levelUnits, _GribRecord->summary->levelTypeLong);


	      // Pre Count the number of levels
	      for(int ln = levelNum; ln <= levelMax; ln+=levelDz)
		_fieldInfo.vlevelInfo.nz ++;

	      if (_paramsPtr->debug) {
		cout << "Processing  " << _fieldInfo.vlevelInfo.nz << " records." << endl;
	      }
	  
	      fieldDataPtr = new fl32[(size_t)_fieldInfo.vlevelInfo.nz*(size_t)_fieldInfo.gridInfo.nx*(size_t)_fieldInfo.gridInfo.ny];
	      currDataPtr = fieldDataPtr;
	    }

	    //
	    // Generation time changed. This shouldn't happen and we can't handle it correctly.
	    if (currDataPtr != fieldDataPtr && _GribRecord->ids->getGenerateTime() != lastGenerateTime) {
	      
	      cerr << "ERROR: File containes multiple generation times, currently unsupported." << endl << flush;
	      _outputFile->clear();
	      _Grib2File->clearInventory();
	      return( RI_FAILURE );
	    }

	    //
	    // Get and save the data, reordering and remaping if needed.
	    fl32 *data = _GribRecord->ds->getData();
	    if(_reMapField)
	      data = _reMapReducedOrGaussian(data);
	    if(_reOrderNS_2_SN)
	      _reOrderNS2SN(data, _fieldInfo.gridInfo.nx, _fieldInfo.gridInfo.ny);
	    if(_reOrderAdjacent_Rows)
	      _reOrderAdjacentRows(data, _fieldInfo.gridInfo.nx, _fieldInfo.gridInfo.ny);
	    
	    memcpy ( currDataPtr, data, sizeof(fl32)*_fieldInfo.gridInfo.nx*_fieldInfo.gridInfo.ny );
	    currDataPtr += _fieldInfo.gridInfo.nx*_fieldInfo.gridInfo.ny;
	    if(_reMapField)
	      delete[] data;
	    _GribRecord->ds->freeData();

	    //
	    // fill out the vlevel info
	    _fieldInfo.vlevelInfo.zData[(levelNum - levelMin)/levelDz] = _GribRecord->summary->levelVal;

	    lastGenerateTime = _GribRecord->ids->getGenerateTime();
	  }
	  // end Loop over number of vertical levels for field
	  
	  if(GribRecords.size() == 0) {
	    cerr << "WARNING: Field " <<  _field->param << " level " << _field->level <<
	      " not found in grib file." << endl;
	  } else {

	    //
	    // We have all the data for this field do any requested transforms
	    if(_paramsPtr->process_everything) {
	      _setFieldNames(-1);
	      _convertUnits(-1,fieldDataPtr);
	      _convertVerticalUnits(-1);
	      fieldDataPtr = _encode(fieldDataPtr, _paramsPtr->packing_type);
	    } else {
	      for (int i = 0; i < _paramsPtr->output_fields_n; i++) {
		if (strcmp(_paramsPtr->_output_fields[i].param,
			   _GribRecord->summary->name.c_str()) == 0 &&
		    strcmp(_paramsPtr->_output_fields[i].level,
			   _GribRecord->summary->levelType.c_str()) == 0 ) {
		  _setFieldNames(i);
		  _limitDataRange(i,fieldDataPtr);
		  _convertUnits(i,fieldDataPtr);
		  _convertVerticalUnits(i);
		  fieldDataPtr = _encode(fieldDataPtr, _paramsPtr->_output_fields[i].packing_type);
		}
	      }
	    }

	    //
	    // Add field to the outputFile
	    _outputFile->addField(fieldDataPtr, _fieldInfo);

	  }

	  _field++;

	}  // end loop over field list

	if(forecastList.size() == 1)
	  _Grib2File->clearInventory();

	if (_outputFile->numFields() == 0) {
	  cerr << "WARNING: No fields found in grib2 file." << endl;
	  cerr << "         No output file created." << endl;
	} else {

#ifndef NOT_RAL
	  PMU_auto_register( "Writing netcdf file" );
#endif

	  char whenChar[20];
	  UTIMstruct timeStruct;
	  UTIMunix_to_date(lastGenerateTime, &timeStruct);

	  sprintf(whenChar, "%4d/%02d/%02d %02d:%02d:%02d",
		  timeStruct.year, timeStruct.month, timeStruct.day,
		  timeStruct.hour, timeStruct.min, timeStruct.sec);
	  
	  cout << "Writing grid output file at " << whenChar << " for a forecast time of " \
	       << (*leadTime/3600) << " hours" << endl << flush;
	  
	  if ( _outputFile->writeNc( lastGenerateTime, *leadTime ) != 0 ) {
            cerr << "ERROR: Could not write netcdf file." << endl;
            _outputFile->clear();
            _Grib2File->clearInventory();
	    return( RI_FAILURE );
	  }

	  _outputFile->clear();	  

	}
	
      }  // Close loop on forecast leadTime
      
      _Grib2File->clearInventory();

   } // Close loop over input grib2 files
   
   return(RI_SUCCESS);
}


//
// Returns the level appropriate to a Grib level
void Grib2Nc::_convertGribLevel2CFLevel(const string &GribLevel, const string &GribLevelUnits, const string &GribLevelLong)
{
  const char *GribLevelCStr = GribLevel.c_str();
  if (!strcmp(GribLevelCStr, "SFC") || !strcmp(GribLevelCStr, "SURFACE") || 
      !strcmp(GribLevelCStr, "SEAB") || !strcmp(GribLevelCStr, "PVL") || !strcmp(GribLevelCStr, "MSL")) 
  {
      _fieldInfo.vlevelInfo.standardName = "";
      _fieldInfo.vlevelInfo.units = GribLevelUnits;
      _fieldInfo.vlevelInfo.longName = GribLevelLong;
  } else
    if( !strcmp(GribLevelCStr, "HYBL") ) {
      _fieldInfo.vlevelInfo.standardName = "atmosphere_hybrid_sigma_pressure_coordinate";
      _fieldInfo.vlevelInfo.units = GribLevelUnits;
      _fieldInfo.vlevelInfo.longName = "hybrid sigma z levels";
      _fieldInfo.vlevelInfo.positive = NcOutput::down;
    } else
    if(!strcmp(GribLevelCStr, "SIGL") || !strcmp(GribLevelCStr, "SIGMA")) {
      _fieldInfo.vlevelInfo.standardName = "atmosphere_sigma_coordinate";
      _fieldInfo.vlevelInfo.units = GribLevelUnits;
      _fieldInfo.vlevelInfo.longName = "sigma z levels";
      _fieldInfo.vlevelInfo.positive = NcOutput::down;
     } else 
      if(!strcmp(GribLevelCStr, "TMPL") || !strcmp(GribLevelCStr, "SPH_ABV_GRD") || 
	 !strcmp(GribLevelCStr, "HTGL") || !strcmp(GribLevelCStr, "GPML") || 
	 !strcmp(GribLevelCStr, "DBLL") || !strcmp(GribLevelCStr, "THEL") ||
	 !strcmp(GribLevelCStr, "EtaL") || !strcmp(GribLevelCStr, "DBSL")) {
	_fieldInfo.vlevelInfo.standardName = "altitude";
	_fieldInfo.vlevelInfo.units = GribLevelUnits;
	_fieldInfo.vlevelInfo.longName = GribLevelLong;
	_fieldInfo.vlevelInfo.positive = NcOutput::up;
      } else
       if(!strcmp(GribLevelCStr, "SPDL") || !strcmp(GribLevelCStr, "ISOB") || !strcmp(GribLevelCStr, "ISBL")) {
	 _fieldInfo.vlevelInfo.standardName = "air_pressure";
	 _fieldInfo.vlevelInfo.units = GribLevelUnits;
	 _fieldInfo.vlevelInfo.longName = GribLevelLong;
	 //
	 // This is an optional attribute since pressure levels
	 // can be identified by pressure units.
	 //
	 _fieldInfo.vlevelInfo.positive = NcOutput::down;
       } else {
	 _fieldInfo.vlevelInfo.positive = NcOutput::up;
	 if (!strcmp(GribLevelCStr, "CBL")) {
	   _fieldInfo.vlevelInfo.standardName = "";
	   _fieldInfo.vlevelInfo.units = NcOutput::level;
	   _fieldInfo.vlevelInfo.longName = GribLevelLong;
	 } else if (!strcmp(GribLevelCStr, "CTL")) {
	   _fieldInfo.vlevelInfo.standardName = "";
	   _fieldInfo.vlevelInfo.units = NcOutput::level;
	   _fieldInfo.vlevelInfo.longName = GribLevelLong;
	 } else if (!strcmp(GribLevelCStr, "0DEG")) {
	   _fieldInfo.vlevelInfo.standardName = "";
	   _fieldInfo.vlevelInfo.units = NcOutput::level;
	   _fieldInfo.vlevelInfo.longName = GribLevelLong;
	 } else if (!strcmp(GribLevelCStr, "0_ISO")) {
	   _fieldInfo.vlevelInfo.standardName = "";
	   _fieldInfo.vlevelInfo.units = NcOutput::level;
	   _fieldInfo.vlevelInfo.longName = GribLevelLong;
	 } else if (!strcmp(GribLevelCStr, "ADCL")) {
	   _fieldInfo.vlevelInfo.standardName = "";
	   _fieldInfo.vlevelInfo.units = NcOutput::level;
	   _fieldInfo.vlevelInfo.longName = GribLevelLong;
	 } else if (!strcmp(GribLevelCStr, "MWSL")) {
	   _fieldInfo.vlevelInfo.standardName = "";
	   _fieldInfo.vlevelInfo.units = NcOutput::level;
	   _fieldInfo.vlevelInfo.longName = GribLevelLong;
	 } else if (!strcmp(GribLevelCStr, "TRO")) {
	   _fieldInfo.vlevelInfo.standardName = "";
	   _fieldInfo.vlevelInfo.units = NcOutput::level;
	   _fieldInfo.vlevelInfo.longName = GribLevelLong;
	 } else if (!strcmp(GribLevelCStr, "NTAT")) {
	   _fieldInfo.vlevelInfo.standardName = "";
	   _fieldInfo.vlevelInfo.units = NcOutput::level;
	   _fieldInfo.vlevelInfo.longName = GribLevelLong;
	 } else {
	   _fieldInfo.vlevelInfo.standardName = "";
	   _fieldInfo.vlevelInfo.units = NcOutput::level;
	   _fieldInfo.vlevelInfo.longName = GribLevelLong;
	 }
       }
}

//
// Creates/sets the Field Information from the current _GribRecord pointer
int Grib2Nc::_setFieldInfo()
{

  _reOrderNS_2_SN = false;
  _reOrderAdjacent_Rows = false;
  _reMapField = false;

  _fieldInfo.addOffset = 0.0;
  _fieldInfo.scaleFactor = 1.0;
  _fieldInfo.ncType = Params::DATA_PACK_NONE;

  // We set nz to zero then count the number of levels we find
  _fieldInfo.vlevelInfo.nz  = 0;

  Grib2::DataRepTemp::data_representation_t drsConstants = _GribRecord->drs->getDrsConstants();
  Grib2::DataRepTemp *drsTemplate = _GribRecord->drs->getDrsTemplate();

  // Generic missing values if not specified in Grib
  _fieldInfo.secondaryMissing = -9998.0;
  _fieldInfo.missing          = -9999.0;  
  _fieldInfo.min_value        = _fieldInfo.missing;
  _fieldInfo.max_value        = _fieldInfo.missing;

  _fieldInfo.gridInfo.lat1           = 0.0;
  _fieldInfo.gridInfo.lat2           = 0.0;
  _fieldInfo.gridInfo.tan_lon        = 0.0;
  _fieldInfo.gridInfo.central_scale  = 0.0;
  _fieldInfo.gridInfo.pole_type      = 0;
 
  if(drsConstants.templateNumber == 2) {
    Grib2::Template5_pt_2 *Template5_2 = (Grib2::Template5_pt_2 *)drsTemplate;

    if(Template5_2->_missingType != 0 && Template5_2->_missingType != 255) {
      _fieldInfo.secondaryMissing  = Template5_2->_primaryMissingVal;
      _fieldInfo.missing           = Template5_2->_secondaryMissingVal;
    }

  } else if(drsConstants.templateNumber == 3) {
    Grib2::Template5_pt_3 *Template5_3 = (Grib2::Template5_pt_3 *)drsTemplate;

    if(Template5_3->_missingType != 0 && Template5_3->_missingType != 255) {
      _fieldInfo.secondaryMissing  = Template5_3->_primaryMissingVal;
      _fieldInfo.missing           = Template5_3->_secondaryMissingVal;
    }

  }

  si32 projID = _GribRecord->gds->getGridID();
  Grib2::GribProj *proj = _GribRecord->gds->getProjection();
  
  if(projID == Grib2::GDS::EQUIDISTANT_CYL_PROJ_ID) {
    Grib2::LatLonProj *latlonProj = (Grib2::LatLonProj *)proj;
    
    _fieldInfo.gridInfo.ncfGridName     = NcOutput::latitude_longitude;
    _fieldInfo.gridInfo.nx              = latlonProj->_ni;
    _fieldInfo.gridInfo.ny              = latlonProj->_nj;
    _fieldInfo.gridInfo.minx            = latlonProj->_lo1;
    _fieldInfo.gridInfo.miny            = latlonProj->_la1;
    _fieldInfo.gridInfo.dx              = latlonProj->_di;
    _fieldInfo.gridInfo.dy              = latlonProj->_dj;

    // If _resolutionFlag & 32 == true
    // i direction increments not given, need to calculate
    if ((latlonProj->_resolutionFlag & 32) == 0) {
      _fieldInfo.gridInfo.dx = (latlonProj->_lo2 - latlonProj->_lo1) / (latlonProj->_ni -1);
      if(_fieldInfo.gridInfo.dx < 0.0)
	_fieldInfo.gridInfo.dx *= -1.0;
    }
    // If _resolutionFlag & 16 == true
    // j direction increments not given, need to calculate
    if ((latlonProj->_resolutionFlag & 16) == 0) {
      _fieldInfo.gridInfo.dy = (latlonProj->_la2 - latlonProj->_la1) / (latlonProj->_nj -1);
      if(_fieldInfo.gridInfo.dy < 0.0)
	_fieldInfo.gridInfo.dy *= -1.0;
    }

    if(_fieldInfo.gridInfo.minx > 180.0 && _fieldInfo.gridInfo.minx <= 360.0)
      _fieldInfo.gridInfo.minx -= 360.0;

    // If _scanModeFlag & 64 == true
    // Data is order north to south
    // We expect south to north so switch la1 with la2 
    // and reorder the data
    if ((latlonProj->_scanModeFlag & 64) == 0) {
      _fieldInfo.gridInfo.miny = latlonProj->_la2;
      _reOrderNS_2_SN = true;  // flag to reorder the data
    }
    // If _scanModeFlag & 16 == true
    // Adjacent rows scan in opposite direction
    // reorder to scan in the same direction
    if (latlonProj->_scanModeFlag & 16) {
      _reOrderAdjacent_Rows = true;  // flag to reorder the data
    }

    _fieldInfo.gridInfo.proj_origin_lat = _fieldInfo.gridInfo.miny;
    _fieldInfo.gridInfo.proj_origin_lon = _fieldInfo.gridInfo.minx;


  } else if(projID == Grib2::GDS::LAMBERT_CONFORMAL_PROJ_ID) {
    Grib2::LambertConfProj *lambertProj = (Grib2::LambertConfProj *)proj;

    _fieldInfo.gridInfo.ncfGridName = NcOutput::lambert_conformal_conic;
    _fieldInfo.gridInfo.nx          = lambertProj->_nx;
    _fieldInfo.gridInfo.ny          = lambertProj->_ny;
    _fieldInfo.gridInfo.proj_origin_lat = lambertProj->_latin1;
    _fieldInfo.gridInfo.proj_origin_lon = lambertProj->_lov;
    _fieldInfo.gridInfo.dx          = lambertProj->_dx;
    _fieldInfo.gridInfo.dy          = lambertProj->_dy;
    _fieldInfo.gridInfo.lat1        = lambertProj->_latin1;
    _fieldInfo.gridInfo.lat2        = lambertProj->_latin2;

    if(_fieldInfo.gridInfo.proj_origin_lon > 180.0 && _fieldInfo.gridInfo.proj_origin_lon <= 360.0)
      _fieldInfo.gridInfo.proj_origin_lon -= 360.0;

    if ((lambertProj->_scanModeFlag & 64) == 0) {
      _reOrderNS_2_SN = true;
    }
    if (lambertProj->_scanModeFlag & 16) {
      _reOrderAdjacent_Rows = true;
    }

     // 
     // calculate min_x and min_y
     double min_x, min_y;
     
     PjgCalc *calculator;
     
     if (lambertProj->_latin1 == lambertProj->_latin2)
       calculator = new PjgLc1Calc(lambertProj->_latin1, lambertProj->_lov, lambertProj->_latin1);
     else
       calculator = new PjgLc2Calc(lambertProj->_latin1, lambertProj->_lov,
				   lambertProj->_latin1, lambertProj->_latin2);

     calculator->latlon2xy(lambertProj->_la1, lambertProj->_lo1, min_x, min_y);
     delete(calculator);

     _fieldInfo.gridInfo.minx = min_x;
     _fieldInfo.gridInfo.miny = min_y;

  } else if(projID == Grib2::GDS::POLAR_STEREOGRAPHIC_PROJ_ID) {
    Grib2::PolarStereoProj *polarProj = (Grib2::PolarStereoProj *)proj;

    _fieldInfo.gridInfo.ncfGridName     = NcOutput::polar_stereographic;
    _fieldInfo.gridInfo.nx              = polarProj->_nx;
    _fieldInfo.gridInfo.ny              = polarProj->_ny;
    _fieldInfo.gridInfo.proj_origin_lat = polarProj->_la1;
    _fieldInfo.gridInfo.proj_origin_lon = polarProj->_lo1;

    double polarStereoAdjustment = 2.0 / (1.0 + sin(polarProj->_lad * PI / 180.0));

    _fieldInfo.gridInfo.dx = polarProj->_dx;
    _fieldInfo.gridInfo.dy = polarProj->_dy;
    _fieldInfo.gridInfo.tan_lon = polarProj->_lov;
    _fieldInfo.gridInfo.pole_type = polarProj->_projCtrFlag;
    _fieldInfo.gridInfo.central_scale = 1.0 / polarStereoAdjustment;
    _fieldInfo.gridInfo.false_easting  = 500.0;
    _fieldInfo.gridInfo.false_northing  = 0.0;

    if ((polarProj->_scanModeFlag & 64) == 0) {
      _reOrderNS_2_SN = true;
    }
    if (polarProj->_scanModeFlag & 16) {
      _reOrderAdjacent_Rows = true;
    }

    // 
    // calculate min_x and min_y
    double min_x, min_y;
    
    PjgCalc *calculator;
    PjgTypes::pole_type_t pole_type = PjgTypes::POLE_NORTH;
    if(_fieldInfo.gridInfo.pole_type == 1)
      pole_type = PjgTypes::POLE_SOUTH;
    calculator = new PjgPolarStereoCalc(polarProj->_lov, pole_type, _fieldInfo.gridInfo.central_scale);
    calculator->latlon2xy(polarProj->_la1, polarProj->_lo1, min_x, min_y);
    delete(calculator);
    
    _fieldInfo.gridInfo.minx = min_x;
    _fieldInfo.gridInfo.miny = min_y;

  } else if(projID == Grib2::GDS::MERCATOR_PROJ_ID) {
    Grib2::MercatorProj *mercatorProj = (Grib2::MercatorProj *)proj;

    if(mercatorProj->_lov != 0.0) {
      cerr << "ERROR: Mercator projection orientation angles not yet supported." << endl << flush;
      return( RI_FAILURE );
    }
      
    _fieldInfo.gridInfo.ncfGridName     = NcOutput::mercator;
    _fieldInfo.gridInfo.nx              = mercatorProj->_ni;
    _fieldInfo.gridInfo.ny              = mercatorProj->_nj;
    _fieldInfo.gridInfo.dx              = mercatorProj->_di;
    _fieldInfo.gridInfo.dy              = mercatorProj->_dj;
    _fieldInfo.gridInfo.proj_origin_lat = 0.0;  // mercatorProj->_lad;
    _fieldInfo.gridInfo.proj_origin_lon = mercatorProj->_lo1;
    _fieldInfo.gridInfo.tan_lon         = mercatorProj->_lov;
    _fieldInfo.gridInfo.false_easting  = 500.0;
    _fieldInfo.gridInfo.false_northing  = 0.0;

    if(_fieldInfo.gridInfo.proj_origin_lon > 180.0 && _fieldInfo.gridInfo.proj_origin_lon <= 360.0)
      _fieldInfo.gridInfo.proj_origin_lon -= 360.0;

     // 
     // calculate min_x and min_y
     double min_x, min_y;
     
     PjgCalc *calculator;
     calculator = new PjgMercatorCalc(0.0, mercatorProj->_lo1);  //mercatorProj->_lad
     calculator->latlon2xy(mercatorProj->_la1, mercatorProj->_lo1, min_x, min_y);
     delete(calculator);

    _fieldInfo.gridInfo.minx = min_x;
    _fieldInfo.gridInfo.miny = min_y;

    if ((mercatorProj->_scanModeFlag & 64) == 0) {
      _reOrderNS_2_SN = true;
    }
    if (mercatorProj->_scanModeFlag & 16) {
      _reOrderAdjacent_Rows = true;
    }

  } else if(projID == Grib2::GDS::GAUSSIAN_LAT_LON_PROJ_ID) {
    Grib2::GausLatLonProj *latlonProj = (Grib2::GausLatLonProj *)proj;
    
    _fieldInfo.gridInfo.ncfGridName     = NcOutput::latitude_longitude;
    _fieldInfo.gridInfo.nx              = latlonProj->_maxNi;
    _fieldInfo.gridInfo.ny              = latlonProj->_nj;
    _fieldInfo.gridInfo.minx            = latlonProj->_lo1;
    _fieldInfo.gridInfo.miny            = latlonProj->_la1;
    _fieldInfo.gridInfo.dx              = latlonProj->_di;
    _fieldInfo.gridInfo.dy              = 180. / (latlonProj->_nParalells*2);
    _fieldInfo.gridInfo.proj_origin_lat = _fieldInfo.gridInfo.miny;
    _fieldInfo.gridInfo.proj_origin_lon = _fieldInfo.gridInfo.minx;

    if(_fieldInfo.gridInfo.dx == -1)
      _fieldInfo.gridInfo.dx = (latlonProj->_lo2 + (latlonProj->_lo2 / (double)(latlonProj->_maxNi-1)) - latlonProj->_lo1) / latlonProj->_maxNi;

    // We must re-map gaussian lat/lon to a regularaly spaced grid
    _reMapField = true;

    // If _scanModeFlag & 64 == true
    // Data is order north to south
    // MDV expects south to north so switch la1 with la2 
    // and reorder the data
    if ((latlonProj->_scanModeFlag & 64) == 0) {
      _fieldInfo.gridInfo.miny = latlonProj->_la2;
      _reOrderNS_2_SN = true;  // flag to reorder the data
    }
    // If _scanModeFlag & 16 == true
    // Adjacent rows scan in opposite direction
    // reorder to scan in the same direction
    if (latlonProj->_scanModeFlag & 16) {
      _reOrderAdjacent_Rows = true;  // flag to reorder the data
    }

  } else {
    cerr << "ERROR: Unimplemented projection type " << projID << endl << flush;
    return( RI_FAILURE );
  }
  
  return( RI_SUCCESS );
}

//
// Sorts a list of Grib2Records by level
void Grib2Nc::_sortByLevel(vector<Grib2::Grib2Record::Grib2Sections_t>::iterator begin, 
			    vector<Grib2::Grib2Record::Grib2Sections_t>::iterator end)
{
  bool reverse = false;
  const char *GribLevelCStr = (begin)->summary->levelType.c_str();
  // Pressure levels go in descending order
  if(!strcmp(GribLevelCStr, "SPDL") || !strcmp(GribLevelCStr, "ISBL"))
    reverse = true;
  vector<Grib2::Grib2Record::Grib2Sections_t>::iterator IPos;
  vector<Grib2::Grib2Record::Grib2Sections_t>::iterator JPos;
  Grib2::Grib2Record::Grib2Sections_t temp;
  for (IPos = end-1; IPos != begin-1; IPos--)
  {
    for (JPos = begin+1; JPos != IPos+1; JPos++)
    {
      if ((!reverse && (JPos-1)->summary->levelVal > JPos->summary->levelVal) ||
	  (reverse && (JPos-1)->summary->levelVal < JPos->summary->levelVal))
      {
        temp = *(JPos-1);
        *(JPos-1) = *(JPos);
        *(JPos) = temp;
      }
    }
  }

}

// reorders data from North to South to South to North
// East/West increments remain the same
// For GFS data this means  that the data starts in the 
// lower left hand corner instead of the upper left hand 
// corner. 
void Grib2Nc::_reOrderNS2SN (fl32 *data, int numX, int numY)
{
  size_t j = 0;
  size_t length = (size_t)numX * (size_t)numY;
  fl32 *bufPtr = new fl32[length];
  memcpy(bufPtr, data, length*sizeof(fl32));
  for (int y = numY - 1; y >= 0; y--) {
    for (int x = 0; x < numX; x++) {
      data[j] = bufPtr[(y * numX) + x];
      j++;
    }
  }

  delete [] bufPtr;

}

//
// Reorders rows that scan in the opposite direction
// to rows that scan in the same direction.
// Grid size remains the same.
void Grib2Nc::_reOrderAdjacentRows (fl32 *data, int numX, int numY)
{
  size_t length = (size_t)numX * (size_t)numY;
  fl32 *bufPtr = new fl32[length];
  memcpy(bufPtr, data, length*sizeof(fl32));
  int halfX = (numX / 2)+1;
  for (int y = 1; y < numY; y += 2) {
    for (int x = 0;  x < halfX; x++) {
      data[(y * numX) + x + ((halfX - x - 1)*2)] = bufPtr[(y * numX) + x];
    }
    for (int x = halfX+1;  x < numX; x++) {
      data[(y * numX) + x - ((x - halfX + 1)*2)] = bufPtr[(y * numX) + x];
    }
  }

  delete [] bufPtr;

}

//
// Selects the field name. Uses the abbreviated name from the Grib2 product table along
// with the abbreviated level name to form a unique name for this field.
// If there is an mdv field name specfied in the parameter file it is used instead.
void Grib2Nc::_setFieldNames(int paramsIndex)
{
  char defaultName[100];
  strncpy(defaultName, _GribRecord->summary->name.c_str(), 100);
  if(strcmp(_GribRecord->summary->levelType.c_str(), "unknown primary surface type") != 0) {
    strcat(defaultName, "_");
    strcat(defaultName, _GribRecord->summary->levelType.c_str());
  }
  if(_paramsPtr->process_everything || paramsIndex < 0 || paramsIndex >= _paramsPtr->output_fields_n) {
    _fieldInfo.name = defaultName;
  } else {

    if (string(_paramsPtr->_output_fields[paramsIndex].field_name).size() > 0) {
      _fieldInfo.name = _paramsPtr->_output_fields[paramsIndex].field_name;
    }
    else {
      _fieldInfo.name = defaultName;
    }
    _fieldInfo.standardName = _paramsPtr->_output_fields[paramsIndex].standard_name;
  }
  
  _fieldInfo.nameLong = _GribRecord->summary->longName;
  _fieldInfo.generatingCenter = _GribRecord->ids->getGeneratingCenterName();
  _fieldInfo.generatingProcess = _GribRecord->pds->getGeneratingProcess();

}

//
// Applies limits on range of values in _data, specified by the upper_range_limit 
// and lower_range_limit values from the parameter file. 
void Grib2Nc::_limitDataRange(int paramsIndex, fl32 *dataPtr)
{
  if(paramsIndex < 0 || paramsIndex >= _paramsPtr->output_fields_n)
    return;

  fl32 upperLimit = _paramsPtr->_output_fields[paramsIndex].upper_range_limit;
  fl32 lowerLimit = _paramsPtr->_output_fields[paramsIndex].lower_range_limit;

  if(lowerLimit > upperLimit) {
    fl32 tmp = lowerLimit;
    lowerLimit = upperLimit;
    upperLimit = tmp;
  }

  fl32 replacementValue = _fieldInfo.secondaryMissing;
  Params::qc_default_t qcDefaultType = 
    _paramsPtr->_output_fields[paramsIndex].qc_default_type;

  if (qcDefaultType == Params::UNKNOWN_VALUE)
    replacementValue = _fieldInfo.missing;
  else if (qcDefaultType == Params::USER_DEFINED)
    replacementValue = _paramsPtr->_output_fields[paramsIndex].qc_default_value;

  //
  // no limits on this data
  if ((upperLimit == 0.0) && (lowerLimit == 0.0)) {
    return;
  }
  
  fl32* flPtr = (fl32*) dataPtr;
  size_t numPts = _fieldInfo.gridInfo.nx*_fieldInfo.gridInfo.ny*_fieldInfo.vlevelInfo.nz;
  
  for (size_t j = 0; j < numPts; j++,  flPtr++) {
    if((*flPtr < lowerLimit) || (*flPtr > upperLimit)) {
      *flPtr = replacementValue;
    }
  }

  _fieldInfo.min_value = lowerLimit;
  _fieldInfo.max_value = upperLimit;
  
}

//
// Performs simple unit conversions on the vertical level
// Or replaces the vertical levels with user defined.
//
void Grib2Nc::_convertVerticalUnits(int paramsIndex)
{
  // Set to User defined value if set in parameter file
  if(strlen(_paramsPtr->override_vlevels) > 0 &&
     strcmp(_GribRecord->summary->levelType.c_str(), _paramsPtr->override_vlevels) == 0 )
  {  
    if(_fieldInfo.vlevelInfo.nz != _paramsPtr->vlevel_array_n)
      cerr << "WARNING: Override_vlevels size does not match parameter " << _GribRecord->summary->name.c_str() << endl;
    else {
      _fieldInfo.vlevelInfo.longName     = _paramsPtr->vlevel_info.longName;
      _fieldInfo.vlevelInfo.standardName = _paramsPtr->vlevel_info.standardName;
      _fieldInfo.vlevelInfo.units        = _paramsPtr->vlevel_info.units;
      for(int a = 0; a < _fieldInfo.vlevelInfo.nz && a < _paramsPtr->vlevel_array_n; a++)
	_fieldInfo.vlevelInfo.zData[a]   = _paramsPtr->_vlevel_array[a];
      return;
    }
  }
 
  // Grib2 pressure levels come in Pa, convert to hPa
  if(_fieldInfo.vlevelInfo.units.compare("pa") == 0) {
    _fieldInfo.vlevelInfo.units = "hPa";
    for(int a = 0; a < _fieldInfo.vlevelInfo.nz; a++)
      _fieldInfo.vlevelInfo.zData[a] *= PASCAL_TO_HECTOPASCAL;
  }
  // Grib2 vertical levels come in m, convert to km
  if(_fieldInfo.vlevelInfo.units.compare("m") == 0) {
    _fieldInfo.vlevelInfo.units = "km";
    for(int a = 0; a < _fieldInfo.vlevelInfo.nz; a++)
      _fieldInfo.vlevelInfo.zData[a] *= M_TO_KM;;
  }

}

//
// Performs simple unit conversions, which are prescribed in the parameter file
//
void Grib2Nc::_convertUnits(int paramsIndex, fl32 *dataPtr)
{

  float scaleFactor = 1.0;
  float offsetFactor = 0.0;
  string theUnits = "";

  // provide the default unit if we're not converting
  _fieldInfo.units = _GribRecord->summary->units;

  // Attempt to get missing data value correct.
  // Grib2 does not allow a missing value to be specified unless complex packing is used.
  fl32 firstVal = ((fl32*)dataPtr)[0];
  if(_fieldInfo.missing == -9999.0 && 
     (firstVal == -1 || firstVal == -99 || firstVal == -999) ) {
    _fieldInfo.missing = firstVal;
  }

  if(paramsIndex >= 0 && paramsIndex < _paramsPtr->output_fields_n) {

    switch (_paramsPtr->_output_fields[paramsIndex].units) {
      case Params::MPS_TO_KNOTS:
	scaleFactor = MPS_TO_KNOTS;
	theUnits = "kts";
	break;
      case Params::M_TO_KM:
	scaleFactor = M_TO_KM;
	theUnits = "km";
	break;
      case Params::M_TO_FT:
	scaleFactor = M_TO_FT;
	theUnits = "ft";
	break;
      case Params::M_TO_KFT:
        scaleFactor = M_TO_KFT;
        theUnits = "kft";
        break;
      case Params::M_TO_MI:
	scaleFactor = M_TO_MI;
	theUnits = "miles";
	break;
      case Params::M_TO_100FT:
	scaleFactor = M_TO_100FT;
	theUnits = "ft(x100)";
	break;
      case Params::PASCALS_TO_MBAR:
	scaleFactor = PASCALS_TO_MBARS;
	theUnits = "mbar";
	break;
      case Params::KELVIN_TO_CELCIUS:
	offsetFactor = KELVIN_TO_CELCIUS;
	theUnits = "C";
	break;
      case Params::KGPKG_TO_GPKG:
	scaleFactor = KG_TO_G;
	theUnits = "g/kg";
	break;
      case Params::PERCENT_TO_FRACTION:
	scaleFactor = PERCENT_TO_FRAC;
	theUnits = "%";
	break;
      case Params::FRACTION_TO_PERCENT:
	scaleFactor = 1.0/PERCENT_TO_FRAC;
	theUnits = "%";
	break;
      case Params::NO_CHANGE:
      default:
	theUnits = _GribRecord->summary->units;
	break;
    }
    
    fl32* flPtr = (fl32*)dataPtr;
    size_t numPts = _fieldInfo.gridInfo.nx*_fieldInfo.gridInfo.ny*_fieldInfo.vlevelInfo.nz;

    if (scaleFactor != 1.0) {
      
      for (size_t j = 0; j < numPts; j++,  flPtr++) {
	if((*flPtr != _fieldInfo.secondaryMissing) && (*flPtr != _fieldInfo.missing)) {
	  *flPtr *= scaleFactor;
	}
      }
    
    }
    
    if (offsetFactor != 0.0) {
      
      for (size_t j = 0; j < numPts; j++,  flPtr++) {
	if((*flPtr != _fieldInfo.secondaryMissing) && (*flPtr != _fieldInfo.missing)) {
	  *flPtr += offsetFactor;
	}
      }
      
    }
    
    _fieldInfo.units = theUnits;
    
  }

}

//
// Performs data encoding if requested
fl32 *Grib2Nc::_encode(fl32 *dataPtr, Params::data_pack_t output_encoding)
{
  //
  // Calculate min and max for every field
  fl32 min_val = 1.0e99;
  fl32 max_val = -1.0e99;
  fl32 missing = _fieldInfo.missing;
  fl32 bad = _fieldInfo.secondaryMissing;
  size_t npoints = (size_t)_fieldInfo.vlevelInfo.nz*(size_t)_fieldInfo.gridInfo.nx*(size_t)_fieldInfo.gridInfo.ny;
  fl32 *val = dataPtr;
  
  for (size_t i = 0; i < npoints; i++, val++) {
    fl32 this_val = *val;
    if (this_val != missing && this_val != bad) {
      min_val = MIN(min_val, this_val);
      max_val = MAX(max_val, this_val);
    }
  }
  _fieldInfo.min_value = min_val;
  _fieldInfo.max_value = max_val;
  
  if (output_encoding != Params::DATA_PACK_NONE)
  {    
    if (output_encoding == Params::DATA_PACK_SHORT) {
      dataPtr = (fl32*) _float32_to_int8(dataPtr);
    } else if (output_encoding == Params::DATA_PACK_BYTE) {
      dataPtr = (fl32*)  _float32_to_int16(dataPtr);
    }
  }
  return dataPtr;
}


// 
// Encode FLOAT32 to INT8
void *Grib2Nc::_float32_to_int8(fl32 *inDataPtr)
{

  // set missing and bad

  fl32 in_missing = _fieldInfo.missing;
  fl32 in_bad =  _fieldInfo.secondaryMissing;
  ui08 out_missing = 0;
  ui08 out_bad;
  if (in_missing == in_bad) {
    out_bad = out_missing;
  } else {
    out_bad = 1;
  }

  // compute scale and bias
  
  double scale, bias;

  if (_fieldInfo.max_value == _fieldInfo.min_value) {
    
    // scale = 1.0;
    // bias = _fieldHeader.min_value - 4.0;

    if (_fieldInfo.max_value == 0.0) {
      scale = 1.0;
    } else {
      scale = fabs(_fieldInfo.max_value);
    }
    bias = _fieldInfo.min_value - 4.0 * scale;
    
  } else {
    
    double range = _fieldInfo.max_value - _fieldInfo.min_value;
    scale = range / 250.0;
    bias = _fieldInfo.min_value - scale * 4.0;
    
  }
  
  // allocate the output buffer

  size_t npoints = _fieldInfo.gridInfo.nx * _fieldInfo.gridInfo.ny * _fieldInfo.vlevelInfo.nz;
  size_t output_size = npoints * sizeof(ui08);
  void *outDataPtr = new ui08[output_size];

  // convert data
  
  fl32 *in = inDataPtr;
  ui08 *out = (ui08 *) outDataPtr;
  size_t nBad = 0;

  for (size_t i = 0; i < npoints; i++, in++, out++) {
    fl32 in_val = *in;
    if (in_val == in_missing) {
      *out = out_missing;
    } else if (in_val == in_bad) {
      *out = out_bad;
    } else {
      int out_val = (int) ((in_val - bias) / scale + 0.49999);
      if (out_val > 255) {
        nBad++;
 	*out = 255;
      } else if (out_val < 3) {
        nBad++;
 	*out = 3;
      } else {
	*out = (ui08) out_val;
      }
    }
  } // i */

  if (nBad > 0) {
    cerr << "ERROR - float32_to_int8" << endl;
    cerr << "  Out of range data found, field: " << _fieldInfo.name << endl;
    cerr << "  n points: " << nBad << endl;
    cerr << "  Replaced with min or max values as appropriate" << endl;
  }

  // We do this encoding in unsigned int, but in netcdf it will be a signed byte
  // adjust info accordingly
  _fieldInfo.ncType = Params::DATA_PACK_BYTE;
  _fieldInfo.missing = (fl32)out_missing - 128;
  _fieldInfo.secondaryMissing = (fl32)out_bad - 128;
  _fieldInfo.max_value = 127;
  _fieldInfo.min_value = -127;
  _fieldInfo.scaleFactor = (fl32) scale;
  _fieldInfo.addOffset = (fl32)(bias - _fieldInfo.min_value * scale);
  
  delete [] inDataPtr;
  return outDataPtr;
}


//
// encode FLOAT32 to INT16
//
void *Grib2Nc::_float32_to_int16(fl32 *inDataPtr)  
{

  // set missing and bad

  fl32 in_missing = _fieldInfo.missing;
  fl32 in_bad =  _fieldInfo.secondaryMissing;
  ui16 out_missing = 0;
  ui16 out_bad;
  if (in_missing == in_bad) {
    out_bad = out_missing;
  } else {
    out_bad = 1;
  }

  // compute scale and bias

  double scale, bias;

  if (_fieldInfo.max_value == _fieldInfo.min_value) {
    
    scale = 1.0;
    bias = _fieldInfo.min_value - 20.0;
    
  } else {
    
    double range = _fieldInfo.max_value - _fieldInfo.min_value;
    scale = range / 65500.0;
    bias = _fieldInfo.min_value - scale * 20.0;

  }
  
  // allocate the output buffer
  
  size_t npoints = _fieldInfo.gridInfo.nx * _fieldInfo.gridInfo.ny * _fieldInfo.vlevelInfo.nz;
  size_t output_size = npoints * sizeof(ui16);
  void *outDataPtr = new ui16[npoints];

  // convert data
  
  fl32 *in = (fl32 *) inDataPtr;
  ui16 *out = (ui16 *) outDataPtr;
  size_t nBad = 0;

  for (size_t i = 0; i < npoints; i++, in++, out++) {
    fl32 in_val = *in;
    if (in_val == in_missing) {
      *out = out_missing;
    } else if (in_val == in_bad) {
      *out = out_bad;
    } else {
      int out_val = (int) ((in_val - bias) / scale + 0.49999);
      if (out_val > 65535) {
        nBad++;
 	*out = 65535;
      } else if (out_val < 20) {
        nBad++;
 	*out = 20;
      } else {
	*out = (ui16) out_val;
      }
    }
  } // i */

  if (nBad > 0) {
    cerr << "ERROR - MdvxField::_float32_to_int16" << endl;
    cerr << "  Out of range data found, field: " << _fieldInfo.name << endl;
    cerr << "  n points: " << nBad << endl;
    cerr << "  Replaced with min or max values as appropriate" << endl;
  }

  // adjust header
  
  _fieldInfo.ncType = Params::DATA_PACK_SHORT;
  _fieldInfo.missing = (fl32)out_missing - 32768;
  _fieldInfo.secondaryMissing = (fl32)out_bad - 32768;
  _fieldInfo.max_value = 32767;
  _fieldInfo.min_value = -32767;
  _fieldInfo.scaleFactor = (fl32) scale;
  _fieldInfo.addOffset = (fl32)(bias - _fieldInfo.min_value * scale);
  
  delete [] inDataPtr;
  return outDataPtr;
}


float inline getInd(double out, double in1, double in2)
{
  float ind, diff;
  if(in1 < in2) {
    double tmp = in1;
    in1 = in2;
    in2 = tmp;
  }
  diff = (in1 - out) + (out - in2);
  ind = (in1 - out) / diff;
  return ind;
}


fl32 *Grib2Nc::_reMapReducedOrGaussian(fl32 *data)
{

  fl32 *lats;
  int nx, ny;
  float dx, dy, minx, miny;
  fl32 **lons;
  int *nlons;
  int *tlons;

  si32 projID = _GribRecord->gds->getGridID();

  if(projID == Grib2::GDS::GAUSSIAN_LAT_LON_PROJ_ID) {
    
    Grib2::GausLatLonProj *latlonProj = (Grib2::GausLatLonProj *)_GribRecord->gds->getProjection();
    latlonProj->getGaussianLats(&lats);
    if(lats == NULL) {
      cerr << "ERROR: Calculation of Gaussian Latitude failure." << endl << flush;
      return NULL;
    }
    nx = latlonProj->_maxNi;
    ny = latlonProj->_nj;
    dx = latlonProj->_di;
    if(dx == -1)
      dx =  (latlonProj->_lo2 + (latlonProj->_lo2 / (double)(latlonProj->_maxNi-1)) - latlonProj->_lo1) / latlonProj->_maxNi;
    dy = 180. / (latlonProj->_nParalells*2);
    minx = latlonProj->_lo1;
    miny = latlonProj->_la1;
    if(latlonProj->_lo2 < latlonProj->_lo1)
      dx *= -1.0;
    if(latlonProj->_la2 < latlonProj->_la1)
      dy *= -1.0;

    lons = new fl32*[ny];
    nlons = new int[ny];
    tlons = new int[ny];
    for(int y = 0; y < ny; y++) {
      nlons[y] = latlonProj->getQuasiLons(&(lons[y]), y);
      if(y > 0) 
	tlons[y] = nlons[y-1] + tlons[y-1];
      else
	tlons[0] = 0;
    }

  }  else {
    cerr << "ERROR: Unsupported Reduced grid type, cannot remap to regular lat/lon grid." << endl << flush;
    return NULL;
  }

  Pjg outproj;
  outproj.initLatlon(nx, ny, 1, dx, dy, 1.0, minx, miny, 0.0);

  float *odata = new float[nx*ny];

  double lat, lon, x;
  int jlat = 0, ilon1, ilon2;
  double p1, p2, p3, p4;

  for(int j = 0; j < ny; j++) 
  {
    outproj.xyIndex2latlon(0, j, lat, lon);
    if(jlat < _fieldInfo.gridInfo.ny -1)
      x = getInd(lat, lats[jlat], lats[jlat+1]);
    else
      x = 0.0;

    int ix = floor(x);

    for(int i = 0; i < nx; i++)
    {
      outproj.xyIndex2latlon(i, j, lat, lon);
      double y1, y2;

      odata[(j*nx)+i] = 0.0;

      ilon1 = 0;
      for(int ii = 0; ii < nlons[jlat]; ii++)
	if(lons[jlat][ii] <= lon)
	  ilon1++;
      ilon1--;

      p1 = data[ilon1+tlons[jlat]];

      if(ilon1 == nlons[jlat]-1) 
      {
	p2 = 0.0;
	y1 = 1.0;
      } else {
	p2 = data[ilon1+1+tlons[jlat]];
	y1 = getInd(lon, lons[jlat][ilon1], lons[jlat][ilon1+1]);
      } 
      
      y2 = 0.0;
      if(jlat == _fieldInfo.gridInfo.ny -1) 
      {
	p3 = 0.0;
	p4 = 0.0;
      } else {
	ilon2 = 0;
	for(int ii = 0; ii < nlons[jlat+1]; ii++)
	  if(lons[jlat+1][ii] <= lon)
	    ilon2++;
	ilon2--;
	
	p3 = data[ilon2+tlons[jlat+1]];
	if(ilon2 == nlons[jlat+1]-1) 
	{
	  p4 = 0.0;
	  y2 = 1.0;
	} else {
	  p4 = data[ilon2+1+tlons[jlat+1]];
	  y2 = getInd(lon, lons[jlat+1][ilon2], lons[jlat+1][ilon2+1]);
	}
	
	
      }


      if(p1 == _fieldInfo.missing || p1 == _fieldInfo.secondaryMissing ||
	 p2 == _fieldInfo.missing || p2 == _fieldInfo.secondaryMissing ||
	 p3 == _fieldInfo.missing || p3 == _fieldInfo.secondaryMissing ||
	 p4 == _fieldInfo.missing || p4 == _fieldInfo.secondaryMissing)
	odata[(j*nx)+i] = _fieldInfo.missing;
      else
	odata[(j*nx)+i] = (p1 * y1 + p2 * (1-y1)) * (1-x) + (p3 * y2 + p4 * (1-y2)) * x;      

    }
    jlat++;
  }

  delete [] lats;
  for(int y = 0; y < ny; y++)
    delete [] lons[y];
  delete[] lons;
  delete[] nlons;
  delete[] tlons;

  return odata;
}
