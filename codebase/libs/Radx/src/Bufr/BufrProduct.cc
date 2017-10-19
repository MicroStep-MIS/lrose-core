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
// BufrProduct.cc
//
// BUFR Product 
//
// Brenda Javornik, EOL, NCAR
// P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// Aug 2017
//
///////////////////////////////////////////////////////////////

#include <Radx/BufrProduct.hh>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <zlib.h>
#include <stdlib.h>

using namespace std;

//////////////
// Constructor

BufrProduct::BufrProduct()
{
  //replicators.reserve(5);
  dataBuffer = NULL;
  reset();
  _debug = false;
  _verbose = false;
}

/////////////
// destructor

BufrProduct::~BufrProduct()
{
  if (dataBuffer != NULL)
    free(dataBuffer);
}

void BufrProduct::reset() {
  currentState = 0;  // initial state
  nData = 0;
  maxData = 0;
  _maxBinsAlongTheRadial = 0;
  //totalData = 0;
  if (dataBuffer != NULL)
    free(dataBuffer);
  dataBuffer = NULL;
  sweepData.clear(); // assume that the Rays are copied
  // reset the replicators vector
  while (!replicators.empty()) 
    trashReplicator();
}

void BufrProduct::allocateSpace(unsigned int n) {
  if (dataBuffer == NULL) {
    dataBuffer = (unsigned char *) malloc(n);
    maxData = n;
  }
}

//size_t BufrProduct::getMaxBinsAlongTheRadial() {
//  return _maxBinsAlongTheRadial;
//}

void BufrProduct::setNBinsAlongTheRadial(size_t nBins) {
  if (sweepData.size() > 0) {
    if ((nBins != sweepData[0].nBinsAlongTheRadial) && _debug) {
      cerr << "Changing the number of bins along the radial from " <<
	sweepData[0].nBinsAlongTheRadial << " to " << nBins << endl;
    // don't resize here, just request everything by sweep number
    }
  }
  nBinsAlongTheRadial = nBins;
  if (nBins > _maxBinsAlongTheRadial)
    _maxBinsAlongTheRadial = nBins;
}

size_t BufrProduct::getNBinsAlongTheRadial(int sweepNumber) {
  return sweepData[sweepNumber].nBinsAlongTheRadial;
}

void BufrProduct::setAntennaElevationDegrees(double value) {
  // if (sweepData.size() > 0) {
  //  if (value!= sweepData[0].antennaElevationDegrees)
  //    throw "Cannot change the antenna elevation.";
  // }
  antennaElevationDegrees = value;
}
double BufrProduct::getAntennaElevationDegrees(int sweepNumber) {
  return sweepData[sweepNumber].antennaElevationDegrees;
}

void BufrProduct::setRangeBinSizeMeters(double value) {
  if (sweepData.size() > 0) {
    if ((value != sweepData[0].rangeBinSizeMeters) && _debug) {
      cerr << "Changing the range bin size from  " <<
	sweepData[0].rangeBinSizeMeters << " to " << value << endl;
    }
  }
  rangeBinSizeMeters = value;
}
double BufrProduct::getRangeBinSizeMeters(int sweepNumber) {
  return sweepData[sweepNumber].rangeBinSizeMeters;
}

void BufrProduct::setRangeBinOffsetMeters(double value) {
  if (sweepData.size() > 0) {
    if (value != sweepData[0].rangeBinOffsetMeters)
      throw "Cannot change the range bin offset.";
  }
  rangeBinOffsetMeters = value;
}
double BufrProduct::getRangeBinOffsetMeters() {
  return rangeBinOffsetMeters;
}

void BufrProduct::setNAzimuths(size_t value) {
  if (sweepData.size() > 0) {
    if (value != sweepData[0].nAzimuths)
      throw "Cannot change the number of azimuths.";
  }
  nAzimuths = value;
}
size_t BufrProduct::getNAzimuths() {
  return nAzimuths;
}

void BufrProduct::setAntennaBeamAzimuthDegrees(double value) {
  //if (sweepData.size() > 0) {
  //  if (value != sweepData[0].antennaBeamAzimuthDegrees)
  //    throw "Cannot change the antenna beam azimuth.";
  //}
  antennaBeamAzimuthDegrees = value;
}
double BufrProduct::getAntennaBeamAzimuthDegrees(int sweepNumber) {
  return sweepData[sweepNumber].antennaBeamAzimuthDegrees;
}

// OK, we are assuming a particular order to the replicators.
// The order is:
// - header repeats
// - number of sweeps
// - number of parameters
// - number of data sections
// - number of byte elements in data section 
void BufrProduct::storeReplicator(unsigned int value) {
  replicators.push_back(value);
  // once we have the number of byte elements, allocate space for the
  // ensuing data values.
  switch (replicators.size()) {
  case 1:  // we know the number of sweeps
    break;
  case 2:  
    // reset dataBuffer
    nData = 0;
    break;
  case 3:
    break;
  case 4:
    allocateSpace(value); 
    break;
  default:
    throw "Unexpected number of entries in Replicator store";
  }
}

void BufrProduct::trashReplicator() {

  switch (replicators.size()) {
  case 1:
    break;
  case 2:  // we know the number of sweeps
    break;
  case 3:
    break;
  case 4:  
    // we are done with a data segment;  
    // combine data segments as needed
    if (dataBuffer != NULL) {
      //combine data segments
      unsigned int size;
      size = nData; 
      compressedData.add(dataBuffer, size);
      nData = 0;
      free(dataBuffer);
      dataBuffer = NULL;
    }
    break;
  default:
    throw "Unexpected number of replicators in store";
  }
  replicators.pop_back();
}

// Record all the information now that we have all the data values
void BufrProduct::createSweep() {
  //double *realData;
  float *realData;
      
      realData = decompressDataFl32();
      if (realData == NULL) {
	throw "ERROR - could not decompress data";
      }
      SweepData newSweep;
      newSweep.endTime = timeStampStack.back();
      timeStampStack.pop_back();
      newSweep.startTime = timeStampStack.back();
      timeStampStack.pop_back();

      newSweep.antennaElevationDegrees = antennaElevationDegrees;
      newSweep.nBinsAlongTheRadial = nBinsAlongTheRadial;
      newSweep.rangeBinSizeMeters = rangeBinSizeMeters;
      newSweep.rangeBinOffsetMeters = rangeBinOffsetMeters;
      newSweep.nAzimuths = nAzimuths;
      newSweep.antennaBeamAzimuthDegrees = antennaBeamAzimuthDegrees;
      ParameterData parameterData;
      parameterData.typeOfProduct = typeOfProduct;
      parameterData.data = realData;
      newSweep.parameterData.push_back(parameterData);
      sweepData.push_back(newSweep);
}

void BufrProduct::addData(unsigned char value) {
  //if (value == 255)
  //  printf("%u at nData = %d\n", value, nData);
  if (nData < maxData)
    dataBuffer[nData++] = value;
  else 
    throw "out of space in dataBuffer";
}

double *BufrProduct::decompressData() {
  int n;
  n = nBinsAlongTheRadial * nAzimuths * sizeof(double);

  //int i, j;
  //unsigned char str[sizeof(double)];
  unsigned char *UnCompDataBuff = (unsigned char *) malloc(n);
  unsigned long DestBuffSize = n;
  
  if (uncompress(UnCompDataBuff, &DestBuffSize, 
		 (unsigned char *) compressedData.getPtr(), 
		 compressedData.getLen()) != Z_OK) {
		 //                 nData) != Z_OK) {
    return NULL;
  }
#if __BYTE_ORDER == __BIG_ENDIAN
  for (i = 0; i < *ndecomp/sizeof(double); ++i) {
    for (j = 0; j < sizeof(double); ++j) {
      str[j] = UnCompDataBuff[i*sizeof(double)+j];
    }
    for (j = 0; j < sizeof(double); ++j) {
      UnCompDataBuff[i*sizeof(double)+j] = str[sizeof(double) - 1 - j];
    }
  }
#endif
  compressedData.clear();
  double *temp;
  temp = (double *)UnCompDataBuff;
  if (_debug) { 
    printf ("--> %g %g %g\n", temp[0], temp[1], temp[2]);
    //printf (" ... %g %g %g <--\n", temp[n-3], temp[n-2], temp[n-1]);
  }
  return (double *)UnCompDataBuff;
}

float *BufrProduct::decompressDataFl32() {
  unsigned long n;
  n = nBinsAlongTheRadial * nAzimuths * sizeof(double);

  //int i, j;
  //unsigned char str[sizeof(double)];
  unsigned char *UnCompDataBuff = (unsigned char *) malloc(n);
  unsigned long DestBuffSize = n;
  
  int result;
  result = uncompress(UnCompDataBuff, &DestBuffSize, 
		 (unsigned char *) compressedData.getPtr(), 
		      compressedData.getLen()); 

  if (result != Z_OK) {
    
    switch(result) {
    case Z_BUF_ERROR:	 	
      throw "The buffer dest was not large enough to hold the uncompressed data.";
      break;

    case Z_MEM_ERROR:	 	
      throw "Insufficient memory.";
      break;

    case Z_DATA_ERROR:	 	
      throw "The compressed data (referenced by source) was corrupted.";
      break;
    default:
      return NULL;
    }
  }

#if __BYTE_ORDER == __BIG_ENDIAN
  for (i = 0; i < *ndecomp/sizeof(double); ++i) {
    for (j = 0; j < sizeof(double); ++j) {
      str[j] = UnCompDataBuff[i*sizeof(double)+j];
    }
    for (j = 0; j < sizeof(double); ++j) {
      UnCompDataBuff[i*sizeof(double)+j] = str[sizeof(double) - 1 - j];
    }
  }
#endif
  compressedData.clear();
  double *temp;
  temp = (double *)UnCompDataBuff;
  if (_debug) {
    printf ("--> %g %g %g\n", temp[0], temp[1], temp[2]);
    //printf (" ... %g %g %g <--\n", temp[n-3], temp[n-2], temp[n-1]);
  }

  // convert the data to float
  unsigned long n32;
  n32 = nBinsAlongTheRadial * nAzimuths * sizeof(float);
  unsigned char *UnCompDataBuff32 = (unsigned char *) malloc(n32);
  float *temp32;
  temp32 = (float *) UnCompDataBuff32;
  for (unsigned long i=0; i< nBinsAlongTheRadial * nAzimuths; i++) {
    temp32[i] = (float) temp[i];
  }

  if (_debug) {
    printf("after conversion to float ...\n");
    printf ("--> %g %g %g\n", temp32[0], temp32[1], temp32[2]);
    //unsigned long nFloats;
    //nFloats = nBinsAlongTheRadial * nAzimuths;
    //printf (" ... %g %g %g <--\n", temp32[200], 
    //	    temp32[201], temp32[202]);
  }

  free(UnCompDataBuff);
  return temp32; //  (float *)UnCompDataBuff32;
}

// maintain the timestamps in a stack
// start a new timestamp with Year,
// always update the top timestamp with day, hour, etc.
void BufrProduct::putYear(double value) {
  TimeStamp timeStamp;
  timeStamp.year = (int) value;
  timeStampStack.push_back(timeStamp);
}

void BufrProduct::putMonth(double value) {
  timeStampStack.back().month = (int) value;
}

void BufrProduct::putDay(double value)  {
  timeStampStack.back().day = (int) value;
}
void BufrProduct::putHour(double value)  {
  timeStampStack.back().hour = (int) value;
}
void BufrProduct::putMinute(double value)  {
  timeStampStack.back().minute = (int) value;
}
void BufrProduct::putSecond(double value)  {
  timeStampStack.back().second = (int) value;
}

/*
BufrProduct::setAntennaElevationDegrees(double value) {
  MetaData sweep = metaData.at(currentSweepNumber);
  sweep.antennaElevationDegrees = value;
}

BufrProduct::setNBinsAlongRadial(double value) {
  MetaData sweep = metaData.at(currentSweepNumber);
  sweep.setNBinsAlongRadial = value;
}

BufrProduct::setRangeBinSizeMeters(double value) {
  MetaData sweep = metaData.at(currentSweepNumber);
  sweep.setRangeBinSizeMeters = value;
}

BufrProduct::setRangeBinOffsetMeters(double value) {
  MetaData sweep = metaData.at(currentSweepNumber);
  sweep.setRangeBinOffsetMeters = value;
}

BufrProduct::setNAzimuths(double value) {
  MetaData sweep = metaData.at(currentSweepNumber);
  sweep.setNAzimuths = value;
}

BufrProduct::setAntennaBeamAzimuthDegrees(double value) {
  MetaData sweep = metaData.at(currentSweepNumber);
  sweep.antennaBeamAzimuthDegrees = value;
}
*/
