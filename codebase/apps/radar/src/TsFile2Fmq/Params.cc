////////////////////////////////////////////
// Params.cc
//
// TDRP C++ code file for class 'Params'.
//
// Code for program TsFile2Fmq
//
// This file has been automatically
// generated by TDRP, do not modify.
//
/////////////////////////////////////////////

/**
 *
 * @file Params.cc
 *
 * @class Params
 *
 * This class is automatically generated by the Table
 * Driven Runtime Parameters (TDRP) system
 *
 * @note Source is automatically generated from
 *       paramdef file at compile time, do not modify
 *       since modifications will be overwritten.
 *
 *
 * @author Automatically generated
 *
 */
using namespace std;

#include "Params.hh"
#include <cstring>

  ////////////////////////////////////////////
  // Default constructor
  //

  Params::Params()

  {

    // zero out table

    memset(_table, 0, sizeof(_table));

    // zero out members

    memset(&_start_, 0, &_end_ - &_start_);

    // class name

    _className = "Params";

    // initialize table

    _init();

    // set members

    tdrpTable2User(_table, &_start_);

    _exitDeferred = false;

  }

  ////////////////////////////////////////////
  // Copy constructor
  //

  Params::Params(const Params& source)

  {

    // sync the source object

    source.sync();

    // zero out table

    memset(_table, 0, sizeof(_table));

    // zero out members

    memset(&_start_, 0, &_end_ - &_start_);

    // class name

    _className = "Params";

    // copy table

    tdrpCopyTable((TDRPtable *) source._table, _table);

    // set members

    tdrpTable2User(_table, &_start_);

    _exitDeferred = false;

  }

  ////////////////////////////////////////////
  // Destructor
  //

  Params::~Params()

  {

    // free up

    freeAll();

  }

  ////////////////////////////////////////////
  // Assignment
  //

  void Params::operator=(const Params& other)

  {

    // sync the other object

    other.sync();

    // free up any existing memory

    freeAll();

    // zero out table

    memset(_table, 0, sizeof(_table));

    // zero out members

    memset(&_start_, 0, &_end_ - &_start_);

    // copy table

    tdrpCopyTable((TDRPtable *) other._table, _table);

    // set members

    tdrpTable2User(_table, &_start_);

    _exitDeferred = other._exitDeferred;

  }

  ////////////////////////////////////////////
  // loadFromArgs()
  //
  // Loads up TDRP using the command line args.
  //
  // Check usage() for command line actions associated with
  // this function.
  //
  //   argc, argv: command line args
  //
  //   char **override_list: A null-terminated list of overrides
  //     to the parameter file.
  //     An override string has exactly the format of an entry
  //     in the parameter file itself.
  //
  //   char **params_path_p:
  //     If this is non-NULL, it is set to point to the path
  //     of the params file used.
  //
  //   bool defer_exit: normally, if the command args contain a 
  //      print or check request, this function will call exit().
  //      If defer_exit is set, such an exit is deferred and the
  //      private member _exitDeferred is set.
  //      Use exidDeferred() to test this flag.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int Params::loadFromArgs(int argc, char **argv,
                           char **override_list,
                           char **params_path_p,
                           bool defer_exit)
  {
    int exit_deferred;
    if (_tdrpLoadFromArgs(argc, argv,
                          _table, &_start_,
                          override_list, params_path_p,
                          _className,
                          defer_exit, &exit_deferred)) {
      return (-1);
    } else {
      if (exit_deferred) {
        _exitDeferred = true;
      }
      return (0);
    }
  }

  ////////////////////////////////////////////
  // loadApplyArgs()
  //
  // Loads up TDRP using the params path passed in, and applies
  // the command line args for printing and checking.
  //
  // Check usage() for command line actions associated with
  // this function.
  //
  //   const char *param_file_path: the parameter file to be read in
  //
  //   argc, argv: command line args
  //
  //   char **override_list: A null-terminated list of overrides
  //     to the parameter file.
  //     An override string has exactly the format of an entry
  //     in the parameter file itself.
  //
  //   bool defer_exit: normally, if the command args contain a 
  //      print or check request, this function will call exit().
  //      If defer_exit is set, such an exit is deferred and the
  //      private member _exitDeferred is set.
  //      Use exidDeferred() to test this flag.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int Params::loadApplyArgs(const char *params_path,
                            int argc, char **argv,
                            char **override_list,
                            bool defer_exit)
  {
    int exit_deferred;
    if (tdrpLoadApplyArgs(params_path, argc, argv,
                          _table, &_start_,
                          override_list,
                          _className,
                          defer_exit, &exit_deferred)) {
      return (-1);
    } else {
      if (exit_deferred) {
        _exitDeferred = true;
      }
      return (0);
    }
  }

  ////////////////////////////////////////////
  // isArgValid()
  // 
  // Check if a command line arg is a valid TDRP arg.
  //

  bool Params::isArgValid(const char *arg)
  {
    return (tdrpIsArgValid(arg));
  }

  ////////////////////////////////////////////
  // load()
  //
  // Loads up TDRP for a given class.
  //
  // This version of load gives the programmer the option to load
  // up more than one class for a single application. It is a
  // lower-level routine than loadFromArgs, and hence more
  // flexible, but the programmer must do more work.
  //
  //   const char *param_file_path: the parameter file to be read in.
  //
  //   char **override_list: A null-terminated list of overrides
  //     to the parameter file.
  //     An override string has exactly the format of an entry
  //     in the parameter file itself.
  //
  //   expand_env: flag to control environment variable
  //               expansion during tokenization.
  //               If TRUE, environment expansion is set on.
  //               If FALSE, environment expansion is set off.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int Params::load(const char *param_file_path,
                   char **override_list,
                   int expand_env, int debug)
  {
    if (tdrpLoad(param_file_path,
                 _table, &_start_,
                 override_list,
                 expand_env, debug)) {
      return (-1);
    } else {
      return (0);
    }
  }

  ////////////////////////////////////////////
  // loadFromBuf()
  //
  // Loads up TDRP for a given class.
  //
  // This version of load gives the programmer the option to
  // load up more than one module for a single application,
  // using buffers which have been read from a specified source.
  //
  //   const char *param_source_str: a string which describes the
  //     source of the parameter information. It is used for
  //     error reporting only.
  //
  //   char **override_list: A null-terminated list of overrides
  //     to the parameter file.
  //     An override string has exactly the format of an entry
  //     in the parameter file itself.
  //
  //   const char *inbuf: the input buffer
  //
  //   int inlen: length of the input buffer
  //
  //   int start_line_num: the line number in the source which
  //     corresponds to the start of the buffer.
  //
  //   expand_env: flag to control environment variable
  //               expansion during tokenization.
  //               If TRUE, environment expansion is set on.
  //               If FALSE, environment expansion is set off.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int Params::loadFromBuf(const char *param_source_str,
                          char **override_list,
                          const char *inbuf, int inlen,
                          int start_line_num,
                          int expand_env, int debug)
  {
    if (tdrpLoadFromBuf(param_source_str,
                        _table, &_start_,
                        override_list,
                        inbuf, inlen, start_line_num,
                        expand_env, debug)) {
      return (-1);
    } else {
      return (0);
    }
  }

  ////////////////////////////////////////////
  // loadDefaults()
  //
  // Loads up default params for a given class.
  //
  // See load() for more detailed info.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int Params::loadDefaults(int expand_env)
  {
    if (tdrpLoad(NULL,
                 _table, &_start_,
                 NULL, expand_env, FALSE)) {
      return (-1);
    } else {
      return (0);
    }
  }

  ////////////////////////////////////////////
  // sync()
  //
  // Syncs the user struct data back into the parameter table,
  // in preparation for printing.
  //
  // This function alters the table in a consistent manner.
  // Therefore it can be regarded as const.
  //

  void Params::sync(void) const
  {
    tdrpUser2Table(_table, (char *) &_start_);
  }

  ////////////////////////////////////////////
  // print()
  // 
  // Print params file
  //
  // The modes supported are:
  //
  //   PRINT_SHORT:   main comments only, no help or descriptions
  //                  structs and arrays on a single line
  //   PRINT_NORM:    short + descriptions and help
  //   PRINT_LONG:    norm  + arrays and structs expanded
  //   PRINT_VERBOSE: long  + private params included
  //

  void Params::print(FILE *out, tdrp_print_mode_t mode)
  {
    tdrpPrint(out, _table, _className, mode);
  }

  ////////////////////////////////////////////
  // checkAllSet()
  //
  // Return TRUE if all set, FALSE if not.
  //
  // If out is non-NULL, prints out warning messages for those
  // parameters which are not set.
  //

  int Params::checkAllSet(FILE *out)
  {
    return (tdrpCheckAllSet(out, _table, &_start_));
  }

  //////////////////////////////////////////////////////////////
  // checkIsSet()
  //
  // Return TRUE if parameter is set, FALSE if not.
  //
  //

  int Params::checkIsSet(const char *paramName)
  {
    return (tdrpCheckIsSet(paramName, _table, &_start_));
  }

  ////////////////////////////////////////////
  // freeAll()
  //
  // Frees up all TDRP dynamic memory.
  //

  void Params::freeAll(void)
  {
    tdrpFreeAll(_table, &_start_);
  }

  ////////////////////////////////////////////
  // usage()
  //
  // Prints out usage message for TDRP args as passed
  // in to loadFromArgs().
  //

  void Params::usage(ostream &out)
  {
    out << "TDRP args: [options as below]\n"
        << "   [ -params path ] specify params file path\n"
        << "   [ -check_params] check which params are not set\n"
        << "   [ -print_params [mode]] print parameters\n"
        << "     using following modes, default mode is 'norm'\n"
        << "       short:   main comments only, no help or descr\n"
        << "                structs and arrays on a single line\n"
        << "       norm:    short + descriptions and help\n"
        << "       long:    norm  + arrays and structs expanded\n"
        << "       verbose: long  + private params included\n"
        << "       short_expand:   short with env vars expanded\n"
        << "       norm_expand:    norm with env vars expanded\n"
        << "       long_expand:    long with env vars expanded\n"
        << "       verbose_expand: verbose with env vars expanded\n"
        << "   [ -tdrp_debug] debugging prints for tdrp\n"
        << "   [ -tdrp_usage] print this usage\n";
  }

  ////////////////////////////////////////////
  // arrayRealloc()
  //
  // Realloc 1D array.
  //
  // If size is increased, the values from the last array 
  // entry is copied into the new space.
  //
  // Returns 0 on success, -1 on error.
  //

  int Params::arrayRealloc(const char *param_name, int new_array_n)
  {
    if (tdrpArrayRealloc(_table, &_start_,
                         param_name, new_array_n)) {
      return (-1);
    } else {
      return (0);
    }
  }

  ////////////////////////////////////////////
  // array2DRealloc()
  //
  // Realloc 2D array.
  //
  // If size is increased, the values from the last array 
  // entry is copied into the new space.
  //
  // Returns 0 on success, -1 on error.
  //

  int Params::array2DRealloc(const char *param_name,
                             int new_array_n1,
                             int new_array_n2)
  {
    if (tdrpArray2DRealloc(_table, &_start_, param_name,
                           new_array_n1, new_array_n2)) {
      return (-1);
    } else {
      return (0);
    }
  }

  ////////////////////////////////////////////
  // _init()
  //
  // Class table initialization function.
  //
  //

  void Params::_init()

  {

    TDRPtable *tt = _table;

    // Parameter 'Comment 0'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = COMMENT_TYPE;
    tt->param_name = tdrpStrDup("Comment 0");
    tt->comment_hdr = tdrpStrDup("TsFile2Fmq reads raw time-series data from a file. It saves the time series data out to a file message queue (FMQ), which can be read by multiple clients. Its purpose is mainly for simulation and debugging time series operations.");
    tt->comment_text = tdrpStrDup("");
    tt++;
    
    // Parameter 'Comment 1'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = COMMENT_TYPE;
    tt->param_name = tdrpStrDup("Comment 1");
    tt->comment_hdr = tdrpStrDup("DEBUGGING AND PROCESS CONTROL.");
    tt->comment_text = tdrpStrDup("");
    tt++;
    
    // Parameter 'debug'
    // ctype is '_debug_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = ENUM_TYPE;
    tt->param_name = tdrpStrDup("debug");
    tt->descr = tdrpStrDup("Debug option");
    tt->help = tdrpStrDup("If set, debug messages will be printed appropriately");
    tt->val_offset = (char *) &debug - &_start_;
    tt->enum_def.name = tdrpStrDup("debug_t");
    tt->enum_def.nfields = 4;
    tt->enum_def.fields = (enum_field_t *)
        tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
      tt->enum_def.fields[0].name = tdrpStrDup("DEBUG_OFF");
      tt->enum_def.fields[0].val = DEBUG_OFF;
      tt->enum_def.fields[1].name = tdrpStrDup("DEBUG_NORM");
      tt->enum_def.fields[1].val = DEBUG_NORM;
      tt->enum_def.fields[2].name = tdrpStrDup("DEBUG_VERBOSE");
      tt->enum_def.fields[2].val = DEBUG_VERBOSE;
      tt->enum_def.fields[3].name = tdrpStrDup("DEBUG_EXTRA");
      tt->enum_def.fields[3].val = DEBUG_EXTRA;
    tt->single_val.e = DEBUG_OFF;
    tt++;
    
    // Parameter 'instance'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("instance");
    tt->descr = tdrpStrDup("Process instance");
    tt->help = tdrpStrDup("Used for registration with procmap.");
    tt->val_offset = (char *) &instance - &_start_;
    tt->single_val.s = tdrpStrDup("test");
    tt++;
    
    // Parameter 'register_with_procmap'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("register_with_procmap");
    tt->descr = tdrpStrDup("Option to register with the process mapper.");
    tt->help = tdrpStrDup("If true, this application will try to register with procmap once per minute. (If unable to do so, no error occurs.)");
    tt->val_offset = (char *) &register_with_procmap - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'Comment 2'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = COMMENT_TYPE;
    tt->param_name = tdrpStrDup("Comment 2");
    tt->comment_hdr = tdrpStrDup("MODE OF OPERATION.");
    tt->comment_text = tdrpStrDup("");
    tt++;
    
    // Parameter 'mode'
    // ctype is '_mode_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = ENUM_TYPE;
    tt->param_name = tdrpStrDup("mode");
    tt->descr = tdrpStrDup("Operating mode");
    tt->help = tdrpStrDup("In REALTIME mode, the program waits for a new input file to process. In ARCHIVE mode, it moves through the list of file names specified on the command line. In SIMULATE mode, the program moves repeatedly through the file list, creating output files with times set to now A delay is inserted between pulses to simulate an operational radar.");
    tt->val_offset = (char *) &mode - &_start_;
    tt->enum_def.name = tdrpStrDup("mode_t");
    tt->enum_def.nfields = 3;
    tt->enum_def.fields = (enum_field_t *)
        tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
      tt->enum_def.fields[0].name = tdrpStrDup("ARCHIVE");
      tt->enum_def.fields[0].val = ARCHIVE;
      tt->enum_def.fields[1].name = tdrpStrDup("REALTIME");
      tt->enum_def.fields[1].val = REALTIME;
      tt->enum_def.fields[2].name = tdrpStrDup("SIMULATE");
      tt->enum_def.fields[2].val = SIMULATE;
    tt->single_val.e = REALTIME;
    tt++;
    
    // Parameter 'Comment 3'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = COMMENT_TYPE;
    tt->param_name = tdrpStrDup("Comment 3");
    tt->comment_hdr = tdrpStrDup("DATA INPUT");
    tt->comment_text = tdrpStrDup("");
    tt++;
    
    // Parameter 'input_dir'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("input_dir");
    tt->descr = tdrpStrDup("Dir for input volume scan files.");
    tt->help = tdrpStrDup("This is used in REALTIME mode only. In ARCHIVE and SIMULATE modes, the file paths are specified on the command line.");
    tt->val_offset = (char *) &input_dir - &_start_;
    tt->single_val.s = tdrpStrDup("$(HOME)/data/time_series/save");
    tt++;
    
    // Parameter 'use_ldata_info_file'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("use_ldata_info_file");
    tt->descr = tdrpStrDup("Option to use _latest_data_info file as input trigger.");
    tt->help = tdrpStrDup("REALTIME mode only. If true, waits on _latest_data_info file. If false, scans the directory for new file.");
    tt->val_offset = (char *) &use_ldata_info_file - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'max_realtime_valid_age'
    // ctype is 'int'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = INT_TYPE;
    tt->param_name = tdrpStrDup("max_realtime_valid_age");
    tt->descr = tdrpStrDup("Max valid age of rdata input files in realtime mode (secs)");
    tt->help = tdrpStrDup("This the max valid age for an incoming file. The program will wait for a data file more recent than this age.");
    tt->val_offset = (char *) &max_realtime_valid_age - &_start_;
    tt->has_min = TRUE;
    tt->min_val.i = 1;
    tt->single_val.i = 360;
    tt++;
    
    // Parameter 'Comment 4'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = COMMENT_TYPE;
    tt->param_name = tdrpStrDup("Comment 4");
    tt->comment_hdr = tdrpStrDup("OUTPUT FMQ DETAILS");
    tt->comment_text = tdrpStrDup("");
    tt++;
    
    // Parameter 'output_fmq_path'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("output_fmq_path");
    tt->descr = tdrpStrDup("Path to output FMQ.");
    tt->help = tdrpStrDup("The FMQ consists of 2 files, ??.buf and ??.stat.");
    tt->val_offset = (char *) &output_fmq_path - &_start_;
    tt->single_val.s = tdrpStrDup("/tmp/fmq/ts");
    tt++;
    
    // Parameter 'output_fmq_size'
    // ctype is 'int'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = INT_TYPE;
    tt->param_name = tdrpStrDup("output_fmq_size");
    tt->descr = tdrpStrDup("Size of output FMQ, in bytes.");
    tt->help = tdrpStrDup("This is the total size of the output FMQ buffer.");
    tt->val_offset = (char *) &output_fmq_size - &_start_;
    tt->single_val.i = 100000000;
    tt++;
    
    // Parameter 'output_fmq_nslots'
    // ctype is 'int'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = INT_TYPE;
    tt->param_name = tdrpStrDup("output_fmq_nslots");
    tt->descr = tdrpStrDup("Number of slots in the output FMQ.");
    tt->help = tdrpStrDup("The number of slots corresponds to the maximum number of messages which may be written to the buffer before overwrites occur. However, overwrites may occur sooner if the size is not set large enough.");
    tt->val_offset = (char *) &output_fmq_nslots - &_start_;
    tt->single_val.i = 1000;
    tt++;
    
    // Parameter 'output_fmq_blocking'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("output_fmq_blocking");
    tt->descr = tdrpStrDup("Option to set up the FMQ as blocking.");
    tt->help = tdrpStrDup("If TRUE, FMQ will be set up FMQ for blocking operation. If the FMQ becomes full, Test2Dsr will then block until there is space for more data. This should only be used if there is a single client reading the FMQ.");
    tt->val_offset = (char *) &output_fmq_blocking - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'write_latest_data_info'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("write_latest_data_info");
    tt->descr = tdrpStrDup("Option to write latest_data_info for the output FMQ.");
    tt->help = tdrpStrDup("If TRUE, latest_data_info will be written for the output FMQ, at the specified interval.");
    tt->val_offset = (char *) &write_latest_data_info - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'latest_data_info_interval'
    // ctype is 'int'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = INT_TYPE;
    tt->param_name = tdrpStrDup("latest_data_info_interval");
    tt->descr = tdrpStrDup("Number of seconds between writing latest_data_info.");
    tt->help = tdrpStrDup("If write_latest_data_info is TRUE, the program will register with the DataMapper when the output FMQ is written to.");
    tt->val_offset = (char *) &latest_data_info_interval - &_start_;
    tt->single_val.i = 5;
    tt++;
    
    // Parameter 'Comment 5'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = COMMENT_TYPE;
    tt->param_name = tdrpStrDup("Comment 5");
    tt->comment_hdr = tdrpStrDup("OUTPUT MESSAGE DETAILS");
    tt->comment_text = tdrpStrDup("The output messages are written to the FMQ. To improve performance, each message contains a number of time series pulses.");
    tt++;
    
    // Parameter 'n_pulses_per_message'
    // ctype is 'int'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = INT_TYPE;
    tt->param_name = tdrpStrDup("n_pulses_per_message");
    tt->descr = tdrpStrDup("Number of pulses per output message.");
    tt->help = tdrpStrDup("The actual number will sometimes be 1 less than this, because an info data part may will be written once in a while.");
    tt->val_offset = (char *) &n_pulses_per_message - &_start_;
    tt->single_val.i = 200;
    tt++;
    
    // Parameter 'n_pulses_per_info'
    // ctype is 'int'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = INT_TYPE;
    tt->param_name = tdrpStrDup("n_pulses_per_info");
    tt->descr = tdrpStrDup("How often to write an info part.");
    tt->help = tdrpStrDup("The info part includes data which does not change on a pulse-to-pulse basis.");
    tt->val_offset = (char *) &n_pulses_per_info - &_start_;
    tt->single_val.i = 10000;
    tt++;
    
    // Parameter 'data_mapper_report_interval'
    // ctype is 'int'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = INT_TYPE;
    tt->param_name = tdrpStrDup("data_mapper_report_interval");
    tt->descr = tdrpStrDup("Number of seconds between reports to DataMapper.");
    tt->help = tdrpStrDup("If > 0, the program will register with the DataMapper when the output FMQ is written to. If <= 0, registration will not be performed.");
    tt->val_offset = (char *) &data_mapper_report_interval - &_start_;
    tt->single_val.i = 5;
    tt++;
    
    // Parameter 'Comment 6'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = COMMENT_TYPE;
    tt->param_name = tdrpStrDup("Comment 6");
    tt->comment_hdr = tdrpStrDup("ADJUSTING THE TIME");
    tt->comment_text = tdrpStrDup("");
    tt++;
    
    // Parameter 'set_time_to_now'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("set_time_to_now");
    tt->descr = tdrpStrDup("Option to set the pulse time to the current time.");
    tt->help = tdrpStrDup("This can be used for two purposes: (a) if the drx time is incorrect, and you wish to override the time, or (b) for simulating an operational radar, or for correcting.");
    tt->val_offset = (char *) &set_time_to_now - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'set_time_offset'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("set_time_offset");
    tt->descr = tdrpStrDup("Option to change the time by a selected offset.");
    tt->help = tdrpStrDup("");
    tt->val_offset = (char *) &set_time_offset - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'time_offset_secs'
    // ctype is 'int'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = INT_TYPE;
    tt->param_name = tdrpStrDup("time_offset_secs");
    tt->descr = tdrpStrDup("Time offset to be applied (secs)");
    tt->help = tdrpStrDup("");
    tt->val_offset = (char *) &time_offset_secs - &_start_;
    tt->single_val.i = 0;
    tt++;
    
    // Parameter 'Comment 7'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = COMMENT_TYPE;
    tt->param_name = tdrpStrDup("Comment 7");
    tt->comment_hdr = tdrpStrDup("CONVERTING THE IQ DATA ENCODING TYPE");
    tt->comment_text = tdrpStrDup("");
    tt++;
    
    // Parameter 'convert_iq_encoding'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("convert_iq_encoding");
    tt->descr = tdrpStrDup("Option to convert the IQ encoding type.");
    tt->help = tdrpStrDup("The IQ data will be converted to the requested encoding type before being written to the FMQ.");
    tt->val_offset = (char *) &convert_iq_encoding - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'iq_encoding'
    // ctype is '_iq_encoding_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = ENUM_TYPE;
    tt->param_name = tdrpStrDup("iq_encoding");
    tt->descr = tdrpStrDup("Encoding type for outgoing IQ data.");
    tt->help = tdrpStrDup("Only applies if convert_iq_encoding is true.");
    tt->val_offset = (char *) &iq_encoding - &_start_;
    tt->enum_def.name = tdrpStrDup("iq_encoding_t");
    tt->enum_def.nfields = 4;
    tt->enum_def.fields = (enum_field_t *)
        tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
      tt->enum_def.fields[0].name = tdrpStrDup("IQ_ENCODING_FL32");
      tt->enum_def.fields[0].val = IQ_ENCODING_FL32;
      tt->enum_def.fields[1].name = tdrpStrDup("IQ_ENCODING_SCALED_SI16");
      tt->enum_def.fields[1].val = IQ_ENCODING_SCALED_SI16;
      tt->enum_def.fields[2].name = tdrpStrDup("IWRF_IQ_ENCODING_DBM_PHASE_SI16");
      tt->enum_def.fields[2].val = IWRF_IQ_ENCODING_DBM_PHASE_SI16;
      tt->enum_def.fields[3].name = tdrpStrDup("IWRF_IQ_ENCODING_SIGMET_FL16");
      tt->enum_def.fields[3].val = IWRF_IQ_ENCODING_SIGMET_FL16;
    tt->single_val.e = IQ_ENCODING_FL32;
    tt++;
    
    // Parameter 'Comment 8'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = COMMENT_TYPE;
    tt->param_name = tdrpStrDup("Comment 8");
    tt->comment_hdr = tdrpStrDup("SIMULATION MODE");
    tt->comment_text = tdrpStrDup("");
    tt++;
    
    // Parameter 'sim_mode_prf'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("sim_mode_prf");
    tt->descr = tdrpStrDup("PRF for sim mode (pulses/s)");
    tt->help = tdrpStrDup("The app will attempt to deliver pulses at this rate.");
    tt->val_offset = (char *) &sim_mode_prf - &_start_;
    tt->single_val.d = 1000;
    tt++;
    
    // Parameter 'sim_override_radar_params'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("sim_override_radar_params");
    tt->descr = tdrpStrDup("Option to override radar params in simulate mode.");
    tt->help = tdrpStrDup("If TRUE, the following sim parameters will be used to override the radar params in the time series. If FALSE, the original params will be retained.");
    tt->val_offset = (char *) &sim_override_radar_params - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'sim_radar_name'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("sim_radar_name");
    tt->descr = tdrpStrDup("Radar name for simulation mode");
    tt->help = tdrpStrDup("");
    tt->val_offset = (char *) &sim_radar_name - &_start_;
    tt->single_val.s = tdrpStrDup("SPOL");
    tt++;
    
    // Parameter 'sim_site_name'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("sim_site_name");
    tt->descr = tdrpStrDup("Site name for simulation mode");
    tt->help = tdrpStrDup("");
    tt->val_offset = (char *) &sim_site_name - &_start_;
    tt->single_val.s = tdrpStrDup("Marshall");
    tt++;
    
    // Parameter 'sim_latitude_deg'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("sim_latitude_deg");
    tt->descr = tdrpStrDup("Radar latitude (deg).");
    tt->help = tdrpStrDup("See override_radar_location.");
    tt->val_offset = (char *) &sim_latitude_deg - &_start_;
    tt->single_val.d = 39.9321;
    tt++;
    
    // Parameter 'sim_longitude_deg'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("sim_longitude_deg");
    tt->descr = tdrpStrDup("Radar longitude (deg).");
    tt->help = tdrpStrDup("See override_radar_location.");
    tt->val_offset = (char *) &sim_longitude_deg - &_start_;
    tt->single_val.d = -105.182;
    tt++;
    
    // Parameter 'sim_altitude_meters'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("sim_altitude_meters");
    tt->descr = tdrpStrDup("Radar altitude (meters).");
    tt->help = tdrpStrDup("See override_radar_location.");
    tt->val_offset = (char *) &sim_altitude_meters - &_start_;
    tt->single_val.d = 1742;
    tt++;
    
    // Parameter 'sim_beam_width_h'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("sim_beam_width_h");
    tt->descr = tdrpStrDup("Horizontal beam width (deg) for simulation mode.");
    tt->help = tdrpStrDup("");
    tt->val_offset = (char *) &sim_beam_width_h - &_start_;
    tt->single_val.d = 0.92;
    tt++;
    
    // Parameter 'sim_beam_width_v'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("sim_beam_width_v");
    tt->descr = tdrpStrDup("Vertical beam width (deg) for simulation mode.");
    tt->help = tdrpStrDup("");
    tt->val_offset = (char *) &sim_beam_width_v - &_start_;
    tt->single_val.d = 0.92;
    tt++;
    
    // Parameter 'sim_staggered_prt'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("sim_staggered_prt");
    tt->descr = tdrpStrDup("Option to simulate staggered PRT operations.");
    tt->help = tdrpStrDup("See 'staggered_mode'. Selected pulses will be omitted to simulate this mode.");
    tt->val_offset = (char *) &sim_staggered_prt - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'stagger_mode'
    // ctype is '_stagger_mode_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = ENUM_TYPE;
    tt->param_name = tdrpStrDup("stagger_mode");
    tt->descr = tdrpStrDup("In this mode, we leave out selected pulses and change the data in the remaining pulses, as required, to simulate staggered PRT mode.");
    tt->help = tdrpStrDup("");
    tt->val_offset = (char *) &stagger_mode - &_start_;
    tt->enum_def.name = tdrpStrDup("stagger_mode_t");
    tt->enum_def.nfields = 3;
    tt->enum_def.fields = (enum_field_t *)
        tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
      tt->enum_def.fields[0].name = tdrpStrDup("STAGGER_2_3");
      tt->enum_def.fields[0].val = STAGGER_2_3;
      tt->enum_def.fields[1].name = tdrpStrDup("STAGGER_3_4");
      tt->enum_def.fields[1].val = STAGGER_3_4;
      tt->enum_def.fields[2].name = tdrpStrDup("STAGGER_4_5");
      tt->enum_def.fields[2].val = STAGGER_4_5;
    tt->single_val.e = STAGGER_2_3;
    tt++;
    
    // Parameter 'sim_select_pulse_interval'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("sim_select_pulse_interval");
    tt->descr = tdrpStrDup("Option to simulate a lower PRT by discarding pulses and only keeping pulses at a selected interval.");
    tt->help = tdrpStrDup("");
    tt->val_offset = (char *) &sim_select_pulse_interval - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'sim_pulse_interval'
    // ctype is 'int'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = INT_TYPE;
    tt->param_name = tdrpStrDup("sim_pulse_interval");
    tt->descr = tdrpStrDup("Interval at which you wish to keep the pulses.");
    tt->help = tdrpStrDup("see sim_select_pulse_interval");
    tt->val_offset = (char *) &sim_pulse_interval - &_start_;
    tt->single_val.i = 2;
    tt++;
    
    // Parameter 'sim_add_dual_pol_channel'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("sim_add_dual_pol_channel");
    tt->descr = tdrpStrDup("Option to add a second channel for dual-polarization simulations.");
    tt->help = tdrpStrDup("");
    tt->val_offset = (char *) &sim_add_dual_pol_channel - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'sim_fixed_zdr'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("sim_fixed_zdr");
    tt->descr = tdrpStrDup("Constant ZDR value for dual-pol simulation (dB).");
    tt->help = tdrpStrDup("The power in the second channel will be lower that the first by this amount.");
    tt->val_offset = (char *) &sim_fixed_zdr - &_start_;
    tt->single_val.d = 3;
    tt++;
    
    // Parameter 'sim_fixed_phidp'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("sim_fixed_phidp");
    tt->descr = tdrpStrDup("Constant PHIDP value for dual-pol simulation (deg).");
    tt->help = tdrpStrDup("The phase in the second channel will be computed relative to the first channel to yield this phidp value.");
    tt->val_offset = (char *) &sim_fixed_phidp - &_start_;
    tt->single_val.d = 45;
    tt++;
    
    // Parameter 'override_xmit_rcv_mode'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("override_xmit_rcv_mode");
    tt->descr = tdrpStrDup("Option to override the transmit/receive mode in the pulse info header.");
    tt->help = tdrpStrDup("");
    tt->val_offset = (char *) &override_xmit_rcv_mode - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'xmit_rcv_mode'
    // ctype is '_xmit_rcv_mode_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = ENUM_TYPE;
    tt->param_name = tdrpStrDup("xmit_rcv_mode");
    tt->descr = tdrpStrDup("Simulated transmit-receive mode.");
    tt->help = tdrpStrDup("This will be used to modify the ops info.");
    tt->val_offset = (char *) &xmit_rcv_mode - &_start_;
    tt->enum_def.name = tdrpStrDup("xmit_rcv_mode_t");
    tt->enum_def.nfields = 8;
    tt->enum_def.fields = (enum_field_t *)
        tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
      tt->enum_def.fields[0].name = tdrpStrDup("SINGLE_POL");
      tt->enum_def.fields[0].val = SINGLE_POL;
      tt->enum_def.fields[1].name = tdrpStrDup("DP_ALT_HV_CO_ONLY");
      tt->enum_def.fields[1].val = DP_ALT_HV_CO_ONLY;
      tt->enum_def.fields[2].name = tdrpStrDup("DP_ALT_HV_CO_CROSS");
      tt->enum_def.fields[2].val = DP_ALT_HV_CO_CROSS;
      tt->enum_def.fields[3].name = tdrpStrDup("DP_ALT_HV_FIXED_HV");
      tt->enum_def.fields[3].val = DP_ALT_HV_FIXED_HV;
      tt->enum_def.fields[4].name = tdrpStrDup("DP_SIM_HV_FIXED_HV");
      tt->enum_def.fields[4].val = DP_SIM_HV_FIXED_HV;
      tt->enum_def.fields[5].name = tdrpStrDup("DP_SIM_HV_SWITCHED_HV");
      tt->enum_def.fields[5].val = DP_SIM_HV_SWITCHED_HV;
      tt->enum_def.fields[6].name = tdrpStrDup("DP_H_ONLY_FIXED_HV");
      tt->enum_def.fields[6].val = DP_H_ONLY_FIXED_HV;
      tt->enum_def.fields[7].name = tdrpStrDup("DP_V_ONLY_FIXED_HV");
      tt->enum_def.fields[7].val = DP_V_ONLY_FIXED_HV;
    tt->single_val.e = SINGLE_POL;
    tt++;
    
    // trailing entry has param_name set to NULL
    
    tt->param_name = NULL;
    
    return;
  
  }