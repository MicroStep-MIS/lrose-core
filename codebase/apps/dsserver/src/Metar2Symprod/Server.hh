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
// Server.hh
//
// FileServerobject
//
// Mike Dixon, RAP, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// Jan 2000
//
///////////////////////////////////////////////////////////////


#ifndef _Server_HH
#define _Server_HH

#include <string>

#include <Spdb/Symprod.hh>
#include <Spdb/DsSymprodServer.hh>
#include <rapformats/WxObs.hh>  
#include <toolsa/MemBuf.hh>

#include "Params.hh"
#include "Filter.hh"

using namespace std;

class Server : public DsSymprodServer
{

public:
    
  // Constructor:                       
  //   o Registers with procmap
  //   o Opens socket on specified port              

  Server(const string &prog_name,
	 Params       *initialParams);

  // destructor

  virtual ~Server();
  
protected:

  // lookup table for spatial decimation

  bool **_locOccupied;

  Filter *_filter;

  // methods invoked from the base class for managing 
  // local override of parameter file

  int loadLocalParams( const string &paramFile, void **params);
  void freeLocalParams( void *params ) { delete (Params*)params; }

  // overload transformData()

  void transformData(const void *serverParams,
		     const string &dir_path,
		     const int prod_id,
		     const string &prod_label,
		     const int n_chunks_in,
		     const Spdb::chunk_ref_t *chunk_refs_in,
		     const Spdb::aux_ref_t *aux_refs_in,
		     const void *chunk_data_in,
		     int &n_chunks_out,
		     MemBuf &refBufOut,
		     MemBuf &auxBufOut,
		     MemBuf &dataBufOut);

  // Convert the given data chunk from the SPDB database to symprod format.

  int convertToSymprod(const void *params,
		       const string &dir_path,
		       const int prod_id,
		       const string &prod_label,
		       const Spdb::chunk_ref_t &chunk_ref,
		       const Spdb::aux_ref_t &aux_ref,
		       const void *spdb_data,
		       const int spdb_len,
		       MemBuf &symprod_buf);

  int _checkForRequiredFields(const Params *serverParams,
                              const WxObs &obs);

  void _addLabels(Params *serverParams,
		  Symprod &prod,
		  const WxObs &obs,
		  int fcat_index);
  
  void _addWindBarb(Params *serverParams,
		    Symprod &prod,
		    const WxObs &obs,
		    int fcat_index);
  
  void _addFlightCat(Params *serverParams,
		     Symprod &prod,
		     const WxObs &obs,
		     int fcat_index);
  
  void _addHiddenText(Params *serverParams,
                      Symprod &prod,
                      const WxObs &obs,
                      int fcat_index);
  
  int _findFlightCatIndex(Params *serverParams,
			  const WxObs &obs);
  
  double _nearest(double target, double delta);



private:

};

#endif
