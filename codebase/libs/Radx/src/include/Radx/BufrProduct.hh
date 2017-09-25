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
// BufrProduct.hh
//
// BUFR Product
//
// Brenda Javornik, EOL, NCAR
// P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// Aug 2017
//
///////////////////////////////////////////////////////////////

#ifndef BufrProduct_HH
#define BufrProduct_HH


#include <Radx/RadxBuf.hh>
#include <vector>

///////////////////////////////////////////////////////////////
/// BASE CLASS FOR BUFR DATA ACCESS

class BufrProduct
{
  
public:

  /// Constructor
  
  BufrProduct();
  
  /// Destructor
  
  ~BufrProduct();

  void reset();

  void allocateSpace(unsigned int n);

  void addData(unsigned char value);

  void transitionState(int n);

  double *decompressData();

  void createSweep();

  void putYear(double value);
  void putMonth(double value);
  void putDay(double value);
  void putHour(double value);
  void putMinute(double value);
  void putSecond(double value);

  /*
  setAntennaElevationDegrees(double value);

  setNBinsAlongRadial(double value);

  setRangeBinSizeMeters(double value);

  setRangeBinOffsetMeters(double value);

  setNAzimuths(double value);

  setAntennaBeamAzimuthDegrees(double value);
  */

  typedef enum {rawData, other} ProductType;
  enum DataType {CM, TV, DBZH, VRAD, TH, WRAD,  KDP, PHIDP, RHOHV,
    OTHER};

  typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
  } TimeStamp;

  vector<TimeStamp> timeStampStack;

  RadxBuf compressedData; // dataBuffer;

  unsigned char *dataBuffer; // *compressedData; // [2*65]; // TODO: fix this
  unsigned int nData = 0;
  int nDataSegments;
  unsigned int maxData;
  unsigned int totalData;

  string typeOfProduct;
    TimeStamp startTime;
    TimeStamp endTime;
    double    antennaElevationDegrees;
    int       nBinsAlongTheRadial;
    double    rangeBinSizeMeters;
    double    rangeBinOffsetMeters;
    int       nAzimuths;
    double    antennaBeamAzimuthDegrees;

  typedef struct {
    string typeOfProduct;
    double *data;
  } ParameterData;

  typedef struct {
    TimeStamp startTime;
    TimeStamp endTime;
    double    antennaElevationDegrees;
    int       nBinsAlongTheRadial;
    double    rangeBinSizeMeters;
    double    rangeBinOffsetMeters;
    int       nAzimuths;
    double    antennaBeamAzimuthDegrees;
    vector<ParameterData> parameterData;

  } SweepData;

  vector<SweepData> sweepData;

  // just accumulate data for one sweep and the metadata for it, then
  // dump it into a RadxVol structure

  vector<unsigned int> replicators;
  void trashReplicator();
  void storeReplicator(unsigned int value);


  // current position information
  int currentSweepNumber;  // set at 3;21;203 -> 1;12;0 -> 0;31;1
  int currentParamNumber;
  //int currentDataSection;  ?? just use add?
  //int currentDataValue; ??? just use add?

  int currentState;

  ProductType productType;
  DataType    dataType;
};
#endif

