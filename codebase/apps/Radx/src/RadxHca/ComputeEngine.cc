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
// ComputeEngine.cc
//
// Mike Dixon, EOL, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// March 2012
//
///////////////////////////////////////////////////////////////
//
// ComputeEngine computation - for multi-threading
// There is one object per thread.
//
///////////////////////////////////////////////////////////////

#include "ComputeEngine.hh"
#include <toolsa/os_config.h>
#include <toolsa/file_io.h>
#include <rapmath/trig.h>
#include <rapmath/umath.h>
#include <radar/FilterUtils.hh>
#include <Radx/RadxRay.hh>
#include <Radx/RadxField.hh>
#include <cerrno>
using namespace std;
pthread_mutex_t ComputeEngine::_debugPrintMutex = PTHREAD_MUTEX_INITIALIZER;
const double ComputeEngine::missingDbl = -9999.0;

// Constructor

ComputeEngine::ComputeEngine(const Params &params,
                             int id,
                             const TempProfile &tempProfile)  :
        _params(params),
        _id(id),
        _tempProfile(tempProfile)
  
{

  OK = true;
  
  // initialize kdp, pid
  // set up interest maps etc

  _kdpInit();
  
  if (_pidInit()) {
    OK = false;
  }

  if (_seaclutInit()) {
    OK = false;
  }

  // create the hca interest maps

  _initInterestMaps();
  if (_createInterestMaps()) {
    OK = FALSE;
  } else {
    if (id == 0 && _params.debug >= Params::DEBUG_VERBOSE) {
      _printInterestMaps(cerr);
    }
  }

}
  
// destructor

ComputeEngine::~ComputeEngine()

{

  // clean up interest maps
  
  for (size_t ii = 0; ii < HcaInterestMap::nClasses; ii++) {
    for (size_t jj = 0; jj < HcaInterestMap::nFeatures; jj++) {
      delete _imaps[ii][jj];
    }
  }

}

//////////////////////////////////////////////////
// compute for given input ray
// storing results in derived ray
//
// Creates derived ray and returns it.
// It must be freed by caller.
//
// Returns NULL on error.

RadxRay *ComputeEngine::compute(RadxRay *inputRay,
                                double radarHtKm,
                                double wavelengthM)
{

  // set ray-specific metadata
  
  _nGates = inputRay->getNGates();
  _startRangeKm = inputRay->getStartRangeKm();
  _gateSpacingKm = inputRay->getGateSpacingKm();
  _azimuth = inputRay->getAzimuthDeg();
  _elevation = inputRay->getElevationDeg();
  _timeSecs = inputRay->getTimeSecs();
  _nanoSecs = inputRay->getNanoSecs();
  _nyquist = inputRay->getNyquistMps();

  // initialize

  _radarHtKm = radarHtKm;
  _wavelengthM = wavelengthM;
  _atmos.setAttenCrpl(_wavelengthM * 100.0);

  // create moments ray
  
  RadxRay *derivedRay = new RadxRay;
  derivedRay->copyMetaData(*inputRay);

  // allocate arrays for computing derived fields,
  // and load them up
  
  _allocArrays();
  _loadMomentsArrays(inputRay);
  
  // compute kdp
  
  _kdpCompute();

  // locate sea clutter

  if (_params.locate_sea_clutter) {
    _locateSeaClutter();
  }

  // compute pid

  if (_params.compute_pid) {
    _pidCompute();
  }

  // compute hca

  if (_params.compute_hca) {
    _hcaCompute();
  }
  
  // load output fields into the moments ray
  
  _loadOutputFields(inputRay, derivedRay);

  // set max range

  if (_params.set_max_range) {
    derivedRay->setMaxRangeKm(_params.max_range_km);
  }

  return derivedRay;

}

///////////////////////////////
// load up fields in output ray

void ComputeEngine::_loadOutputFields(RadxRay *inputRay,
                                      RadxRay *derivedRay)

{

  // initialize array pointers

  const double *dbzForKdp = _kdp.getDbz();
  const double *zdrForKdp = _kdp.getZdr();
  const double *rhohvForKdp = _kdp.getRhohv();
  const double *snrForKdp = _kdp.getSnr();
  const double *zdrSdevForKdp = _kdp.getZdrSdev();
  const bool *validFlagForKdp = _kdp.getValidForKdp();

  const double *phidpForKdp = _kdp.getPhidp();
  const double *phidpMeanForKdp = _kdp.getPhidpMean();
  const double *phidpMeanUnfoldForKdp = _kdp.getPhidpMeanUnfold();
  const double *phidpSdevForKdp = _kdp.getPhidpSdev();
  const double *phidpJitterForKdp = _kdp.getPhidpJitter();
  const double *phidpUnfoldForKdp = _kdp.getPhidpUnfold();
  const double *phidpFiltForKdp = _kdp.getPhidpFilt();
  const double *phidpCondForKdp = _kdp.getPhidpCond();
  const double *phidpCondFiltForKdp = _kdp.getPhidpCondFilt();

  const double *psob = _kdp.getPsob();

  const double *dbzAtten = _kdp.getDbzAttenCorr();
  const double *zdrAtten = _kdp.getZdrAttenCorr();
  
  const double *snrMeanSeaclut = _seaclut.getSnrMean();
  const double *rhohvMeanSeaclut = _seaclut.getRhohvMean();
  const double *phidpSdevSeaclut = _seaclut.getPhidpSdev();
  const double *zdrSdevSeaclut = _seaclut.getZdrSdev();

  const double *rhohvMeanInterestSeaclut = _seaclut.getRhohvMeanInterest();
  const double *phidpSdevInterestSeaclut = _seaclut.getPhidpSdevInterest();
  const double *zdrSdevInterestSeaclut = _seaclut.getZdrSdevInterest();
  const double *dbzElevGradientInterestSeaclut = _seaclut.getDbzElevGradientInterest();

  const bool *seaclutFlag = _seaclut.getClutFlag();

  RadxField *rayHtField = inputRay->getField(_params.ray_height_field_name);
  const Radx::fl32 *rayHt = NULL;
  if (rayHtField != NULL) {
    rayHtField->convertToFl32();
    rayHt = rayHtField->getDataFl32();
  }
  RadxField *dbzGradientField =
    inputRay->getField(_params.dbz_elevation_gradient_field_name);
  const Radx::fl32 *dbzGradient = NULL;
  if (dbzGradientField != NULL) {
    dbzGradientField->convertToFl32();
    dbzGradient = dbzGradientField->getDataFl32();
  }

  // load up output data

  for (int ifield = 0; ifield < _params.output_fields_n; ifield++) {

    const Params::output_field_t &ofld = _params._output_fields[ifield];

    // fill data array
    
    TaArray<Radx::fl32> data_;
    Radx::fl32 *data = data_.alloc(derivedRay->getNGates());
    for (size_t igate = 0; igate < derivedRay->getNGates(); igate++) {
      data[igate] = Radx::missingFl32;
    }
    Radx::fl32 *datp = data;
    
    for (size_t igate = 0; igate < derivedRay->getNGates();  igate++, datp++) {
      
      switch (ofld.id) {

        // computed fields

        case Params::SNR:
          *datp = _snrArray[igate];
          break;
        case Params::DBZ:
          *datp = _dbzArray[igate];
          break;
        case Params::VEL:
          *datp = _velArray[igate];
          break;
        case Params::WIDTH:
          *datp = _widthArray[igate];
          break;
        case Params::NCP:
          *datp = _ncpArray[igate];
          break;
        case Params::ZDR:
          *datp = _zdrArray[igate];
          break;
        case Params::LDR:
          *datp = _ldrArray[igate];
          break;
        case Params::RHOHV:
          *datp = _rhohvArray[igate];
          break;
        case Params::PHIDP:
          *datp = _phidpArray[igate];
          break;

        // hca

        case Params::SD_DBZ2:
          *datp = _sdDbzArray[igate];
          break;
        case Params::SD_DBZ:
          *datp = _tdDbzArray[igate];
          break;
        case Params::SD_PHIDP:
        case Params::SD_PHIDP2:
          *datp = _tdPhidpArray[igate];
          break;

        case Params::GC_INTEREST:
          *datp = _gcInterest[igate];
          break;
        case Params::BS_INTEREST:
          *datp = _bsInterest[igate];
          break;
        case Params::DS_INTEREST:
          *datp = _dsInterest[igate];
          break;
        case Params::WS_INTEREST:
          *datp = _wsInterest[igate];
          break;
        case Params::CR_INTEREST:
          *datp = _crInterest[igate];
          break;
        case Params::GR_INTEREST:
          *datp = _grInterest[igate];
          break;
        case Params::BD_INTEREST:
          *datp = _bdInterest[igate];
          break;
        case Params::RA_INTEREST:
          *datp = _raInterest[igate];
          break;
        case Params::HR_INTEREST:
          *datp = _hrInterest[igate];
          break;
        case Params::RH_INTEREST:
          *datp = _rhInterest[igate];
          break;
        case Params::HCA:
          {
            int hca = _hcaArray[igate];
            if (hca > 0) {
              *datp = hca;
            } else {
              *datp = missingDbl;
            }
          }
          break;

          // kdp
          
        case Params::DBZ_FOR_KDP:
          *datp = dbzForKdp[igate];
          break;
        case Params::ZDR_FOR_KDP:
          *datp = zdrForKdp[igate];
          break;
        case Params::RHOHV_FOR_KDP:
          *datp = rhohvForKdp[igate];
          break;
        case Params::SNR_FOR_KDP:
          *datp = snrForKdp[igate];
          break;
        case Params::ZDR_SDEV_FOR_KDP:
          *datp = zdrSdevForKdp[igate];
          break;
        case Params::VALID_FLAG_FOR_KDP:
          if (validFlagForKdp[igate]) {
            *datp = 1.0;
          } else {
            *datp = 0.0;
          }
          break;
        case Params::KDP:
          *datp = _kdpArray[igate];
          break;
        case Params::KDP_COND:
          *datp = _kdpCondArray[igate];
          break;
        case Params::KDP_LOG:
          *datp = _kdpLogArray[igate];
          break;
        case Params::PSOB:
          *datp = psob[igate];
          break;

        case Params::PHIDP_FOR_KDP:
          *datp = phidpForKdp[igate];
          break;
        case Params::PHIDP_MEAN_FOR_KDP:
          *datp = phidpMeanForKdp[igate];
          break;
        case Params::PHIDP_MEAN_UNFOLD_FOR_KDP:
          *datp = phidpMeanUnfoldForKdp[igate];
          break;
        case Params::PHIDP_SDEV_FOR_KDP:
          *datp = phidpSdevForKdp[igate];
          break;
        case Params::PHIDP_JITTER_FOR_KDP:
          *datp = phidpJitterForKdp[igate];
          break;
        case Params::PHIDP_UNFOLD_FOR_KDP:
          *datp = phidpUnfoldForKdp[igate];
          break;
        case Params::PHIDP_FILT_FOR_KDP:
          *datp = phidpFiltForKdp[igate];
          break;
        case Params::PHIDP_COND_FOR_KDP:
          *datp = phidpCondForKdp[igate];
          break;
        case Params::PHIDP_COND_FILT_FOR_KDP:
          *datp = phidpCondFiltForKdp[igate];
          break;

          // attenuation

        case Params::DBZ_ATTEN_CORRECTION:
          *datp = dbzAtten[igate];
          break;
        case Params::ZDR_ATTEN_CORRECTION:
          *datp = zdrAtten[igate];
          break;
        case Params::DBZ_ATTEN_CORRECTED:
          if (dbzAtten[igate] != missingDbl &&
              _dbzArray[igate] != missingDbl) {
            *datp = dbzAtten[igate] + _dbzArray[igate];
          }
          break;
        case Params::ZDR_ATTEN_CORRECTED:
          if (zdrAtten[igate] != missingDbl &&
              _zdrArray[igate] != missingDbl) {
            *datp = zdrAtten[igate] + _zdrArray[igate];
          }
          break;

        case Params::RAY_HEIGHT:
          if (rayHt != NULL) {
            *datp = rayHt[igate];
          }
          break;
        case Params::DBZ_ELEV_GRADIENT_SEACLUT:
          if (dbzGradient != NULL) {
            *datp = dbzGradient[igate];
          }
          break;

        case Params::SNR_MEAN_SEACLUT:
          *datp = snrMeanSeaclut[igate];
          break;
        case Params::RHOHV_MEAN_SEACLUT:
          *datp = rhohvMeanSeaclut[igate];
          break;
        case Params::PHIDP_SDEV_SEACLUT:
          *datp = phidpSdevSeaclut[igate];
          break;
        case Params::ZDR_SDEV_SEACLUT:
          *datp = zdrSdevSeaclut[igate];
          break;
        case Params::RHOHV_MEAN_INTEREST_SEACLUT:
          *datp = rhohvMeanInterestSeaclut[igate];
          break;
        case Params::PHIDP_SDEV_INTEREST_SEACLUT:
          *datp = phidpSdevInterestSeaclut[igate];
          break;
        case Params::ZDR_SDEV_INTEREST_SEACLUT:
          *datp = zdrSdevInterestSeaclut[igate];
          break;
        case Params::DBZ_ELEV_GRADIENT_INTEREST_SEACLUT:
          *datp = dbzElevGradientInterestSeaclut[igate];
          break;
        case Params::SEACLUT_FLAG:
          if (seaclutFlag[igate]) {
            *datp = 1.0;
          } else {
            *datp = 0.0;
          }
          break;

        case Params::TEMP_FOR_PID:
          *datp = _tempForPid[igate];
          break;

        case Params::PARTICLE_ID:
          {
            int pid = _pidArray[igate];
            if (pid > 0) {
              *datp = pid;
            } else {
              *datp = missingDbl;
            }
          }
          break;
          
      } // switch

    } // igate

    // create field
    
    RadxField *field = new RadxField(ofld.name, ofld.units);
    field->setLongName(ofld.long_name);
    field->setStandardName(ofld.standard_name);
    field->setTypeFl32(missingDbl);
    field->addDataFl32(derivedRay->getNGates(), data);
    field->copyRangeGeom(*inputRay);

    // add to ray

    derivedRay->addField(field);

  } // ifield

  // if required, output individual PID particle interest fields

  if (_params.PID_output_particle_interest_fields) {
    
    const vector<NcarParticleId::Particle*> plist = _pid.getParticleList();
    for (size_t ii = 0; ii < plist.size(); ii++) {

      const NcarParticleId::Particle *particle = plist[ii];
      string fieldName = particle->label;
      fieldName += "_interest";

      // fill data array
      
      TaArray<Radx::fl32> data_;
      Radx::fl32 *data = data_.alloc(derivedRay->getNGates());

      const double *interest = particle->gateInterest;
      for (int igate = 0; igate < (int) derivedRay->getNGates(); igate++) {
        data[igate] = interest[igate];
      } // igate
      
      // create field
      
      RadxField *field = new RadxField(fieldName, "");
      field->setTypeFl32(missingDbl);
      field->addDataFl32(derivedRay->getNGates(), data);
      
      // add to ray
      
      derivedRay->addField(field);
      
    } // ii
    
  } // if (_params.PID_output_particle_interest_fields) ...
  
}

//////////////////////////////////////
// initialize KDP
  
void ComputeEngine::_kdpInit()
  
{

  // initialize KDP object

  if (_params.KDP_fir_filter_len == Params::FIR_LEN_125) {
    _kdp.setFIRFilterLen(KdpFilt::FIR_LENGTH_125);
  } else if (_params.KDP_fir_filter_len == Params::FIR_LEN_60) {
    _kdp.setFIRFilterLen(KdpFilt::FIR_LENGTH_60);
  } else if (_params.KDP_fir_filter_len == Params::FIR_LEN_40) {
    _kdp.setFIRFilterLen(KdpFilt::FIR_LENGTH_40);
  } else if (_params.KDP_fir_filter_len == Params::FIR_LEN_30) {
    _kdp.setFIRFilterLen(KdpFilt::FIR_LENGTH_30);
  } else if (_params.KDP_fir_filter_len == Params::FIR_LEN_20) {
    _kdp.setFIRFilterLen(KdpFilt::FIR_LENGTH_20);
  } else {
    _kdp.setFIRFilterLen(KdpFilt::FIR_LENGTH_10);
  }
  _kdp.setNGatesStats(_params.KDP_ngates_for_stats);
  _kdp.setMinValidAbsKdp(_params.KDP_min_valid_abs_kdp);
  if (_params.set_max_range) {
    _kdp.setMaxRangeKm(true, _params.max_range_km);
  }
  _kdp.setNFiltIterUnfolded(_params.KDP_n_filt_iterations_unfolded);
  _kdp.setNFiltIterCond(_params.KDP_n_filt_iterations_conditioned);
  if (_params.KDP_use_iterative_filtering) {
    _kdp.setUseIterativeFiltering(true);
    _kdp.setPhidpDiffThreshold(_params.KDP_phidp_difference_threshold);
  }
  _kdp.setPhidpSdevMax(_params.KDP_phidp_sdev_max);
  _kdp.setPhidpJitterMax(_params.KDP_phidp_jitter_max);
  _kdp.setMinValidAbsKdp(_params.KDP_min_valid_abs_kdp);
  _kdp.checkSnr(_params.KDP_check_snr);
  _kdp.setSnrThreshold(_params.KDP_snr_threshold);
  _kdp.checkRhohv(_params.KDP_check_rhohv);
  _kdp.setRhohvThreshold(_params.KDP_rhohv_threshold);
  if (_params.KDP_check_zdr_sdev) {
    _kdp.checkZdrSdev(true);
  }
  _kdp.setZdrSdevMax(_params.KDP_zdr_sdev_max);
  _kdp.setThresholdForKdpZZdr(_params.KDP_threshold_for_ZZDR);
  _kdp.setMedianFilterLenForKdpZZdr(_params.KDP_median_filter_len_for_ZZDR);
  if (_params.KDP_debug) {
    _kdp.setDebug(true);
  }
  if (_params.KDP_write_ray_files) {
    _kdp.setWriteRayFile(true, _params.KDP_ray_files_dir);
  }

  if (_params.apply_precip_attenuation_correction) {
    if (_params.specify_coefficients_for_attenuation_correction) {
      _kdp.setAttenCoeffs(_params.dbz_attenuation_coefficient,
                          _params.dbz_attenuation_exponent,
                          _params.zdr_attenuation_coefficient,
                          _params.zdr_attenuation_exponent);
    } else {
      _kdp.setComputeAttenCorr(true);
    }
  }

}

////////////////////////////////////////////////
// compute kdp from phidp, using Bringi's method

void ComputeEngine::_kdpCompute()
  
{

  // set up array for range
  
  TaArray<double> rangeKm_;
  double *rangeKm = rangeKm_.alloc(_nGates);
  double range = _startRangeKm;
  for (int ii = 0; ii < _nGates; ii++, range += _gateSpacingKm) {
    rangeKm[ii] = range;
  }

  // compute KDP
  
  _kdp.compute(_timeSecs,
               _nanoSecs / 1.0e9,
               _elevation,
               _azimuth,
               _wavelengthM * 100.0,
               _nGates, 
               _startRangeKm,
               _gateSpacingKm,
               _snrArray,
               _dbzArray,
               _zdrArray,
               _rhohvArray,
               _phidpArray,
               missingDbl);

  const double *kdp = _kdp.getKdp();
  const double *kdpCond = _kdp.getKdpCond();
  
  // put KDP into fields objects
  
  for (int ii = 0; ii < _nGates; ii++) {
    if (kdp[ii] == NAN) {
      _kdpArray[ii] = missingDbl;
      _kdpCondArray[ii] = missingDbl;
      _kdpLogArray[ii] = missingDbl;
    } else {
      _kdpArray[ii] = kdp[ii];
      _kdpCondArray[ii] = kdpCond[ii];
      if (kdp[ii] > 1.0e-3) {
        _kdpLogArray[ii] = log10(kdp[ii]);
      } else {
        _kdpLogArray[ii] = -30.0;
      }
    }
  }

}

//////////////////////////////////////
// Initialize Sea Clutter

int ComputeEngine::_seaclutInit()
  
{
  
  int iret = 0;

  if (_convertInterestParamsToVector
      ("seaclut_rhohv_mean",
       _params._seaclut_rhohv_mean_interest_map,
       _params.seaclut_rhohv_mean_interest_map_n,
       _seaclutImapRhohvMean)) {
    iret = -1;
  } else {
    _seaclut.setInterestMapRhohvMean
      (_seaclutImapRhohvMean,
       _params.seaclut_rhohv_mean_weight);
  }
  
  if (_convertInterestParamsToVector
      ("seaclut_phidp_sdev",
       _params._seaclut_phidp_sdev_interest_map,
       _params.seaclut_phidp_sdev_interest_map_n,
       _seaclutImapPhidpSdev)) {
    iret = -1;
  } else {
    _seaclut.setInterestMapPhidpSdev
      (_seaclutImapPhidpSdev,
       _params.seaclut_phidp_sdev_weight);
  }
  
  if (_convertInterestParamsToVector
      ("seaclut_zdr_sdev",
       _params._seaclut_zdr_sdev_interest_map,
       _params.seaclut_zdr_sdev_interest_map_n,
       _seaclutImapZdrSdev)) {
    iret = -1;
  } else {
    _seaclut.setInterestMapZdrSdev
      (_seaclutImapZdrSdev,
       _params.seaclut_zdr_sdev_weight);
  }
  
  if (_convertInterestParamsToVector
      ("seaclut_dbz_elev_gradient",
       _params._seaclut_dbz_elev_gradient_interest_map,
       _params.seaclut_dbz_elev_gradient_interest_map_n,
       _seaclutImapDbzElevGradient)) {
    iret = -1;
  } else {
    _seaclut.setInterestMapDbzElevGradient
      (_seaclutImapDbzElevGradient,
       _params.seaclut_dbz_elev_gradient_weight);
  }
  
  _seaclut.setMinSnrDb(_params.seaclut_min_snr_db);
  _seaclut.setMaxElevDeg(_params.seaclut_max_elev_deg);

  return iret;

}

//////////////////////////////////////
// Locate Sea Clutter

void ComputeEngine::_locateSeaClutter()
  
{

  // set up sea clut

  if (_params.debug >= Params::DEBUG_VERBOSE) {
    _seaclut.setDebug(true);
  }

  _seaclut.setNGatesKernel(9);
  _seaclut.setWavelengthM(_wavelengthM);
  _seaclut.setRadarHtM(_radarHtKm * 1000.0);

  _seaclut.setRayProps(_timeSecs,
                       _nanoSecs,
                       _elevation,
                       _azimuth,
                       _nGates,
                       _startRangeKm,
                       _gateSpacingKm);
  
  _seaclut.setFieldMissingVal(missingDbl);
  
  if (_params.SNR_available) {
    _seaclut.setSnrField(_snrArray);
  } else {
    _seaclut.setDbzField(_dbzArray, _params.noise_dbz_at_100km);
  }

  _seaclut.setRhohvField(_rhohvArray);

  _seaclut.setPhidpField(_phidpArray);

  _seaclut.setZdrField(_zdrArray);

  if (_dbzElevGradientAvail) {
    _seaclut.setDbzElevGradientField(_dbzElevGradientArray);
  }

  // locate seaclutter

  _seaclut.locate();

}

//////////////////////////////////////
// initialize pid computations
  
int ComputeEngine::_pidInit()
  
{

  _pid.setSnrThresholdDb(_params.PID_snr_threshold);
  _pid.setSnrUpperThresholdDb(_params.PID_snr_upper_threshold);

  if (_params.PID_apply_median_filter_to_DBZ) {
    _pid.setApplyMedianFilterToDbz(_params.PID_DBZ_median_filter_len);
  }
  if (_params.PID_apply_median_filter_to_ZDR) {
    _pid.setApplyMedianFilterToZdr(_params.PID_ZDR_median_filter_len);
  }

  if (_params.PID_apply_median_filter_to_LDR) {
    _pid.setApplyMedianFilterToLdr(_params.PID_LDR_median_filter_len);
  }
  if (_params.PID_replace_missing_LDR) {
    _pid.setReplaceMissingLdr(_params.PID_LDR_replacement_value);
  }

  if (_params.PID_apply_median_filter_to_RHOHV) {
    _pid.setApplyMedianFilterToRhohv(_params.PID_RHOHV_median_filter_len);
  }
  if (_params.apply_median_filter_to_PID) {
    _pid.setApplyMedianFilterToPid(_params.PID_median_filter_len);
  }

  _pid.setNgatesSdev(_params.PID_ngates_for_sdev);
  _pid.setMinValidInterest(_params.PID_min_valid_interest);
  
  _pid.setMissingDouble(missingDbl);

  _pid.setDebug(false);
  if (_params.debug >= Params::DEBUG_VERBOSE) {
    _pid.setDebug(true);
  }
  if (_params.debug >= Params::DEBUG_EXTRA) {
    _pid.setVerbose(true);
  }
  if (_pid.readThresholdsFromFile(_params.pid_thresholds_file_path)) {
    cerr << "ERROR - RadxPartRain::ComputeEngine::_run" << endl;
    cerr << "  Cannot read in pid thresholds from file: "
         << _params.pid_thresholds_file_path << endl;
    cerr << "  PID will not be computed" << endl;
    return -1;
  }
  
  return 0;
  
}

//////////////////////////////////////
// compute PID

void ComputeEngine::_pidCompute()
  
{
  
  // override temp profile in PID
  
  if (_params.pid_override_temp_profile) {
    const vector<TempProfile::PointVal> &profile = _tempProfile.getProfile();
    if (profile.size() > 0) {
      _pid.setTempProfile(profile);
    }
  }

  // fill temperature array
  
  _pid.fillTempArray(_radarHtKm,
                     _params.override_standard_pseudo_earth_radius,
                     _params.pseudo_earth_radius_ratio,
                     _elevation, _nGates,
                     _startRangeKm,
                     _gateSpacingKm,
                     _tempForPid);

  // compute particle ID
  
  _pid.computePidBeam(_nGates, _snrArray, _dbzArray, 
                      _zdrArray, _kdpArray, _ldrArray, 
                      _rhohvArray, _phidpArray, _tempForPid);
  
  // load results

  memcpy(_pidArray, _pid.getPid(), _nGates * sizeof(int));
  memcpy(_pidInterest, _pid.getInterest(), _nGates * sizeof(double));

}

//////////////////////////////////////
// alloc arrays
  
void ComputeEngine::_allocArrays()
  
{

  _snrArray = _snrArray_.alloc(_nGates);
  _dbzArray = _dbzArray_.alloc(_nGates);
  _velArray = _velArray_.alloc(_nGates);
  _widthArray = _widthArray_.alloc(_nGates);
  _ncpArray = _ncpArray_.alloc(_nGates);
  _zdrArray = _zdrArray_.alloc(_nGates);
  _ldrArray = _ldrArray_.alloc(_nGates);
  _kdpArray = _kdpArray_.alloc(_nGates);
  _kdpCondArray = _kdpCondArray_.alloc(_nGates);
  _rhohvArray = _rhohvArray_.alloc(_nGates);
  _phidpArray = _phidpArray_.alloc(_nGates);
  _dbzElevGradientArray = _dbzElevGradientArray_.alloc(_nGates);

  _kdpLogArray = _kdpLogArray_.alloc(_nGates);
  _sdDbzArray = _sdDbzArray_.alloc(_nGates);
  _tdDbzArray = _tdDbzArray_.alloc(_nGates);
  _tdPhidpArray = _tdPhidpArray_.alloc(_nGates);

  _gcInterest = _gcInterest_.alloc(_nGates);
  _bsInterest = _bsInterest_.alloc(_nGates);
  _dsInterest = _dsInterest_.alloc(_nGates);
  _wsInterest = _wsInterest_.alloc(_nGates);
  _crInterest = _crInterest_.alloc(_nGates);
  _grInterest = _grInterest_.alloc(_nGates);
  _bdInterest = _bdInterest_.alloc(_nGates);
  _raInterest = _raInterest_.alloc(_nGates);
  _hrInterest = _hrInterest_.alloc(_nGates);
  _rhInterest = _rhInterest_.alloc(_nGates);
  _hcaArray = _hcaArray_.alloc(_nGates);

  _pidArray = _pidArray_.alloc(_nGates);
  _pidInterest = _pidInterest_.alloc(_nGates);
  _tempForPid = _tempForPid_.alloc(_nGates);

}

/////////////////////////////////////////////////////
// load momemts arrays ready for KDP, PID and PRECIP
  
int ComputeEngine::_loadMomentsArrays(RadxRay *inputRay)
  
{
  
  if (_loadFieldArray(inputRay, _params.DBZ_field_name,
                      true, _dbzArray)) {
    return -1;
  }

  if (_loadFieldArray(inputRay, _params.VEL_field_name,
                      true, _velArray)) {
    return -1;
  }

  if (_params.WIDTH_available) {
    if (_loadFieldArray(inputRay, _params.WIDTH_field_name,
                        true, _widthArray)) {
      return -1;
    }
  } else {
    for (int igate = 0; igate < _nGates; igate++) {
      _widthArray[igate] = missingDbl;
    }
  }

  if (_params.NCP_available) {
    if (_loadFieldArray(inputRay, _params.NCP_field_name,
                        true, _ncpArray)) {
      return -1;
    }
  } else {
    for (int igate = 0; igate < _nGates; igate++) {
      _ncpArray[igate] = missingDbl;
    }
  }
  
  if (_params.SNR_available) {
    if (_loadFieldArray(inputRay, _params.SNR_field_name,
                        true, _snrArray)) {
      return -1;
    }
  } else {
    _computeSnrFromDbz();
  }
  

  if (_loadFieldArray(inputRay, _params.ZDR_field_name,
                      true, _zdrArray)) {
    return -1;
  }

  if (_params.LDR_available) {
    if (_loadFieldArray(inputRay, _params.LDR_field_name,
                        true, _ldrArray)) {
      return -1;
    }
  } else {
    for (int igate = 0; igate < _nGates; igate++) {
      _ldrArray[igate] = missingDbl;
    }
  }

  if (_loadFieldArray(inputRay, _params.PHIDP_field_name,
                      true, _phidpArray)) {
    return -1;
  }

  if (_loadFieldArray(inputRay, _params.RHOHV_field_name,
                      true, _rhohvArray)) {
    return -1;
  }

  for (int igate = 0; igate < _nGates; igate++) {
    _kdpArray[igate] = missingDbl;
    _kdpCondArray[igate] = missingDbl;
    _kdpLogArray[igate] = missingDbl;
  }
  
  if (_loadFieldArray(inputRay, _params.dbz_elevation_gradient_field_name,
                      false, _dbzElevGradientArray) == 0) {
    _dbzElevGradientAvail = true;
  } else {
    _dbzElevGradientAvail = false;
  }

  return 0;
  
}

////////////////////////////////////////
// load a field array based on the name

int ComputeEngine::_loadFieldArray(RadxRay *inputRay,
                                   const string &fieldName,
                                   bool required,
                                   double *array)

{
  
  RadxField *field = inputRay->getField(fieldName);
  if (field == NULL) {

    if (!required) {
      for (int igate = 0; igate < _nGates; igate++) {
        array[igate] = missingDbl;
      }
      return -1;
    }

    pthread_mutex_lock(&_debugPrintMutex);
    cerr << "ERROR - ComputeEngine::_getField" << endl;
    cerr << "  Cannot find field in ray: " << fieldName<< endl;
    cerr << "  El, az: "
         << inputRay->getElevationDeg() << ", "
         << inputRay->getAzimuthDeg() << endl;
    cerr << "  N fields in ray: " << inputRay->getNFields() << endl;
    pthread_mutex_unlock(&_debugPrintMutex);
    return -1;
  }
  
  // convert field data to floats

  field->convertToFl64();
  const double *vals = field->getDataFl64();
  double missingVal = field->getMissingFl64();
  for (int igate = 0; igate < _nGates; igate++, vals++) {
    double val = *vals;
    if (val == missingVal) {
      array[igate] = missingDbl;
    } else {
      array[igate] = val;
    }
  }
  
  return 0;
  
}

//////////////////////////////////////////////////////////////
// Compute the SNR field from the DBZ field

void ComputeEngine::_computeSnrFromDbz()

{

  // compute noise at each gate

  TaArray<double> noiseDbz_;
  double *noiseDbz = noiseDbz_.alloc(_nGates);
  double range = _startRangeKm;
  if (range == 0) {
    range = _gateSpacingKm / 10.0;
  }
  for (int igate = 0; igate < _nGates; igate++, range += _gateSpacingKm) {
    noiseDbz[igate] = _params.noise_dbz_at_100km +
      20.0 * (log10(range) - log10(100.0));
  }

  // compute snr from dbz
  
  double *snr = _snrArray;
  const double *dbz = _dbzArray;
  for (int igate = 0; igate < _nGates; igate++, snr++, dbz++) {
    if (*dbz != missingDbl) {
      *snr = *dbz - noiseDbz[igate];
    } else {
      *snr = -20;
    }
  }

}

////////////////////////////////////////////////////////////////////////
// Convert interest map points to vector
//
// Returns 0 on success, -1 on failure

int ComputeEngine::_convertInterestParamsToVector(const string &label,
                                                  const Params::interest_map_point_t *map,
                                                  int nPoints,
                                                  vector<InterestMap::ImPoint> &pts)
  
{
  
  pts.clear();
  
  double prevVal = -1.0e99;
  for (int ii = 0; ii < nPoints; ii++) {
    if (map[ii].value <= prevVal) {
      pthread_mutex_lock(&_debugPrintMutex);
      cerr << "ERROR - ComputeEngine:_convertInterestParamsToVector" << endl;
      cerr << "  Map label: " << label << endl;
      cerr << "  Map values must increase monotonically" << endl;
      pthread_mutex_unlock(&_debugPrintMutex);
      return -1;
    }
    InterestMap::ImPoint pt(map[ii].value, map[ii].interest);
    pts.push_back(pt);
    prevVal = map[ii].value;
  } // ii
  
  return 0;

}

//////////////////////////////////////
// compute HCA

void ComputeEngine::_hcaCompute()
  
{

  // compute trend deviation of dbz

  FilterUtils::computeTrendDevInRange(_dbzArray,
                                      _tdDbzArray,
                                      _nGates,
                                      _params.HCA_SD_DBZ_kernel_len,
                                      missingDbl);
  

  FilterUtils::computeSdevInRange(_dbzArray,
                                  _sdDbzArray,
                                  _nGates,
                                  _params.HCA_SD_DBZ_kernel_len,
                                  missingDbl);
  

  // compute trend deviation of phidp

  FilterUtils::computeTrendDevInRange(_kdp.getPhidpUnfold(),
                                      _tdPhidpArray,
                                      _nGates,
                                      _params.HCA_SD_PHIDP_kernel_len,
                                      missingDbl);
  
  for (int igate = 0; igate < _nGates; igate++) {
    double tdPhidp = _tdPhidpArray[igate];
    double snr = _snrArray[igate];
    if (tdPhidp == missingDbl) {
      if (snr != missingDbl && snr > _params.KDP_snr_threshold) {
        _tdPhidpArray[igate] = 0.0;
      }
    } else if (tdPhidp < 0.01) {
      if (snr == missingDbl || snr < _params.KDP_snr_threshold) {
        _tdPhidpArray[igate] = missingDbl;
      }
    }
  }

  // set up features
  
  double *featureVals[HcaInterestMap::nFeatures];
  featureVals[HcaInterestMap::FeatureDBZ] = _dbzArray;
  featureVals[HcaInterestMap::FeatureZDR] = _zdrArray;
  featureVals[HcaInterestMap::FeatureRHOHV] = _rhohvArray;
  featureVals[HcaInterestMap::FeatureLOG_KDP] = _kdpLogArray;
  featureVals[HcaInterestMap::FeatureSD_DBZ] = _tdDbzArray;
  featureVals[HcaInterestMap::FeatureSD_PHIDP] = _tdPhidpArray;

  // set up classes interest
  
  double *interestVals[HcaInterestMap::nClasses];
  interestVals[HcaInterestMap::ClassGC] = _gcInterest;
  interestVals[HcaInterestMap::ClassBS] = _bsInterest;
  interestVals[HcaInterestMap::ClassDS] = _dsInterest;
  interestVals[HcaInterestMap::ClassWS] = _wsInterest;
  interestVals[HcaInterestMap::ClassCR] = _crInterest;
  interestVals[HcaInterestMap::ClassGR] = _grInterest;
  interestVals[HcaInterestMap::ClassBD] = _bdInterest;
  interestVals[HcaInterestMap::ClassRA] = _raInterest;
  interestVals[HcaInterestMap::ClassHR] = _hrInterest;
  interestVals[HcaInterestMap::ClassRH] = _rhInterest;

  // initialize interest to missing

  for (int igate = 0; igate < _nGates; igate++) {
    for (size_t iclass = 0; iclass < HcaInterestMap::nClasses; iclass++) {
      interestVals[iclass][igate] = missingDbl;
    }
    _hcaArray[igate] = -1;
  }

  // compute the interest for each class at each gate

  for (int igate = 0; igate < _nGates; igate++) {

    // ensure we have all feature fields

    bool missing = false;
    for (size_t ifeature = 0; ifeature < HcaInterestMap::nFeatures; ifeature++) {
      if (featureVals[ifeature][igate] == missingDbl) {
        missing = true;
        break;
      }
    }
    if (missing) {
      continue;
    }

    // sum up interest and weights for each class
    
    for (size_t iclass = 0; iclass < HcaInterestMap::nClasses; iclass++) {
    
      double sumWtInterest = 0.0;
      double sumWt = 0.0;
      for (size_t ifeature = 0; ifeature < HcaInterestMap::nFeatures; ifeature++) {
        _imaps[iclass][ifeature]->accumWeightedInterest(_dbzArray[igate],
                                                        featureVals[ifeature][igate],
                                                        sumWtInterest,
                                                        sumWt);
      } // ifeature

      double meanInterest = sumWtInterest / sumWt;
      interestVals[iclass][igate] = meanInterest;
      
    } // iclass

    // determine the class with the highest weighted interest

    int mostLikelyClass = -1;
    double maxInterest = -9999.0;
    for (size_t iclass = 0; iclass < HcaInterestMap::nClasses; iclass++) {
      if(interestVals[iclass][igate] > maxInterest) {
        mostLikelyClass = iclass;
        maxInterest = interestVals[iclass][igate];
      }
    }
    _hcaArray[igate] = mostLikelyClass + 1;
    
  } // igate

}

/////////////////////////////////////
// initialize interest maps to NULL

void ComputeEngine::_initInterestMaps()
  
{
  
  for (size_t iclass = 0; iclass < HcaInterestMap::nClasses; iclass++) {
    for (size_t ifeature = 0; ifeature < HcaInterestMap::nFeatures; ifeature++) {
      _imaps[iclass][ifeature] = NULL;
    }
  }

}

///////////////////////////////////////////////////////////////////
// Create the interest maps

int ComputeEngine::_createInterestMaps()
{

  int iret = 0;

  // clean up any existing maps
  
  _deleteInterestMaps();

  for (int imap = 0; imap < _params.hca_interest_maps_n; imap++) {
    
    const Params::hca_interest_map_t &pmap = _params._hca_interest_maps[imap];

    if (_imaps[pmap.hca_class][pmap.feature] != NULL) {
      cerr << "ERROR - ComputeEngine::_createInterestMaps()" << endl;
      cerr << "  Duplicate interest map" << endl;
      cerr << "                  class   : "
           << _hcaClassToStr(pmap.hca_class) << endl;
      cerr << "                  feature : " 
           << _hcaFeatureToStr(pmap.feature) << endl;
      iret = -1;
    } else {
      _imaps[pmap.hca_class][pmap.feature] =
        new HcaInterestMap(_getImapClass(pmap.hca_class),
                           _getImapFeature(pmap.feature),
                           pmap.x1, pmap.x2, pmap.x3, pmap.x4,
                           pmap.weight);
    }
  
  } // imap

  return iret;

}

////////////////////////////////////////////
// check that all interest maps are non-NULL

int ComputeEngine::_checkInterestMaps()
  
{
  
  int iret = 0;

  for (size_t iclass = 0; iclass < HcaInterestMap::nClasses; iclass++) {
    for (size_t ifeature = 0; ifeature < HcaInterestMap::nFeatures; ifeature++) {
      if (_imaps[iclass][ifeature] == NULL) {
        cerr << "ERROR - ComputeEngine::_checkInterestMaps()" << endl;
        cerr << "  Missing interest map" << endl;
        cerr << "                  class   : "
             << _hcaClassToStr((Params::hca_class_t) iclass) << endl;
        cerr << "                  feature : " 
             << _hcaFeatureToStr((Params::feature_field_t) ifeature) << endl;
        iret = -1;
      }
    }
  }

  return iret;

}

/////////////////////////////////////
// print all interest maps

void ComputeEngine::_printInterestMaps(ostream &out)
  
{
  
  out << "=============================================" << endl;
  for (size_t iclass = 0; iclass < HcaInterestMap::nClasses; iclass++) {
    for (size_t ifeature = 0; ifeature < HcaInterestMap::nFeatures; ifeature++) {
      if (_imaps[iclass][ifeature] != NULL) {
        if (_imaps[iclass][ifeature] != NULL) {
          _imaps[iclass][ifeature]->print(out);
        }
      }
    }
  }
  out << "=============================================" << endl;

}

/////////////////////////////////////
// delete all interest maps

void ComputeEngine::_deleteInterestMaps()
  
{
  
  for (size_t iclass = 0; iclass < HcaInterestMap::nClasses; iclass++) {
    for (size_t ifeature = 0; ifeature < HcaInterestMap::nFeatures; ifeature++) {
      if (_imaps[iclass][ifeature] != NULL) {
        delete _imaps[iclass][ifeature];
      }
    }
  }

}

//////////////////////////////////////////////////////////////
// convert Params::hca_class_t to HcaInterestMap::imap_class_t

HcaInterestMap::imap_class_t ComputeEngine::_getImapClass(Params::hca_class_t hcaClass)
{
  switch(hcaClass) {
    case Params::CLASS_GC:
      return HcaInterestMap::ClassGC;
    case Params::CLASS_BS:
      return HcaInterestMap::ClassBS;
    case Params::CLASS_DS:
      return HcaInterestMap::ClassDS;
    case Params::CLASS_WS:
      return HcaInterestMap::ClassWS;
    case Params::CLASS_CR:
      return HcaInterestMap::ClassCR;
    case Params::CLASS_GR:
      return HcaInterestMap::ClassGR;
    case Params::CLASS_BD:
      return HcaInterestMap::ClassBD;
    case Params::CLASS_RA:
      return HcaInterestMap::ClassRA;
    case Params::CLASS_HR:
      return HcaInterestMap::ClassHR;
    case Params::CLASS_RH:
      return HcaInterestMap::ClassRH;
  }
  return HcaInterestMap::ClassGC;
}

//////////////////////////////////////////////////////////////
// convert Params::feature_field_t to HcaInterestMap::imap_feature_t

HcaInterestMap::imap_feature_t ComputeEngine::_getImapFeature(Params::feature_field_t feature)
{
  switch(feature) {
    case Params::FEATURE_DBZ:
      return HcaInterestMap::FeatureDBZ;
    case Params::FEATURE_ZDR:
      return HcaInterestMap::FeatureZDR;
    case Params::FEATURE_RHOHV:
      return HcaInterestMap::FeatureRHOHV;
    case Params::FEATURE_LOG_KDP:
      return HcaInterestMap::FeatureLOG_KDP;
    case Params::FEATURE_SD_DBZ:
      return HcaInterestMap::FeatureSD_DBZ;
    case Params::FEATURE_SD_PHIDP:
      return HcaInterestMap::FeatureSD_PHIDP;
  }
  return HcaInterestMap::FeatureDBZ;
}

/////////////////////////////////
// get string for classification

string ComputeEngine::_hcaClassToStr(Params::hca_class_t hcaClass)
{
  switch (hcaClass) {
    case Params::CLASS_GC:
      return "GC";
      break;
    case Params::CLASS_BS:
      return "BS";
      break;
    case Params::CLASS_DS:
      return "DS";
      break;
    case Params::CLASS_WS:
      return "WS";
      break;
    case Params::CLASS_CR:
      return "CR";
      break;
    case Params::CLASS_GR:
      return "GR";
      break;
    case Params::CLASS_BD:
      return "BD";
      break;
    case Params::CLASS_RA:
      return "RA";
      break;
    case Params::CLASS_HR:
      return "HR";
      break;
    case Params::CLASS_RH:
      return "RH";
  }
  return "UNKNOWN";
}

/////////////////////////////////
// get string for featureification

string ComputeEngine::_hcaFeatureToStr(Params::feature_field_t hcaFeature)
{
  switch (hcaFeature) {
    case Params::FEATURE_DBZ:
      return "DBZ";
      break;
    case Params::FEATURE_ZDR:
      return "ZDR";
      break;
    case Params::FEATURE_RHOHV:
      return "RHOHV";
      break;
    case Params::FEATURE_LOG_KDP:
      return "LOG_KDP";
      break;
    case Params::FEATURE_SD_DBZ:
      return "SD_DBZ";
      break;
    case Params::FEATURE_SD_PHIDP:
      return "SD_PHIPD";
      break;
  }
  return "UNKNOWN";
}

