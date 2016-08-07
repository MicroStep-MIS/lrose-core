// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// ** Copyright UCAR (c) 1992 - 2016
// ** University Corporation for Atmospheric Research(UCAR)
// ** National Center for Atmospheric Research(NCAR)
// ** Boulder, Colorado, USA
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
////////////////////////////////////////////
// Params.cc
//
// TDRP C++ code file for class 'Params'.
//
// Code for program Storms2Xml2
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
        << "   [ -params/--params path ] specify params file path\n"
        << "   [ -check_params/--check_params] check which params are not set\n"
        << "   [ -print_params/--print_params [mode]] print parameters\n"
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
    tt->comment_hdr = tdrpStrDup("The idea of Storms2Xml2 is that it will read a Titan\ndatabase and write XML files representing the storms \ntracked by the Titan system.\n\nThis can be done in realtime or archive mode. The program\nwas developed to allow the transfer of Titan data to\nusers outside of NCAR. The program does very little, if any,\ntranslation or thresholding of the data - for instance, there\nis no ability to only print data with an area greater than\na certain area. The outside user is expected to do that.\n\nUnits for speed are Km/Hr, area is square Km, and times\nare GMT.\n\nNiles Oien May 2003.");
    tt->comment_text = tdrpStrDup("");
    tt++;
    
    // Parameter 'debug'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("debug");
    tt->descr = tdrpStrDup("Set to print more messages as data are processed.");
    tt->help = tdrpStrDup("Can usually be FALSE");
    tt->val_offset = (char *) &debug - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'debugTitan'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("debugTitan");
    tt->descr = tdrpStrDup("Set to print titan read requests.");
    tt->help = tdrpStrDup("Can almost always be FALSE, it is rather verbose.");
    tt->val_offset = (char *) &debugTitan - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'Instance'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("Instance");
    tt->descr = tdrpStrDup("Instance to check in to Procmap with");
    tt->help = tdrpStrDup("Set to something unique.");
    tt->val_offset = (char *) &Instance - &_start_;
    tt->single_val.s = tdrpStrDup("primary");
    tt++;
    
    // Parameter 'radar_name'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("radar_name");
    tt->descr = tdrpStrDup("Radar site name.");
    tt->help = tdrpStrDup("Sets radar name for output to XML file.");
    tt->val_offset = (char *) &radar_name - &_start_;
    tt->single_val.s = tdrpStrDup("not-set");
    tt++;
    
    // Parameter 'radar_location'
    // ctype is '_radar_location_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRUCT_TYPE;
    tt->param_name = tdrpStrDup("radar_location");
    tt->descr = tdrpStrDup("Radar location.");
    tt->help = tdrpStrDup("Note the altitude is in kilometers, not meters.");
    tt->val_offset = (char *) &radar_location - &_start_;
    tt->struct_def.name = tdrpStrDup("radar_location_t");
    tt->struct_def.nfields = 3;
    tt->struct_def.fields = (struct_field_t *)
        tdrpMalloc(tt->struct_def.nfields * sizeof(struct_field_t));
      tt->struct_def.fields[0].ftype = tdrpStrDup("double");
      tt->struct_def.fields[0].fname = tdrpStrDup("latitude");
      tt->struct_def.fields[0].ptype = DOUBLE_TYPE;
      tt->struct_def.fields[0].rel_offset = 
        (char *) &radar_location.latitude - (char *) &radar_location;
      tt->struct_def.fields[1].ftype = tdrpStrDup("double");
      tt->struct_def.fields[1].fname = tdrpStrDup("longitude");
      tt->struct_def.fields[1].ptype = DOUBLE_TYPE;
      tt->struct_def.fields[1].rel_offset = 
        (char *) &radar_location.longitude - (char *) &radar_location;
      tt->struct_def.fields[2].ftype = tdrpStrDup("double");
      tt->struct_def.fields[2].fname = tdrpStrDup("altitude");
      tt->struct_def.fields[2].ptype = DOUBLE_TYPE;
      tt->struct_def.fields[2].rel_offset = 
        (char *) &radar_location.altitude - (char *) &radar_location;
    tt->n_struct_vals = 3;
    tt->struct_vals = (tdrpVal_t *)
        tdrpMalloc(tt->n_struct_vals * sizeof(tdrpVal_t));
      tt->struct_vals[0].d = 0;
      tt->struct_vals[1].d = 0;
      tt->struct_vals[2].d = 0;
    tt++;
    
    // Parameter 'titanUrl'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("titanUrl");
    tt->descr = tdrpStrDup("Input titan URL");
    tt->help = tdrpStrDup("Program reads these data as input unless the trigger mode is\nset to TRIGGER_LDATAINFO, in which case inputDir is used.");
    tt->val_offset = (char *) &titanUrl - &_start_;
    tt->single_val.s = tdrpStrDup("titanp:://localhost::titan/storms/[station]");
    tt++;
    
    // Parameter 'titanDir'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("titanDir");
    tt->descr = tdrpStrDup("Input titan directory.");
    tt->help = tdrpStrDup("If the trigger mode is TRIGGER_LDATAINFO, this is where the\ntitan data are read from, otherwise the titanUrl parameter is used.");
    tt->val_offset = (char *) &titanDir - &_start_;
    tt->single_val.s = tdrpStrDup("$DATA_DIR/titan/storms/[station]/");
    tt++;
    
    // Parameter 'outDir'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("outDir");
    tt->descr = tdrpStrDup("Output directory.");
    tt->help = tdrpStrDup("Output file(s) written here. ");
    tt->val_offset = (char *) &outDir - &_start_;
    tt->single_val.s = tdrpStrDup("$DATA_DIR/titan/storms/[station]/xml/");
    tt++;
    
    // Parameter 'timestampFiles'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("timestampFiles");
    tt->descr = tdrpStrDup("Decision to overwrite output file or timestamp them.");
    tt->help = tdrpStrDup("If TRUE, then output files will be time stamped. If FALSE,a single output file will be continually overwritten.");
    tt->val_offset = (char *) &timestampFiles - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'outName'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("outName");
    tt->descr = tdrpStrDup("Output filename.");
    tt->help = tdrpStrDup("If timestampFiles is TRUE, the trigger time will be prepended\nto the filename, otherwise the file will be continually overwritten.");
    tt->val_offset = (char *) &outName - &_start_;
    tt->single_val.s = tdrpStrDup("Titan.xml");
    tt++;
    
    // Parameter 'triggerMode'
    // ctype is '_triggerMode_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = ENUM_TYPE;
    tt->param_name = tdrpStrDup("triggerMode");
    tt->descr = tdrpStrDup("Decide how the program is triggered.");
    tt->help = tdrpStrDup("The program can be triggered in one of three ways.\n\n (1) TRIGGER_ARCHIVE - the program is run in archive mode\n     on existing data. The start and end times are determined\n     on the command line with -start and -end in the usual way.\n     Specifying these argumnets automatically invokes this mode.     The time step is determined by the timeStep parameter.\n\n (2) TRIGGER_INTERVAL - the program wakes up at regular intervals and\n     runs. These intervals are determined by the timeStep parameter.     This enables the program to run in realtime on a machine remote\n     from where the data are.\n\n (3) TRIGGER_LDATAINFO - the program triggers directly from\n     the titan output using the DsInputPath class. The timeStep\n     parameter is ignored. This has the advantage that the program\n     is triggered directly by Titan; but it does mean that it must\n     run on the same machine as the Titan system (since DsInputPath\n     is inherrently a local device at the time of writing).");
    tt->val_offset = (char *) &triggerMode - &_start_;
    tt->enum_def.name = tdrpStrDup("triggerMode_t");
    tt->enum_def.nfields = 3;
    tt->enum_def.fields = (enum_field_t *)
        tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
      tt->enum_def.fields[0].name = tdrpStrDup("TRIGGER_ARCHIVE");
      tt->enum_def.fields[0].val = TRIGGER_ARCHIVE;
      tt->enum_def.fields[1].name = tdrpStrDup("TRIGGER_INTERVAL");
      tt->enum_def.fields[1].val = TRIGGER_INTERVAL;
      tt->enum_def.fields[2].name = tdrpStrDup("TRIGGER_LDATAINFO");
      tt->enum_def.fields[2].val = TRIGGER_LDATAINFO;
    tt->single_val.e = TRIGGER_LDATAINFO;
    tt++;
    
    // Parameter 'timeStep'
    // ctype is 'int'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = INT_TYPE;
    tt->param_name = tdrpStrDup("timeStep");
    tt->descr = tdrpStrDup("Time increment between triggering for TRIGGER_ARCHIVE\nand TRIGGER_INTERVAL triggering modes, seconds.");
    tt->help = tdrpStrDup("Typically set to approximate radar scan time.");
    tt->val_offset = (char *) &timeStep - &_start_;
    tt->single_val.i = 720;
    tt++;
    
    // Parameter 'maxValidAge'
    // ctype is 'int'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = INT_TYPE;
    tt->param_name = tdrpStrDup("maxValidAge");
    tt->descr = tdrpStrDup("Maximum valid age for input files, seconds, if\nthe TRIGGER_LDATAINFO trigger method is invoked.");
    tt->help = tdrpStrDup("Defaults to half an hour.");
    tt->val_offset = (char *) &maxValidAge - &_start_;
    tt->single_val.i = 1800;
    tt++;
    
    // Parameter 'lookBack'
    // ctype is 'long'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = LONG_TYPE;
    tt->param_name = tdrpStrDup("lookBack");
    tt->descr = tdrpStrDup("Time to look back, seconds.");
    tt->help = tdrpStrDup("Program looks back from now by this time to get\ndata interval. Set to be equal to or more than timeStep\nfor complete temporal coverage. Temporal window is from\ntriggerTime - lookBack to triggerTime, inclusive.");
    tt->val_offset = (char *) &lookBack - &_start_;
    tt->single_val.l = 720;
    tt++;
    
    // Parameter 'runScriptWhenDone'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("runScriptWhenDone");
    tt->descr = tdrpStrDup("Option to run a script when a file is written.");
    tt->help = tdrpStrDup("The output file name will be passed in as an argument\nto this script.");
    tt->val_offset = (char *) &runScriptWhenDone - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'scriptToRun'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("scriptToRun");
    tt->descr = tdrpStrDup("Script to run when a file is written.");
    tt->help = tdrpStrDup("Only relevant if runScriptWhenDone is TRUE. The output\nfile name will be passed in as an argument to the script.");
    tt->val_offset = (char *) &scriptToRun - &_start_;
    tt->single_val.s = tdrpStrDup("copyXmlFile");
    tt++;
    
    // trailing entry has param_name set to NULL
    
    tt->param_name = NULL;
    
    return;
  
  }