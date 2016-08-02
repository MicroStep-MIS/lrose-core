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

#include <toolsa/pmu.h>
#include <Mdv/DsMdvxTimes.hh>
#include <iostream>
#include <strings.h>

#include "Params.hh"
#include "UrlWatcher.hh"
#include "Process.hh"

#include <toolsa/file_io.h>

using namespace std;

//
// Constructor
//
UrlWatcher::UrlWatcher(){
  startTime = 0; endTime = 0; // Default to REALTIME mode.
}
//////////////////////////////////////////////  
//
// Destructor
//
UrlWatcher::~UrlWatcher(){
}

//////////////////////////////////////////////////
// Main method - run.
//
int UrlWatcher::init( int argc, char **argv ){

  //
  // Parse command line args. Pretty minimal for this program.
  //
  if (ParseArgs(argc,argv)) return -1;
  //
  // Get TDRP args and check in to PMU.
  //

  
  if (P.loadFromArgs(argc,argv,NULL,NULL)){
    cerr << "Specify params file with -params option." << endl ;
    return(-1);
  }                       
  
  PMU_auto_init("MdvHist", P.Instance,
                PROCMAP_REGISTER_INTERVAL);     

  if ((startTime != 0) || (endTime != 0)){
    P.Mode = Params::ARCHIVE;
    if (P.Debug){
      cerr << "Interval from " << utimstr(startTime) << " to " << utimstr(endTime) << endl;
    }
  }

  //
  // Make the output dir, if desired.
  //
  if (P.fileOut){
    if (ta_makedir_recurse( P.outDir)){
      cerr << "Failed to make directory " << P.outDir << endl;
    }
  }
  //
  // Set up for input.
  // Depends on what mode we're in.
  //

  //
  // Special case - archive mode on forecast data.
  // In this case, we run the whole thing
  // right here.
  //
  if ((P.Mode == Params::ARCHIVE) && (P.forecastMode)){

    Process S(&P);

    if (P.Debug) cerr << "ARCHIVE FORECAST mode." << endl;

    //
    // Get generate times.
    //

    DsMdvx Tlist;
    Tlist.setTimeListModeGen(P.TriggerUrl, startTime, endTime);
    Tlist.compileTimeList();
    vector<time_t> genTimes = Tlist.getTimeList();

    if (P.Debug){
      cerr << genTimes.size() << " generate times found between " << endl;
      cerr << utimstr(startTime) << " and " << utimstr(endTime);
      cerr << " at " << P.TriggerUrl << endl;
    }

    for (unsigned i=0; i < genTimes.size(); i++){

      if (P.Debug)
	cerr << "Gen time " << utimstr(genTimes[i]) << endl;

      time_t genTime = genTimes[i];
      DsMdvx inTim;
      inTim.setTimeListModeForecast( P.TriggerUrl, genTime);
      inTim.compileTimeList();
    
      vector <time_t> vtimes = inTim.getValidTimes();

      for (unsigned j=0; j < vtimes.size(); j++){
	if (P.Debug)
	  cerr << "  Valid time " << utimstr(vtimes[j]) << endl;

	time_t validTime = vtimes[j];
	S.Derive(&P, genTime, validTime-genTime );
      }
    }
    exit(0);
  }


  if (P.Mode == Params::ARCHIVE){

    if (InMdv.setArchive(P.TriggerUrl,
			 startTime,
			 endTime)){
      cerr << "Failed to set URL " << P.TriggerUrl << endl;
      return -1;
    }

  } else { // REALTIME mode


    if (InMdv.setRealtime(P.TriggerUrl,
			  P.MaxRealtimeValidAge,
			  PMU_auto_register)){
      cerr << "Failed to set URL " << P.TriggerUrl << endl;
      return -1;
    }



  } // End of if we are in realtime.

  //
  // Input Mdv object should now be all set. Use it in the run function.
  //
  return 0;

}

int UrlWatcher::run(){

  Process S(&P);

  do{

    time_t Time;
    
    InMdv.getNext( Time );
    
    if ((Time == (time_t)NULL) && (P.Mode == Params::ARCHIVE)){
      //
      // If we are outputting files, output the cumulative total file.
      //
      if (P.fileOut && P.outputTotalFile) {
	S.outputTotalCounts(&P, startTime, endTime);
      }
      return 0; // Reached end of the list in archive mode.
    }

    if (Time != (time_t)NULL){
      S.Derive(&P, Time);
    }

  } while (1);
  
  return 0;

}
///////////////////////////////////////////////
// Parse command line args.
//
int UrlWatcher::ParseArgs(int argc,char *argv[]){

  for (int i=1; i<argc; i++){
 
    if ( (!strcmp(argv[i], "-h")) ||
         (!strcmp(argv[i], "--")) ||
         (!strcmp(argv[i], "-?")) ) {                
      cout << "USAGE : MdvHist [-print_params to get parameters]" << endl;
      cout << "[-h or -- or -? to get this help message]" << endl;
      cout << "[-interval YYYYMMDDhhmmss YYYYMMDDhhmmss -params pfile to run in archive mode]" << endl;
      cout << "[-yesterday to run on yesterday's data, UTC]" << endl;
      exit(0);

    }
    
    if (!strcmp(argv[i], "-yesterday")){

      date_time_t A;
      A.unix_time = time(NULL) - 86400;
      uconvert_from_utime( &A );
      A.sec = 0; A.min = 0; A.hour = 0;
      uconvert_to_utime( &A );

      startTime = A.unix_time;
      endTime = startTime + 86399;

      // cerr << "Running from " << utimstr(startTime) << " to " << utimstr(endTime) << endl;

    }

    if (!strcmp(argv[i], "-interval")){
      i++;
      if (i == argc) {
	cerr << "Must specify start and end times with -interval" << endl;
	exit(-1);
      }
      
      date_time_t T;
      if (6!=sscanf(argv[i],"%4d%2d%2d%2d%2d%2d",
		    &T.year, &T.month, &T.day,
		    &T.hour, &T.min, &T.sec)){
	cerr << "Format for start time is YYYYMMDDhhmmss" << endl;
	return -1;
      }
      uconvert_to_utime( &T );
      startTime = T.unix_time;
    
      i++;
      if (i == argc) {
	cerr << "Must specify end time with -interval" << endl;
	return -1;
      }

      if (6!=sscanf(argv[i],"%4d%2d%2d%2d%2d%2d",
		    &T.year, &T.month, &T.day,
		    &T.hour, &T.min, &T.sec)){
	cerr << "Format for end time is YYYYMMDDhhmmss" << endl;
	exit(-1);
      }
      uconvert_to_utime( &T );
      endTime = T.unix_time;


    }


  }

  return 0; // All systems go.
  
}

     






