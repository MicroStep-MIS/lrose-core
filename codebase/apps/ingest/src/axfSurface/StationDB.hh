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
////////////////////////////////////////////////////////////////////////////////
//
// Terri L. Betancourt RAP, NCAR, Boulder, CO, 80307, USA
// August 1999
//
// $Id: StationDB.hh,v 1.4 2016/03/07 01:23:07 dixon Exp $
//
////////////////////////////////////////////////////////////////////////////////

#include <toolsa/membuf.h>
#include <symprod/spdb.h>
using namespace std;


class StationDB  {
 public:
   
   StationDB( char **dest, int nDest, int expSecs );
  ~StationDB();
   
   void   reset( int size );
   int    add( const StationReport &stationReport );
   int    write();
   
 private:

   char**  destinations;
   int     nDestinations;
   int     expireSecs;
   
   spdb_chunk_ref_t *chunkHdrs;
   MEMbuf           *chunkBuf;
   int               chunkIdex;
   int               nChunks;
};
