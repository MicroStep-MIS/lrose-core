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
// Input.cc
//
// Base and derived classes for dealing with different inputs.
//
// Mike Dixon, RAP, NCAR, P.O.Box 3000, Boulder, CO, 80307-3000, USA
//
// May 2000
//
///////////////////////////////////////////////////////////////

#include <cerrno>
#include <toolsa/pmu.h>
#include <toolsa/str.h>
#include <toolsa/uusleep.h>
#include <toolsa/Path.hh>
#include <toolsa/file_io.h>
#include <didss/DataFileNames.hh>
#include <didss/DsInputPath.hh>
#include "Input.hh"
#include "AcSim.hh"
using namespace std;

////////////////////////////////
// Input - abstract base class

// constructor
  
Input::Input(const string &prog_name, const Params &params) :
  _progName(prog_name),
  _params(params)

{

}

// destructor
  
Input::~Input() 
{
  
}

// get the reference time, if available
// this is a time which should be close to the data time,
// to allow the date to be deduced if not available
// from the line-by-line data
// For most cases, this is 'now'

time_t Input::getRefTime()
{
  return time(NULL);
}
  
////////////////////////////////////////////////////////////////  
// SerialInput constructor
  
SerialInput::SerialInput(const string &prog_name, const Params &params) :
  Input(prog_name, params),
  _tty(prog_name)

{

  // open tty

  if (_params.debug) {
    _tty.setDebug();
  }
  
  speed_t speed;
  switch (_params.baud_rate) {
  case Params::BAUD_300:
    speed = B300;
    break;
  case Params::BAUD_1200:
    speed = B1200;
    break;
  case Params::BAUD_2400:
    speed = B2400;
    break;
  case Params::BAUD_4800:
    speed = B4800;
    break;
  case Params::BAUD_9600:
    speed = B9600;
    break;
  case Params::BAUD_19200:
    speed = B19200;
    break;
  case Params::BAUD_38400:
    speed = B38400;
    break;
  default:
    speed = B9600;
  }
  
  if (_tty.openPort(_params.input_device,
		    speed,
		    _params.two_stop_bits,
		    _params.enable_parity,
		    _params.odd_parity,
		    _params.data_is_7_bit)) {
    cerr << "ERROR - " << _progName << " - SerialInput::SerialInput" << endl;
    cerr << "  Cannot open serial port: " << _params.input_device << endl;
  }
  
}

////////////////////////////////////////////////////////////////  
// SerialInput destructor
  
SerialInput::~SerialInput()

{
  _tty.closePort();
}

////////////////////////////////////////////////////////
// SerialInput::readLine()
//
// read the next line from the Tty
//
// returns 0 on success, -1 on failure (input exhausted)

int SerialInput::readLine(char *line, int len)

{

  while (true) {
    
    while (_tty.readSelect(1000) != 1) {
      PMU_auto_register("Waiting for tty data");
    }
    
    PMU_auto_register("Reading tty data");

    if (_tty.readLine(line, len)) {
      cerr << "ERROR - " << _progName << " - SerialInput::readLine" << endl;
      cerr << "  Reading serial data." << endl;
      return -1;
    } else {
      return 0;
    }

  } // while

  return -1;

}

////////////////////////////////////////////////////////////////  
// SimInput constructor

SimInput::SimInput(const string &prog_name, const Params &params) :
  Input(prog_name, params)
  
{

  _acNum = 0;

  for (int i = 0; i < _params.sim_aircraft_n; i++) {
    Params::sim_aircraft_t *ac = &_params._sim_aircraft[i];
    AcSim *acSim = new AcSim(ac->callsign,
			     ac->start_lat, ac->start_lon,
			     ac->altitude, ac->speed,
			     ac->right_burn,
			     ac->left_burn,
			     ac->ejectable_interval,
			     ac->burn_in_place_interval,
			     ac->n_bip_at_a_time,
			     _params.comma_delimited_optional_fields_n,
			     _params._comma_delimited_optional_fields);
    _acs.push_back(acSim);
  }
  
}

////////////////////////////////////////////////////////////////  
// SimInput destructor
  
SimInput::~SimInput()

{

  for (size_t i = 0; i < _acs.size(); i++) {
    delete _acs[i];
  }
  _acs.erase(_acs.begin(), _acs.end());

}

////////////////////////////////////////////////////////
// SimInput::readLine()
//
// read the next line from the Test
//
// returns 0 on success, -1 on failure (input exhausted)

int SimInput::readLine(char *line, int len)

{

  AcSim *ac = _acs[_acNum];
  _acNum++;
  if (_acNum >= (int) _acs.size()) {
    _acNum = 0;
  }
  string posStr(ac->getNextPos());
  STRncopy(line, posStr.c_str(), len);

  return 0;

}

////////////////////////////////////////////////////////////////  
// TcpInput constructor
  
TcpInput::TcpInput(const string &prog_name, const Params &params,
                   const char *host, int port) :
        Input(prog_name, params),
        _host(host),
        _port(port)

{ 
  _sockIsOpen = false;
}

// destructor
  
TcpInput::~TcpInput() 
{
  _closeSocket();
}

////////////////////////////////////////////////////////
// read the next line from socket
// returns 0 on success, -1 on failure (input exhausted)

int TcpInput::readLine(char *line, int len)

{

  while (!_sockIsOpen) {
    if (_openSocket()) {
      PMU_auto_register("Waiting for TCP connection");
    }
    umsleep(5000);
  }

  int pos = 0;
  while (true) {
    
    // check for new data

    bool timedOut;
    if (_readSelect(1000, timedOut)) {
      return -1;
    }
    if (timedOut) {
      PMU_auto_register("Waiting for tcp data");
      umsleep(10);
      continue;
    }
    
    PMU_auto_register("Reading tcp data");

    // read next char
    
    char cc;
    if (_readChar(cc) == 0) {
      
      // add to line
      
      line[pos] = cc;
      pos++;

      // check for length

      if (pos == (len-1)) {
        line[pos] = '\0';
        return 0;
      }

      // check for line feed

      if (cc == '\n') {
        line[pos] = '\0';
        return 0;
      }
      
    } // if (_readChar ...

  } // while
    
    
  return -1;

}

////////////////////////////////////////////////////////
// Open socket
// returns 0 on success, -1 on failure

int TcpInput::_openSocket()
  
{
  
  if (_params.debug >= Params::DEBUG_VERBOSE) {
    cerr << "Trying to open for TCP, host, port: "
         << _host << ", " << _port << endl;
  }
  
  _sock.close();
  _sockIsOpen = false;

  if (_sock.open(_host.c_str(), _port, 10000)) {
    
    cerr << "ERROR - TcpInput::open()" << endl;
    cerr << "  Cannot connect" << endl;
    cerr << "  Host: " << _host << endl;
    cerr << "  Port: " << _port << endl;
    cerr << "  " << _sock.getErrStr() << endl;
    
    return -1;
    
  } // if (_sock.open ...
  
  if (_params.debug >= Params::DEBUG_VERBOSE) {
    cerr << "Opened for TCP, host, port: "
         << _host << ", " << _port << endl;
  }
  
  if (_params.send_tcp_handshake) {
    
    if (_params.debug >= Params::DEBUG_VERBOSE) {
      cerr << "Sending handshake bytes to server" << endl;
      for (int ii = 0; ii < _params.tcp_handshake_bytes_n; ii++) {
        int decimal = _params._tcp_handshake_bytes[ii];
        cerr << "Byte: " << decimal << endl;
      }
    } // verbose
    
    for (int ii = 0; ii < _params.tcp_handshake_bytes_n; ii++) {
      int decimal = _params._tcp_handshake_bytes[ii];
      if (decimal >= 0 && decimal <= 255) {
        unsigned char bb = (unsigned char) decimal;
        if (_sock.writeBuffer(&bb, 1, 5000)) {
          cerr << "ERROR writing handshake byte: " << hex << bb << endl;
          cerr << _sock.getErrStr() << endl;
          _sock.close();
          return -1;
        }
      }
    } // ii

  } // if (_params.send_tcp_handshake)
  
  _sockIsOpen = true;
  return 0;

}

////////////////////////////////////////////////////////
// close socket

void TcpInput::_closeSocket()

{
  _sock.close();
}

////////////////////////////////////////////////////////
// readSelect()
// wait for input
// returns 0 on success, -1 on failure.
// Sets timedOut to true if timedOut, returns 0.
// Blocks if wait_msecs == -1

int TcpInput::_readSelect(int wait_msecs, bool &timedOut)
  
{
  timedOut = false;
  int iret = _sock.readSelect(wait_msecs);
  if (iret == 0) {
    return 0;
  } else if (_sock.getErrNum() == Socket::TIMED_OUT) {
    timedOut = true;
    return 0;
  } else {
    return -1;
  }
}

////////////////////////////////////////////////////////
// Read one character
// returns 0 on success, -1 on failure

int TcpInput::_readChar(char &cc)
  
{
  return _sock.readBuffer(&cc, 1, 10);
}


////////////////////////////////////////////////////////////////  
// REALTIME mode constructor
  
FileInput::FileInput(const string &prog_name, const Params &params) :
        Input(prog_name, params)
        
{
  
  _input = new DsInputPath(_progName,
                           _params.debug >= Params::DEBUG_VERBOSE,
                           _params.input_dir,
                           _params.max_realtime_valid_age,
                           PMU_auto_register,
                           _params.latest_data_info_avail,
                           true);
  
  if (_params.strict_dir_scan) {
    _input->setStrictDirScan(true);
  }
  
  if (strlen(_params.file_name_substr)) {
    _input->setSubString(_params.file_name_substr);
  }

  _in = NULL;
  
}

////////////////////////////////////////////////////////////////  
// ARCHIVE mode constructor
  
FileInput::FileInput(const string &prog_name, const Params &params,
                     time_t startTime, time_t endTime) :
        Input(prog_name, params)

{
  
  _input = new DsInputPath(_progName,
                           _params.debug >= Params::DEBUG_VERBOSE,
                           _params.input_dir,
                           startTime, endTime);
  
  _in = NULL;
  
}

////////////////////////////////////////////////////////////////  
// FILELIST MODE constructor
  
FileInput::FileInput(const string &prog_name, const Params &params,
                     const vector<string> &input_file_paths) :
        Input(prog_name, params)
        
{
  
  _input = new DsInputPath(_progName,
                           _params.debug >= Params::DEBUG_VERBOSE,
                           input_file_paths);

  _in = NULL;

}

////////////////////////////////////////////////////////////////  
// FileInput destructor
  
FileInput::~FileInput()

{

  if (_in != NULL) {
    fclose(_in);
    _in = NULL;
  }

}

// get the reference time, if available
// this is a time which should be close to the data time,
// to allow the date to be deduced if not available
// from the line-by-line data
// For most cases, this is 'now'

time_t FileInput::getRefTime()
{

  if (_params.debug) {
    cerr << "Getting time for: " << _currentPath << endl;
  }

  Path path(_currentPath);
  time_t fileTime;
  bool date_only;
  if (DataFileNames::getDataTime(_currentPath, fileTime, date_only) == 0) {
    if (_params.debug) {
      cerr << "fileTime: "
           << fileTime << ", " << DateTime::strm(fileTime) << endl;
    }
    return fileTime;
  }
  
  // failure - use current time

  fileTime = time(NULL);
  if (_params.debug) {
    cerr << "Using current time, fileTime: "
         << fileTime << ", " << DateTime::strm(fileTime) << endl;
  }
    
  return fileTime;
  

}
  
////////////////////////////////////////////////////////
// FileInput::readLine()
//
// read the next line from the File
//
// returns 0 on success, -1 on failure (input exhausted)

int FileInput::readLine(char *line, int len)

{

  if (_in == NULL) {
    if (_openNext()) {
      return -1;
    }
  }

  while (true) {

    if (fgets(line, len, _in) != NULL) {
      return 0;
    }

    if (_openNext()) {
      return -1;
    }

  }

  return -1;

}

////////////////////////////////////////////////////////
// FileInput::_openNext()
//
// Open the next file for reading
//
// returns 0 on success, -1 on failure

int FileInput::_openNext()

{

  if (_in != NULL) {
    fclose(_in);
    _in = NULL;
  }
 
  char *inputPath = _input->next();
  if (inputPath == NULL) {
    if (_params.debug) {
      cerr << _progName << " - FileInput::_openNext" << endl;
      cerr << "  File list exhausted." << endl;
    }
    return -1;
  }

  _currentPath = inputPath;

  if ((_in = ta_fopen_uncompress(inputPath, "r")) == NULL) {
    cerr << "ERROR - " << _progName
	 << " - FileInput::_openNext" << endl;
    cerr << "  Cannot open input file." << endl;
    cerr << "  " << inputPath << ": " << strerror(errno) << endl;
    return -1;
  }

  if (_params.debug) {
    cerr << "Opened file: " << inputPath << endl;
  }

  return 0;

}

