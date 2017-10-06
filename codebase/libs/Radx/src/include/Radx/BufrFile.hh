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
// BufrFile.hh
//
// BUFR file wrapper
//
// Brenda Javornik, EOL, NCAR
// P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// Aug 2017
//
///////////////////////////////////////////////////////////////

#ifndef BufrFile_HH
#define BufrFile_HH

#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include <cstdio>
#include <Radx/Radx.hh>
#include <Radx/RadxTime.hh>
#include <Radx/TableMapElement.hh>
#include <Radx/TableMap.hh>
#include <Radx/RadxBuf.hh>
#include <Radx/BufrProduct.hh>

using namespace std;

///////////////////////////////////////////////////////////////
/// CLASS FOR BUFR IO OPERATIONS

class BufrFile

{
  
public:

  /// Constructor
  
  BufrFile();


  
  /// Destructor
  
  virtual ~BufrFile();

  //////////////////////////////////////////////////////////////
  /// \name File operations
  //@{

  /// read the data for the field
  /// 
  void readThatField(string fileName,
             string filePath,
             time_t fileTime,
             string fieldName,
             string standardName,
             string longName,
             string units,
             bool debug,
             bool verbose);

  /// open for reading
  /// Returns 0 on success, -1 on failure
  
  int openRead(const string &path);

  /// open netcdf file for writing
  /// create error object so we can handle errors
  /// set the netcdf format, before a write
  /// format options are:
  ///   Classic - classic format (i.e. version 1 format)
  ///   Offset64Bits - 64-bit offset format
  ///   Netcdf4 - netCDF-4 using HDF5 format
  ///   Netcdf4Classic - netCDF-4 using HDF5 but only netCDF-3 calls
  /// Returns 0 on success, -1 on failure
  
  // int openWrite(const string &path, Nc3File::FileFormat format);

  int readSection0();
  int readSection1();
  int readDataDescriptors();
  int readDescriptorTables();
  int readData();

  int Print();

  /// close previously-opened file

  void close();

  double latitude;
  double longitude;
  double height;
  int hdr_year;
  int hdr_month;
  int hdr_day;
  int hour;
  int minute;

  int WMOBlockNumber;
  int WMOStationNumber;
  string typeOfStationId;
  string stationId;

  size_t getNumberOfSweeps();
  size_t getTimeDimension();
  size_t getRangeDimension();

  size_t getNBinsAlongTheRadial();
  double getRangeBinOffsetMeters();
  double getRangeBinSizeMeters();

  double getElevationForSweep(int sweepNumber);
  int getNAzimuthsForSweep(int sweepNumber);
  double getStartingAzimuthForSweep(int sweepNumber);

  double getStartTimeForSweep(int sweepNumber);
  double getEndTimeForSweep(int sweepNumber);

  double *getDataForSweep(int sweepNumber);
  string getTypeOfProductForSweep(int sweepNumber);

private:

  bool StuffIt(string fieldName, double value);
  Radx::ui32 ExtractIt(int nBits);
  string ExtractText(int nBits);
  void SkipIt(int nBits);
  double fastPow10(int n);
  Radx::ui32 Apply(TableMapElement f);
  Radx::si32 ApplyNumeric(TableMapElement f);
  Radx::fl32 ApplyNumericFloat(TableMapElement f);
  //  int TraverseOriginal(vector<unsigned short> descriptors);
  int TraverseNew(vector<unsigned short> descriptors);
  //int Traverse(int start, int length); //vector<unsigned short> descriptors);
  int ReplenishBuffer();
  bool NextBit();

  BufrProduct currentProduct;
  //vector<int> repeaters;


  //@}

  // TODO: see how offsets are handled in Dorade???
  typedef enum {
  
    YEAR = 12,
    MONTH = 14,
    DAY = 15,
    HOUR = 16,
    MINUTE = 17,
    SECOND = 18
  } Bufr_4_offset;

  typedef enum {
    TABLE_B = 0,
    REPLICATOR = 1,
    TABLE_C = 2,
    TABLE_D = 3
  } Type_of_descriptor;

  
  typedef struct {
    //Type_of_descriptor f;
    Radx::ui08 f;
    Radx::ui08 x;
    Radx::ui08 y;
  } DescriptorKey;
  
  typedef struct {
    Radx::ui08 edition;
    Radx::ui32 nBytes;
  } Section0;

  typedef struct {
    bool hasSection2;
    Radx::ui08 masterTable;
    Radx::ui16 generatingCenter;
    Radx::ui16 originatingSubcenter;
    Radx::ui08 updateSequenceNumber;
    Radx::ui08 dataCategoryType;
    Radx::ui08 dataCategorySubtype;
    Radx::ui08 masterTableVersionNumber;
    Radx::ui08 localTableVersionNumber;
    Radx::ui16 year;
    Radx::ui08 month;
    Radx::ui08 day;
    Radx::ui08 hour;
    Radx::ui08 minute;
    Radx::ui08 seconds;
  } Section1;


  Section0 _s0;
  Section1 _s1; 
  Radx::ui32 _numBytesRead;

  typedef struct node {
    unsigned short des;
    struct node *children;
    //Radx::ui32 nRepeats;
    unsigned short delayed_repeater;
    struct node *next;
  } DNode;

  DNode *GTree;
  int _descend(DNode *tree);
  DNode* buildTree(vector<unsigned short> descriptors, bool reverse);
  void _deleteAfter(DNode *p);
  int moveChildren(DNode *parent, int howManySiblings);
  void printTree(DNode *tree, int level);
  void freeTree(DNode *tree);

#define  MAX_BUFFER_SIZE_BYTES  2048

  //  static const int MAX_BUFFER_SIZE_BYTES = 2048; // 4096; //  2048; // 5; //1024; // 30;
  unsigned char _dataBuffer[MAX_BUFFER_SIZE_BYTES];
  int currentBufferLengthBits;
  int currentBufferLengthBytes;
  int currentBufferIndexBits;
  int nOctetsRead;
  //int currentBufferIndexBytes;

  std::vector<unsigned short> _descriptorsToProcess;

  TableMap tableMap;

  bool _debug;
  bool _verbose;
  
/*
  //////////////////////////////////////////////////////////////
  /// \name Attributes
  //@{
   

  //@}

  //////////////////////////////////////////////////////////////
  /// \name dimensions
  //@{
  
  //@}

  //////////////////////////////////////////////////////////////
  /// \name variables
  //@{
  
  //@}

  ///////////////////////////////
  /// \name Strings from nc items
  //@{
  
  */
  ////////////////////////
  /// \name Handles:
  //@{
  
  /// Get the path in use after read or write
  
  string getPathInUse() const { return _pathInUse; }
  
  
  //@}

  ////////////////////////
  /// \name Error string:
  //@{
  
  /// Clear error string.
  
  void clearErrStr() { _errString.clear(); }

  /// Get the Error String.
  ///
  /// The contents are only meaningful if an error has returned.
  
  string getErrStr() const { return _errString; }
  
  //@}
  
protected:

private:

  void clear();

  // error string

  string _errString; ///< Error string is set on read or write error
  
  // handles
  
  FILE *_file;
  string _pathInUse;
  bool _firstBufferReplenish;

  // Error strings accumulate information and then 
  // thrown as exceptions.
  
  /// add integer value to error string, with label
  
  void _addErrInt(string label, int iarg,
                  bool cr = true);
  
  /// add double value to error string, with label
  
  void _addErrDbl(string label, double darg,
                  string format, bool cr = true);
  
  /// add string value to error string, with label
  
  void _addErrStr(string label, string strarg = "",
                  bool cr = true);

};
#endif
