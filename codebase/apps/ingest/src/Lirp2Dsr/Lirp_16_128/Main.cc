///////////////////////////////////////////////////////////////
//
// main for Lirp2Dsr
//
// Mike Dixon, RAP, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// May 2003
//
///////////////////////////////////////////////////////////////
//
// Lirp2Dsr reads raw Sigmet IQ time-series data, computes the
// moments and writes the contents into a DsRadar FMQ.
//
////////////////////////////////////////////////////////////////

#include "Lirp2Dsr.hh"
#include <toolsa/str.h>
#include <toolsa/port.h>
#include <signal.h>
#include <new>
using namespace std;

// file scope

static void tidy_and_exit (int sig);
static void out_of_store();
static Lirp2Dsr *_prog;
static int _argc;
static char **_argv;

// main

int main(int argc, char **argv)

{

  _argc = argc;
  _argv = argv;

  // create program object

  _prog = new Lirp2Dsr(argc, argv);
  if (!_prog->isOK) {
    return(-1);
  }

  // set signal handling
  
  PORTsignal(SIGINT, tidy_and_exit);
  PORTsignal(SIGHUP, tidy_and_exit);
  PORTsignal(SIGTERM, tidy_and_exit);
  PORTsignal(SIGPIPE, (PORTsigfunc)SIG_IGN);

  // set new() memory failure handler function

  set_new_handler(out_of_store);

  // run it

  int iret = _prog->Run();

  // clean up

  tidy_and_exit(iret);
  return (iret);
  
}

///////////////////
// tidy up on exit

static void tidy_and_exit (int sig)

{

  delete(_prog);
  exit(sig);

}
////////////////////////////////////
// out_of_store()
//
// Handle out-of-memory conditions
//

static void out_of_store()

{

  fprintf(stderr, "FATAL ERROR - program Lirp2Dsr\n");
  fprintf(stderr, "  Operator new failed - out of store\n");
  exit(-1);

}
