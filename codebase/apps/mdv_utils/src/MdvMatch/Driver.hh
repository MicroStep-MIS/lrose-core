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
//  Driver for MdvMatch application class
//
//  Terri L. Betancourt RAP, NCAR, Boulder, CO, 80307, USA
//  January 2000
//
//  $Id: Driver.hh,v 1.4 2016/03/04 02:22:11 dixon Exp $
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _MDVMATCH_DRIVER_INC_
#define _MDVMATCH_DRIVER_INC_

#include <string>
#include <vector>
#include <toolsa/Path.hh>
#include <toolsa/MsgLog.hh>
#include <Mdv/DsMdvx.hh>
#include <Mdv/DsMdvxInput.hh>

#include "Params.hh"
#include "DataMgr.hh"
using namespace std;

//
// Forward class declarations
//


class Driver
{
public:
   Driver();
  ~Driver();

   //
   // Initialization
   //
   static const string version;
   int                 init( int argc, char **argv );
   const string&       getProgramName(){ return program.getFile(); }

   //
   // Messaging
   //
   MsgLog&             getMsgLog(){ return msgLog; }

   //
   // Execution
   //
   int                 run();

private:

   //
   // Command line processing
   //
   Path               program;
   void               usage();
   int                processArgs( int argc, char **argv );

   time_t             startTime;
   time_t             endTime;

   //
   // Parameter processing
   //
   Params             params;
   char              *paramPath;
   tdrp_override_t    tdrpOverride;

   int                readParams(  int argc, char **argv );
   int                processParams();

   //
   // Messaging
   //
   MsgLog             msgLog;

   //
   // Data management and execution
   //
   vector< string >   inputFileList;
   DataMgr            dataMgr;
   DsMdvxInput        trigger;
   DsMdvx             mdvxInputA;
   DsMdvx             mdvxInputB;

};

//
// Make one instance global
//
#ifdef _MDVMATCH_MAIN_
          Driver *driver;
#else
   extern Driver *driver;
#endif

// 
// Macros for message logging 
// 
#define POSTMSG          driver->getMsgLog().postMsg
#define DEBUG_ENABLED    driver->getMsgLog().isEnabled( DEBUG )
#define VERBOSE_ENABLED  driver->getMsgLog().isEnabled( INFO )
   
//
// Macro for easy access to application name
//
#define PROGRAM_NAME driver->getProgramName().c_str()

//
// Prototypes for asynchronous handlers
//
void   dieGracefully( int signal );

#endif
