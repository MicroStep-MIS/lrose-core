// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// ** Copyright UCAR (c) 1992 - 2016
// ** University Corporation for Atmospheric Research(UCAR)
// ** National Center for Atmospheric Research(NCAR)
// ** Boulder, Colorado, USA
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
////////////////////////////////////////////
// RadxAppParams.cc
//
// TDRP C++ code file for class 'RadxAppParams'.
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

#include "RadxAppParams.hh"
#include <cstring>

  ////////////////////////////////////////////
  // Default constructor
  //

  RadxAppParams::RadxAppParams()

  {

    // zero out table

    memset(_table, 0, sizeof(_table));

    // zero out members

    memset(&_start_, 0, &_end_ - &_start_);

    // class name

    _className = "RadxAppParams";

    // initialize table

    _init();

    // set members

    tdrpTable2User(_table, &_start_);

    _exitDeferred = false;

  }

  ////////////////////////////////////////////
  // Copy constructor
  //

  RadxAppParams::RadxAppParams(const RadxAppParams& source)

  {

    // sync the source object

    source.sync();

    // zero out table

    memset(_table, 0, sizeof(_table));

    // zero out members

    memset(&_start_, 0, &_end_ - &_start_);

    // class name

    _className = "RadxAppParams";

    // copy table

    tdrpCopyTable((TDRPtable *) source._table, _table);

    // set members

    tdrpTable2User(_table, &_start_);

    _exitDeferred = false;

  }

  ////////////////////////////////////////////
  // Destructor
  //

  RadxAppParams::~RadxAppParams()

  {

    // free up

    freeAll();

  }

  ////////////////////////////////////////////
  // Assignment
  //

  void RadxAppParams::operator=(const RadxAppParams& other)

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

  int RadxAppParams::loadFromArgs(int argc, char **argv,
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

  int RadxAppParams::loadApplyArgs(const char *params_path,
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

  bool RadxAppParams::isArgValid(const char *arg)
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

  int RadxAppParams::load(const char *param_file_path,
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

  int RadxAppParams::loadFromBuf(const char *param_source_str,
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

  int RadxAppParams::loadDefaults(int expand_env)
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

  void RadxAppParams::sync(void) const
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

  void RadxAppParams::print(FILE *out, tdrp_print_mode_t mode)
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

  int RadxAppParams::checkAllSet(FILE *out)
  {
    return (tdrpCheckAllSet(out, _table, &_start_));
  }

  //////////////////////////////////////////////////////////////
  // checkIsSet()
  //
  // Return TRUE if parameter is set, FALSE if not.
  //
  //

  int RadxAppParams::checkIsSet(const char *paramName)
  {
    return (tdrpCheckIsSet(paramName, _table, &_start_));
  }

  ////////////////////////////////////////////
  // freeAll()
  //
  // Frees up all TDRP dynamic memory.
  //

  void RadxAppParams::freeAll(void)
  {
    tdrpFreeAll(_table, &_start_);
  }

  ////////////////////////////////////////////
  // usage()
  //
  // Prints out usage message for TDRP args as passed
  // in to loadFromArgs().
  //

  void RadxAppParams::usage(ostream &out)
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

  int RadxAppParams::arrayRealloc(const char *param_name, int new_array_n)
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

  int RadxAppParams::array2DRealloc(const char *param_name,
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

  void RadxAppParams::_init()

  {

    TDRPtable *tt = _table;

    // Parameter 'Comment 0'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = COMMENT_TYPE;
    tt->param_name = tdrpStrDup("Comment 0");
    tt->comment_hdr = tdrpStrDup("RadxApp");
    tt->comment_text = tdrpStrDup("Parameters for general algorithm data filtering for Radx format input data");
    tt++;
    
    // Parameter 'instance'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("instance");
    tt->descr = tdrpStrDup("instance");
    tt->help = tdrpStrDup("for PMU");
    tt->val_offset = (char *) &instance - &_start_;
    tt->single_val.s = tdrpStrDup("default");
    tt++;
    
    // Parameter 'debug_mode'
    // ctype is '_debug_e'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = ENUM_TYPE;
    tt->param_name = tdrpStrDup("debug_mode");
    tt->descr = tdrpStrDup("debugging mode");
    tt->help = tdrpStrDup("NONE=as quiet as possible, DEBUG = minimal debug output, DEBUG_VERBOSE=more");
    tt->val_offset = (char *) &debug_mode - &_start_;
    tt->enum_def.name = tdrpStrDup("debug_e");
    tt->enum_def.nfields = 3;
    tt->enum_def.fields = (enum_field_t *)
        tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
      tt->enum_def.fields[0].name = tdrpStrDup("NONE");
      tt->enum_def.fields[0].val = NONE;
      tt->enum_def.fields[1].name = tdrpStrDup("DEBUG");
      tt->enum_def.fields[1].val = DEBUG;
      tt->enum_def.fields[2].name = tdrpStrDup("DEBUG_VERBOSE");
      tt->enum_def.fields[2].val = DEBUG_VERBOSE;
    tt->single_val.e = DEBUG;
    tt++;
    
    // Parameter 'debug_triggering'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("debug_triggering");
    tt->descr = tdrpStrDup("triggering debug");
    tt->help = tdrpStrDup("set to TRUE to debug triggering");
    tt->val_offset = (char *) &debug_triggering - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'mode'
    // ctype is '_mode_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = ENUM_TYPE;
    tt->param_name = tdrpStrDup("mode");
    tt->descr = tdrpStrDup("Operating mode");
    tt->help = tdrpStrDup("In REALTIME mode, the program waits for a new input file.  In ARCHIVE mode, it moves through the data between the start and end times set on the command line. In FILELIST mode, it moves through the list of file names specified on the command line. Paths (in ARCHIVE mode, at least) MUST contain a day-directory above the data file -- ./data_file.ext will not work as a file path, but ./yyyymmdd/data_file.ext will.");
    tt->val_offset = (char *) &mode - &_start_;
    tt->enum_def.name = tdrpStrDup("mode_t");
    tt->enum_def.nfields = 3;
    tt->enum_def.fields = (enum_field_t *)
        tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
      tt->enum_def.fields[0].name = tdrpStrDup("REALTIME");
      tt->enum_def.fields[0].val = REALTIME;
      tt->enum_def.fields[1].name = tdrpStrDup("ARCHIVE");
      tt->enum_def.fields[1].val = ARCHIVE;
      tt->enum_def.fields[2].name = tdrpStrDup("FILELIST");
      tt->enum_def.fields[2].val = FILELIST;
    tt->single_val.e = FILELIST;
    tt++;
    
    // Parameter 'trigger_url'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("trigger_url");
    tt->descr = tdrpStrDup("trigger url");
    tt->help = tdrpStrDup("URL to trigger off");
    tt->val_offset = (char *) &trigger_url - &_start_;
    tt->single_val.s = tdrpStrDup("");
    tt++;
    
    // Parameter 'output_url'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("output_url");
    tt->descr = tdrpStrDup("output for Algorithm results");
    tt->help = tdrpStrDup("");
    tt->val_offset = (char *) &output_url - &_start_;
    tt->single_val.s = tdrpStrDup("");
    tt++;
    
    // Parameter 'Comment 1'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = COMMENT_TYPE;
    tt->param_name = tdrpStrDup("Comment 1");
    tt->comment_hdr = tdrpStrDup("Threading");
    tt->comment_text = tdrpStrDup("Optional threading of beams, depends on app whether params are used or not");
    tt++;
    
    // Parameter 'num_threads'
    // ctype is 'int'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = INT_TYPE;
    tt->param_name = tdrpStrDup("num_threads");
    tt->descr = tdrpStrDup("Number of threads");
    tt->help = tdrpStrDup("set to 1 or 0 for no threading");
    tt->val_offset = (char *) &num_threads - &_start_;
    tt->single_val.i = 0;
    tt++;
    
    // Parameter 'thread_debug'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("thread_debug");
    tt->descr = tdrpStrDup("Thread debugging");
    tt->help = tdrpStrDup("TRUE to see thread related debugging");
    tt->val_offset = (char *) &thread_debug - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'Comment 2'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = COMMENT_TYPE;
    tt->param_name = tdrpStrDup("Comment 2");
    tt->comment_hdr = tdrpStrDup("Input paths");
    tt->comment_text = tdrpStrDup("Inputs can come from one or more location, a primary source, and any number of secondary sources.");
    tt++;
    
    // Parameter 'input'
    // ctype is '_input_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRUCT_TYPE;
    tt->param_name = tdrpStrDup("input");
    tt->descr = tdrpStrDup("inputs");
    tt->help = tdrpStrDup("index = numerical value to refer to this elsewhere\npath = where the data is\nThe first thing on this list is the primary data source. The geometry of non-primary files will be converted to match the primary files. The file_match_time_offset_sec and file_match_time_tolerance_sec are used to search for the best secondary file to merge. The offset is ignored for the primary field. The ray tolerances are used to match rays in the secondary file with those in the primary file.  If is_climo=true, a fixed file is used no matter what the request, specified by path/climo_file");
    tt->array_offset = (char *) &_input - &_start_;
    tt->array_n_offset = (char *) &input_n - &_start_;
    tt->is_array = TRUE;
    tt->array_len_fixed = FALSE;
    tt->array_elem_size = sizeof(input_t);
    tt->array_n = 0;
    tt->struct_def.name = tdrpStrDup("input_t");
    tt->struct_def.nfields = 9;
    tt->struct_def.fields = (struct_field_t *)
        tdrpMalloc(tt->struct_def.nfields * sizeof(struct_field_t));
      tt->struct_def.fields[0].ftype = tdrpStrDup("int");
      tt->struct_def.fields[0].fname = tdrpStrDup("index");
      tt->struct_def.fields[0].ptype = INT_TYPE;
      tt->struct_def.fields[0].rel_offset = 
        (char *) &_input->index - (char *) _input;
      tt->struct_def.fields[1].ftype = tdrpStrDup("string");
      tt->struct_def.fields[1].fname = tdrpStrDup("path");
      tt->struct_def.fields[1].ptype = STRING_TYPE;
      tt->struct_def.fields[1].rel_offset = 
        (char *) &_input->path - (char *) _input;
      tt->struct_def.fields[2].ftype = tdrpStrDup("double");
      tt->struct_def.fields[2].fname = tdrpStrDup("file_match_time_offset_sec");
      tt->struct_def.fields[2].ptype = DOUBLE_TYPE;
      tt->struct_def.fields[2].rel_offset = 
        (char *) &_input->file_match_time_offset_sec - (char *) _input;
      tt->struct_def.fields[3].ftype = tdrpStrDup("double");
      tt->struct_def.fields[3].fname = tdrpStrDup("file_match_time_tolerance_sec");
      tt->struct_def.fields[3].ptype = DOUBLE_TYPE;
      tt->struct_def.fields[3].rel_offset = 
        (char *) &_input->file_match_time_tolerance_sec - (char *) _input;
      tt->struct_def.fields[4].ftype = tdrpStrDup("double");
      tt->struct_def.fields[4].fname = tdrpStrDup("ray_match_elevation_tolerance_deg");
      tt->struct_def.fields[4].ptype = DOUBLE_TYPE;
      tt->struct_def.fields[4].rel_offset = 
        (char *) &_input->ray_match_elevation_tolerance_deg - (char *) _input;
      tt->struct_def.fields[5].ftype = tdrpStrDup("double");
      tt->struct_def.fields[5].fname = tdrpStrDup("ray_match_azimuth_tolerance_deg");
      tt->struct_def.fields[5].ptype = DOUBLE_TYPE;
      tt->struct_def.fields[5].rel_offset = 
        (char *) &_input->ray_match_azimuth_tolerance_deg - (char *) _input;
      tt->struct_def.fields[6].ftype = tdrpStrDup("double");
      tt->struct_def.fields[6].fname = tdrpStrDup("ray_match_time_tolerance_sec");
      tt->struct_def.fields[6].ptype = DOUBLE_TYPE;
      tt->struct_def.fields[6].rel_offset = 
        (char *) &_input->ray_match_time_tolerance_sec - (char *) _input;
      tt->struct_def.fields[7].ftype = tdrpStrDup("boolean");
      tt->struct_def.fields[7].fname = tdrpStrDup("is_climo");
      tt->struct_def.fields[7].ptype = BOOL_TYPE;
      tt->struct_def.fields[7].rel_offset = 
        (char *) &_input->is_climo - (char *) _input;
      tt->struct_def.fields[8].ftype = tdrpStrDup("string");
      tt->struct_def.fields[8].fname = tdrpStrDup("climo_file");
      tt->struct_def.fields[8].ptype = STRING_TYPE;
      tt->struct_def.fields[8].rel_offset = 
        (char *) &_input->climo_file - (char *) _input;
    tt->n_struct_vals = 0;
    tt->struct_vals = (tdrpVal_t *)
        tdrpMalloc(tt->n_struct_vals * sizeof(tdrpVal_t));
    tt++;
    
    // Parameter 'Comment 3'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = COMMENT_TYPE;
    tt->param_name = tdrpStrDup("Comment 3");
    tt->comment_hdr = tdrpStrDup("Mapping from input field to path");
    tt->comment_text = tdrpStrDup("For each field that is an input to the app, it is associated with an input path using this mapping (index/field name).");
    tt++;
    
    // Parameter 'field_mapping'
    // ctype is '_field_map_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRUCT_TYPE;
    tt->param_name = tdrpStrDup("field_mapping");
    tt->descr = tdrpStrDup("mapping from field to url");
    tt->help = tdrpStrDup("for each input field, the index to the path of that data");
    tt->array_offset = (char *) &_field_mapping - &_start_;
    tt->array_n_offset = (char *) &field_mapping_n - &_start_;
    tt->is_array = TRUE;
    tt->array_len_fixed = FALSE;
    tt->array_elem_size = sizeof(field_map_t);
    tt->array_n = 0;
    tt->struct_def.name = tdrpStrDup("field_map_t");
    tt->struct_def.nfields = 2;
    tt->struct_def.fields = (struct_field_t *)
        tdrpMalloc(tt->struct_def.nfields * sizeof(struct_field_t));
      tt->struct_def.fields[0].ftype = tdrpStrDup("string");
      tt->struct_def.fields[0].fname = tdrpStrDup("field");
      tt->struct_def.fields[0].ptype = STRING_TYPE;
      tt->struct_def.fields[0].rel_offset = 
        (char *) &_field_mapping->field - (char *) _field_mapping;
      tt->struct_def.fields[1].ftype = tdrpStrDup("int");
      tt->struct_def.fields[1].fname = tdrpStrDup("index");
      tt->struct_def.fields[1].ptype = INT_TYPE;
      tt->struct_def.fields[1].rel_offset = 
        (char *) &_field_mapping->index - (char *) _field_mapping;
    tt->n_struct_vals = 0;
    tt->struct_vals = (tdrpVal_t *)
        tdrpMalloc(tt->n_struct_vals * sizeof(tdrpVal_t));
    tt++;
    
    // Parameter 'Comment 4'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = COMMENT_TYPE;
    tt->param_name = tdrpStrDup("Comment 4");
    tt->comment_hdr = tdrpStrDup("Data Read Parameters");
    tt->comment_text = tdrpStrDup(" Parameters that control the reading of input data ");
    tt++;
    
    // Parameter 'max_wait_minutes'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("max_wait_minutes");
    tt->descr = tdrpStrDup("max wait");
    tt->help = tdrpStrDup("maximum wait time (minutes) for realtime=TRUE and multiple input URLs. Not used in archive mode");
    tt->val_offset = (char *) &max_wait_minutes - &_start_;
    tt->single_val.d = 1;
    tt++;
    
    // Parameter 'max_realtime_data_age_secs'
    // ctype is 'int'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = INT_TYPE;
    tt->param_name = tdrpStrDup("max_realtime_data_age_secs");
    tt->descr = tdrpStrDup("max_realtime_data_age_secs");
    tt->help = tdrpStrDup("maximum allowed difference (seconds) between read request time and data time");
    tt->val_offset = (char *) &max_realtime_data_age_secs - &_start_;
    tt->single_val.i = 120;
    tt++;
    
    // Parameter 'read_set_fixed_angle_limits'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("read_set_fixed_angle_limits");
    tt->descr = tdrpStrDup("Option to set fixed angle limits");
    tt->help = tdrpStrDup("Only read sweeps within the specified fixed angle limits. Fixed angles are elevation in PPI mode and azimuth in RHI mode.");
    tt->val_offset = (char *) &read_set_fixed_angle_limits - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'read_lower_fixed_angle'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("read_lower_fixed_angle");
    tt->descr = tdrpStrDup("Lower fixed angle limit - degrees.");
    tt->help = tdrpStrDup("");
    tt->val_offset = (char *) &read_lower_fixed_angle - &_start_;
    tt->single_val.d = 0;
    tt++;
    
    // Parameter 'read_upper_fixed_angle'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("read_upper_fixed_angle");
    tt->descr = tdrpStrDup("Upper fixed angle limit - degrees.");
    tt->help = tdrpStrDup("");
    tt->val_offset = (char *) &read_upper_fixed_angle - &_start_;
    tt->single_val.d = 90;
    tt++;
    
    // Parameter 'ignore_antenna_transitions'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("ignore_antenna_transitions");
    tt->descr = tdrpStrDup("Option to ignore rays with antenna transition flag set.");
    tt->help = tdrpStrDup("The transition flag is set when the antenna is moving between sweeps. If this parameter is true, rays containing the transition flag will not be read in.");
    tt->val_offset = (char *) &ignore_antenna_transitions - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'ignore_idle_scan_mode_on_read'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("ignore_idle_scan_mode_on_read");
    tt->descr = tdrpStrDup("Option to ignore data taken in IDLE mode.");
    tt->help = tdrpStrDup("If true, on read will ignore files with an IDLE scan mode.");
    tt->val_offset = (char *) &ignore_idle_scan_mode_on_read - &_start_;
    tt->single_val.b = pTRUE;
    tt++;
    
    // Parameter 'set_max_range'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("set_max_range");
    tt->descr = tdrpStrDup("Option to set the max range for any ray.");
    tt->help = tdrpStrDup("");
    tt->val_offset = (char *) &set_max_range - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'max_range_km'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("max_range_km");
    tt->descr = tdrpStrDup("Specified maximim range - km.");
    tt->help = tdrpStrDup("Gates beyond this range are removed.");
    tt->val_offset = (char *) &max_range_km - &_start_;
    tt->single_val.d = 9999;
    tt++;
    
    // Parameter 'Comment 5'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = COMMENT_TYPE;
    tt->param_name = tdrpStrDup("Comment 5");
    tt->comment_hdr = tdrpStrDup("Data Write Parameters");
    tt->comment_text = tdrpStrDup(" Parameters that control the writing of output data ");
    tt++;
    
    // Parameter 'netcdf_style'
    // ctype is '_netcdf_style_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = ENUM_TYPE;
    tt->param_name = tdrpStrDup("netcdf_style");
    tt->descr = tdrpStrDup("NetCDF style - if output_format is CFRADIAL");
    tt->help = tdrpStrDup("netCDF classic format, netCDF 64-bit offset format, netCDF4 using HDF5 format, netCDF4 using HDF5 format but only netCDF3 calls");
    tt->val_offset = (char *) &netcdf_style - &_start_;
    tt->enum_def.name = tdrpStrDup("netcdf_style_t");
    tt->enum_def.nfields = 4;
    tt->enum_def.fields = (enum_field_t *)
        tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
      tt->enum_def.fields[0].name = tdrpStrDup("CLASSIC");
      tt->enum_def.fields[0].val = CLASSIC;
      tt->enum_def.fields[1].name = tdrpStrDup("NC64BIT");
      tt->enum_def.fields[1].val = NC64BIT;
      tt->enum_def.fields[2].name = tdrpStrDup("NETCDF4");
      tt->enum_def.fields[2].val = NETCDF4;
      tt->enum_def.fields[3].name = tdrpStrDup("NETCDF4_CLASSIC");
      tt->enum_def.fields[3].val = NETCDF4_CLASSIC;
    tt->single_val.e = CLASSIC;
    tt++;
    
    // Parameter 'Comment 6'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = COMMENT_TYPE;
    tt->param_name = tdrpStrDup("Comment 6");
    tt->comment_hdr = tdrpStrDup("OUTPUT BYTE-SWAPPING and COMPRESSION");
    tt->comment_text = tdrpStrDup("");
    tt++;
    
    // Parameter 'output_native_byte_order'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("output_native_byte_order");
    tt->descr = tdrpStrDup("Option to leave data in native byte order.");
    tt->help = tdrpStrDup("If false, data will be byte-swapped as appropriate on output.");
    tt->val_offset = (char *) &output_native_byte_order - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'output_compressed'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("output_compressed");
    tt->descr = tdrpStrDup("Option to compress data fields on output.");
    tt->help = tdrpStrDup("Applies to netCDF and Dorade. UF does not support compression.");
    tt->val_offset = (char *) &output_compressed - &_start_;
    tt->single_val.b = pTRUE;
    tt++;
    
    // Parameter 'compression_level'
    // ctype is 'int'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = INT_TYPE;
    tt->param_name = tdrpStrDup("compression_level");
    tt->descr = tdrpStrDup("Compression level for output, if compressed.");
    tt->help = tdrpStrDup("Applies to netCDF only. Dorade compression is run-length encoding, and has not options..");
    tt->val_offset = (char *) &compression_level - &_start_;
    tt->single_val.i = 5;
    tt++;
    
    // Parameter 'output_filename_mode'
    // ctype is '_filename_mode_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = ENUM_TYPE;
    tt->param_name = tdrpStrDup("output_filename_mode");
    tt->descr = tdrpStrDup("Mode for computing output file name.");
    tt->help = tdrpStrDup("START_AND_END_TIMES: include both start and end times in file name. START_TIME_ONLY: include only start time in file name. END_TIME_ONLY: include only end time in file name.");
    tt->val_offset = (char *) &output_filename_mode - &_start_;
    tt->enum_def.name = tdrpStrDup("filename_mode_t");
    tt->enum_def.nfields = 3;
    tt->enum_def.fields = (enum_field_t *)
        tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
      tt->enum_def.fields[0].name = tdrpStrDup("START_AND_END_TIMES");
      tt->enum_def.fields[0].val = START_AND_END_TIMES;
      tt->enum_def.fields[1].name = tdrpStrDup("START_TIME_ONLY");
      tt->enum_def.fields[1].val = START_TIME_ONLY;
      tt->enum_def.fields[2].name = tdrpStrDup("END_TIME_ONLY");
      tt->enum_def.fields[2].val = END_TIME_ONLY;
    tt->single_val.e = START_AND_END_TIMES;
    tt++;
    
    // Parameter 'append_day_dir_to_output_dir'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("append_day_dir_to_output_dir");
    tt->descr = tdrpStrDup("Add the day directory to the output directory.");
    tt->help = tdrpStrDup("Path will be output_dir/yyyymmdd/filename.");
    tt->val_offset = (char *) &append_day_dir_to_output_dir - &_start_;
    tt->single_val.b = pTRUE;
    tt++;
    
    // Parameter 'append_year_dir_to_output_dir'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("append_year_dir_to_output_dir");
    tt->descr = tdrpStrDup("Add the year directory to the output directory.");
    tt->help = tdrpStrDup("Path will be output_dir/yyyy/yyyymmdd/filename.");
    tt->val_offset = (char *) &append_year_dir_to_output_dir - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'write_latest_data_info'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("write_latest_data_info");
    tt->descr = tdrpStrDup("Option to write out _latest_data_info files.");
    tt->help = tdrpStrDup("If true, the _latest_data_info files will be written after the converted file is written.");
    tt->val_offset = (char *) &write_latest_data_info - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // trailing entry has param_name set to NULL
    
    tt->param_name = NULL;
    
    return;
  
  }