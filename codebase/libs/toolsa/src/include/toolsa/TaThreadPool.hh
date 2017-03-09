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
// TaThreadPool.h
//
// Mike Dixon, EOL, NCAR
// P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// March 2017
//
///////////////////////////////////////////////////////////////
//
// Class for thread pool.
//
///////////////////////////////////////////////////////////////

#ifndef TaThreadPool_h
#define TaThreadPool_h

#include <string>
#include <deque>
#include <pthread.h>
class TaThread;

using namespace std;

////////////////////////////
// Generic thread base class

class TaThreadPool {

public:
  
  TaThreadPool();
  virtual ~TaThreadPool();
  
  // Add a thread to the main pool
  // this is used to initialize the pool.
  
  void addThreadToMain(TaThread *thread);
  
  // Add a thread to the avail pool
  // This is called after a done thread is handled.

  void addThreadToAvail(TaThread *thread);
  
  // Add a thread to the done pool
  // This is normally called by the thread itself, once it is done.
  
  void addThreadToDone(TaThread *thread);
  
  // Get a thread from the done or avail pool.
  // Preference is given to done threads.
  // If block is true, blocks until a suitable thread is available.
  // If block is false an no thread is available, returns NULL.
  //
  // If isDone is returned true, the thread came from the done pool.
  //   Handle any return information from the done thread, and then
  //   add it into the avail pool, using addThreadToAvail();
  //
  // If isDone is returned false, the thread came from the avail pool.
  //   In this case, set the thread running.
  //   It will add itself to the done pool when done,
  //      using addThreadToDone().
  
  TaThread *getNextThread(bool block, bool &isDone);
  
  // debugging
  
  void setDebug(bool val) { _debug = val; }
  bool getDebug() const { return _debug; }

protected:

private:
  
  bool _debug;

  pthread_mutex_t _mutex;    // general mutex
  pthread_cond_t _availCond; // available threads condition variable

  deque<TaThread *> _mainPool;  // all of the threads
  deque<TaThread *> _availPool; // threads available for use
  deque<TaThread *> _donePool;  // threads done but not yet dealt with
  
  TaThread *_getAvailThread();
  TaThread *_getDoneThread();

};

#endif

