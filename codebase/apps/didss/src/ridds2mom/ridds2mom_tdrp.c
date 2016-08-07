/*******************************************
 * ridds2mom_tdrp.c
 *
 * TDRP C code file 'ridds2mom' module.
 *
 * Code for program ridds2mom
 *
 * This file has been automatically
 * generated by TDRP, do not modify.
 *
 *******************************************/

#include "ridds2mom_tdrp.h"
#include <string.h>

/*
 * file scope variables
 */

static TDRPtable Table[55];
static ridds2mom_tdrp_struct *Params;
static char *Module = "ridds2mom";

/*************************************************************
 * ridds2mom_tdrp_load_from_args()
 *
 * Loads up TDRP using the command line args.
 *
 * Check TDRP_usage() for command line actions associated with
 * this function.
 *
 *   argc, argv: command line args
 *
 *   ridds2mom_tdrp_struct *params: loads up this struct
 * 
 *   char **override_list: A null-terminated list of overrides
 *     to the parameter file.
 *     An override string has exactly the format of an entry
 *     in the parameter file itself.
 *
 *   char **params_path_p: if non-NULL, this is set to point to
 *                         the path of the params file used.
 *
 *  Returns 0 on success, -1 on failure.
 */

int ridds2mom_tdrp_load_from_args(int argc, char **argv,
                         ridds2mom_tdrp_struct *params,
                         char **override_list,
                         char **params_path_p)
{
  Params = params;
  ridds2mom_tdrp_init(Params);
  if (tdrpLoadFromArgs(argc, argv,
                       Table, Params,
                       override_list, params_path_p)) {
    return (-1);
  } else {
    return (0);
  }
}

/*************************************************************
 * ridds2mom_tdrp_load()
 *
 * Loads up TDRP for a given module.
 *
 * This version of load gives the programmer the option to load
 * up more than one module for a single application. It is a
 * lower-level routine than ridds2mom_tdrp_load_from_args,
 * and hence more flexible, but the programmer must do more work.
 *
 *   char *param_file_path: the parameter file to be read in.
 *
 *   ridds2mom_tdrp_struct *params: loads up this struct
 *
 *   char **override_list: A null-terminated list of overrides
 *     to the parameter file.
 *     An override string has exactly the format of an entry
 *     in the parameter file itself.
 *
 *  expand_env: flag to control environment variable
 *                expansion during tokenization.
 *              If TRUE, environment expansion is set on.
 *              If FALSE, environment expansion is set off.
 *
 *  Returns 0 on success, -1 on failure.
 */

int ridds2mom_tdrp_load(char *param_file_path,
               ridds2mom_tdrp_struct *params,
               char **override_list,
               int expand_env, int debug)
{
  Params = params;
  ridds2mom_tdrp_init(Params);
  if (tdrpLoad(param_file_path, Table,
               params, override_list,
               expand_env, debug)) {
    return (-1);
  } else {
    return (0);
  }
}

/*************************************************************
 * ridds2mom_tdrp_load_defaults()
 *
 * Loads up defaults for a given module.
 *
 * See ridds2mom_tdrp_load() for more details.
 *
 *  Returns 0 on success, -1 on failure.
 */

int ridds2mom_tdrp_load_defaults(ridds2mom_tdrp_struct *params,
                        int expand_env)
{
  Params = params;
  ridds2mom_tdrp_init(Params);
  if (tdrpLoad(NULL, Table,
               params, NULL,
               expand_env, FALSE)) {
    return (-1);
  } else {
    return (0);
  }
}

/*************************************************************
 * ridds2mom_tdrp_sync()
 *
 * Syncs the user struct data back into the parameter table,
 * in preparation for printing.
 */

void ridds2mom_tdrp_sync(void)
{
  tdrpUser2Table(Table, Params);
}

/*************************************************************
 * ridds2mom_tdrp_print()
 * 
 * Print params file
 *
 * The modes supported are:
 *
 *   PRINT_SHORT:   main comments only, no help or descriptions
 *                  structs and arrays on a single line
 *   PRINT_NORM:    short + descriptions and help
 *   PRINT_LONG:    norm  + arrays and structs expanded
 *   PRINT_VERBOSE: long  + private params included
 */

void ridds2mom_tdrp_print(FILE *out, tdrp_print_mode_t mode)
{
  tdrpPrint(out, Table, Module, mode);
}

/*************************************************************
 * ridds2mom_tdrp_check_all_set()
 *
 * Return TRUE if all set, FALSE if not.
 *
 * If out is non-NULL, prints out warning messages for those
 * parameters which are not set.
 */

int ridds2mom_tdrp_check_all_set(FILE *out)
{
  return (tdrpCheckAllSet(out, Table, Params));
}

/*************************************************************
 * ridds2mom_tdrp_check_is_set()
 *
 * Return TRUE if parameter is set, FALSE if not.
 *
 */

int ridds2mom_tdrp_check_is_set(char *param_name)
{
  return (tdrpCheckIsSet(param_name, Table, Params));
}

/*************************************************************
 * ridds2mom_tdrp_free_all()
 *
 * Frees up all TDRP dynamic memory.
 */

void ridds2mom_tdrp_free_all(void)
{
  tdrpFreeAll(Table, Params);
}

/*************************************************************
 * ridds2mom_tdrp_array_realloc()
 *
 * Realloc 1D array.
 *
 * If size is increased, the values from the last array entry is
 * copied into the new space.
 *
 * Returns 0 on success, -1 on error.
 */

int ridds2mom_tdrp_array_realloc(char *param_name, int new_array_n)
{
  if (tdrpArrayRealloc(Table, Params, param_name, new_array_n)) {
    return (-1);
  } else {
    return (0);
  }
}

/*************************************************************
 * ridds2mom_tdrp_array2D_realloc()
 *
 * Realloc 2D array.
 *
 * If size is increased, the values from the last array entry is
 * copied into the new space.
 *
 * Returns 0 on success, -1 on error.
 */

int ridds2mom_tdrp_array2D_realloc(char *param_name,
                          int new_array_n1,
                          int new_array_n2)
{
  if (tdrpArray2DRealloc(Table, Params, param_name,
			 new_array_n1, new_array_n2)) {
    return (-1);
  } else {
    return (0);
  }
}

/*************************************************************
 * ridds2mom_tdrp_table()
 *
 * Returns pointer to static Table for this module.
 */

TDRPtable *ridds2mom_tdrp_table(void)
{
  return (Table);
}

/*************************************************************
 * ridds2mom_tdrp_init()
 *
 * Module table initialization function.
 *
 *
 * Returns pointer to static Table for this module.
 */

TDRPtable *ridds2mom_tdrp_init(ridds2mom_tdrp_struct *params)

{

  TDRPtable *tt = Table;

  ridds2mom_tdrp_struct pp; /* for computing byte_offsets */

  /* zero out struct, and store size */

  memset(params, 0, sizeof(ridds2mom_tdrp_struct));
  params->struct_size = sizeof(ridds2mom_tdrp_struct);

  /* Parameter 'debug' */
  /* ctype is 'ridds2mom_debug' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = ENUM_TYPE;
  tt->param_name = tdrpStrDup("debug");
  tt->descr = tdrpStrDup("Debug option");
  tt->help = tdrpStrDup("If set, debug messages will be printed appropriately. Use DEBUG_VERBOSE for all debug messages.");
  tt->val_offset = (char *) &(pp.debug) - (char *) &pp;
  tt->enum_def.name = tdrpStrDup("debug");
  tt->enum_def.nfields = 3;
  tt->enum_def.fields = (enum_field_t *)
      tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
    tt->enum_def.fields[0].name = tdrpStrDup("DEBUG_OFF");
    tt->enum_def.fields[0].val = DEBUG_OFF;
    tt->enum_def.fields[1].name = tdrpStrDup("DEBUG_NORM");
    tt->enum_def.fields[1].val = DEBUG_NORM;
    tt->enum_def.fields[2].name = tdrpStrDup("DEBUG_VERBOSE");
    tt->enum_def.fields[2].val = DEBUG_VERBOSE;
  tt->single_val.e = DEBUG_OFF;
  tt++;
  
  /* Parameter 'instance' */
  /* ctype is 'char*' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = STRING_TYPE;
  tt->param_name = tdrpStrDup("instance");
  tt->descr = tdrpStrDup("Process instance.");
  tt->help = tdrpStrDup("This is used by procmap utilities for registering the process with procmap.");
  tt->val_offset = (char *) &(pp.instance) - (char *) &pp;
  tt->single_val.s = tdrpStrDup("Test");
  tt++;
  
  /* Parameter 'malloc_debug_level' */
  /* ctype is 'int' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = INT_TYPE;
  tt->param_name = tdrpStrDup("malloc_debug_level");
  tt->descr = tdrpStrDup("Malloc debug level");
  tt->help = tdrpStrDup("0 - none, 1 - corruption checking, 2 - records all malloc blocks and checks, 3 - printout of all mallocs etc.");
  tt->val_offset = (char *) &(pp.malloc_debug_level) - (char *) &pp;
  tt->has_min = TRUE;
  tt->has_max = TRUE;
  tt->min_val.i = 0;
  tt->max_val.i = 3;
  tt->single_val.i = 0;
  tt++;
  
  /* Parameter 'input_stream_format' */
  /* ctype is 'ridds2mom_input_stream_format' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = ENUM_TYPE;
  tt->param_name = tdrpStrDup("input_stream_format");
  tt->descr = tdrpStrDup("Input data stream format.");
  tt->help = tdrpStrDup("Input data stream may be expected in Nexrad format (alternating ref/vel at the lower tilts), or constant format (both ref & vel at every tilt).");
  tt->val_offset = (char *) &(pp.input_stream_format) - (char *) &pp;
  tt->enum_def.name = tdrpStrDup("input_stream_format");
  tt->enum_def.nfields = 2;
  tt->enum_def.fields = (enum_field_t *)
      tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
    tt->enum_def.fields[0].name = tdrpStrDup("NEXRAD_FORMAT");
    tt->enum_def.fields[0].val = NEXRAD_FORMAT;
    tt->enum_def.fields[1].name = tdrpStrDup("CONSTANT_FORMAT");
    tt->enum_def.fields[1].val = CONSTANT_FORMAT;
  tt->single_val.e = NEXRAD_FORMAT;
  tt++;
  
  /* Parameter 'input_device' */
  /* ctype is 'ridds2mom_input_device' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = ENUM_TYPE;
  tt->param_name = tdrpStrDup("input_device");
  tt->descr = tdrpStrDup("Input device type.");
  tt->help = tdrpStrDup("For IN_TAPE, set input_tape_name, input_tape wait and short_tape. For IN_UDP, set input_udp_port. For IN_SHMEM, set input_shmem_key. IN_SHMEM is not supported yet.");
  tt->val_offset = (char *) &(pp.input_device) - (char *) &pp;
  tt->enum_def.name = tdrpStrDup("input_device");
  tt->enum_def.nfields = 3;
  tt->enum_def.fields = (enum_field_t *)
      tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
    tt->enum_def.fields[0].name = tdrpStrDup("IN_TAPE");
    tt->enum_def.fields[0].val = IN_TAPE;
    tt->enum_def.fields[1].name = tdrpStrDup("IN_UDP");
    tt->enum_def.fields[1].val = IN_UDP;
    tt->enum_def.fields[2].name = tdrpStrDup("IN_SHMEM");
    tt->enum_def.fields[2].val = IN_SHMEM;
  tt->single_val.e = IN_UDP;
  tt++;
  
  /* Parameter 'input_tape_name' */
  /* ctype is 'char*' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = STRING_TYPE;
  tt->param_name = tdrpStrDup("input_tape_name");
  tt->descr = tdrpStrDup("Tape device name for input. Applicable if input_device is IN_TAPE.");
  tt->help = tdrpStrDup("");
  tt->val_offset = (char *) &(pp.input_tape_name) - (char *) &pp;
  tt->single_val.s = tdrpStrDup("/dev/nrst0");
  tt++;
  
  /* Parameter 'input_tape_wait' */
  /* ctype is 'int' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = INT_TYPE;
  tt->param_name = tdrpStrDup("input_tape_wait");
  tt->descr = tdrpStrDup("Tape input read delay (millisecs).");
  tt->help = tdrpStrDup("Wait between beam reads, to allow the user to simulate varying data rates. Applicable if input_device is IN_TAPE.");
  tt->val_offset = (char *) &(pp.input_tape_wait) - (char *) &pp;
  tt->single_val.i = 30;
  tt++;
  
  /* Parameter 'short_tape' */
  /* ctype is 'tdrp_bool_t' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = BOOL_TYPE;
  tt->param_name = tdrpStrDup("short_tape");
  tt->descr = tdrpStrDup("Short tape option.");
  tt->help = tdrpStrDup("Set to TRUE if reading a standard NCDC Nexrad Level 2 archive tape.  Set to FALSE if reading a RAP generated Level 2 archive tape.  Applicable if input_device is IN_TAPE.");
  tt->val_offset = (char *) &(pp.short_tape) - (char *) &pp;
  tt->single_val.b = pFALSE;
  tt++;
  
  /* Parameter 'input_udp_port' */
  /* ctype is 'int' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = INT_TYPE;
  tt->param_name = tdrpStrDup("input_udp_port");
  tt->descr = tdrpStrDup("Input udp port number. Applicable if input_device is IN_UDP.");
  tt->help = tdrpStrDup("");
  tt->val_offset = (char *) &(pp.input_udp_port) - (char *) &pp;
  tt->single_val.i = 3280;
  tt++;
  
  /* Parameter 'input_shmem_key' */
  /* ctype is 'int' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = INT_TYPE;
  tt->param_name = tdrpStrDup("input_shmem_key");
  tt->descr = tdrpStrDup("Shared memory key for input queue. Applicable if input_device is IN_SHMEM.");
  tt->help = tdrpStrDup("");
  tt->val_offset = (char *) &(pp.input_shmem_key) - (char *) &pp;
  tt->single_val.i = 2750400;
  tt++;
  
  /* Parameter 'output_format' */
  /* ctype is 'ridds2mom_output_format' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = ENUM_TYPE;
  tt->param_name = tdrpStrDup("output_format");
  tt->descr = tdrpStrDup("Output format type.");
  tt->help = tdrpStrDup("Output may be in Lincoln format (LL_FORMAT), DsRadar format (DS_FORMAT) or unchanged from input (RIDDS_FORMAT).");
  tt->val_offset = (char *) &(pp.output_format) - (char *) &pp;
  tt->enum_def.name = tdrpStrDup("output_format");
  tt->enum_def.nfields = 3;
  tt->enum_def.fields = (enum_field_t *)
      tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
    tt->enum_def.fields[0].name = tdrpStrDup("LL_FORMAT");
    tt->enum_def.fields[0].val = LL_FORMAT;
    tt->enum_def.fields[1].name = tdrpStrDup("DS_FORMAT");
    tt->enum_def.fields[1].val = DS_FORMAT;
    tt->enum_def.fields[2].name = tdrpStrDup("RIDDS_FORMAT");
    tt->enum_def.fields[2].val = RIDDS_FORMAT;
  tt->single_val.e = LL_FORMAT;
  tt++;
  
  /* Parameter 'output_device' */
  /* ctype is 'ridds2mom_output_device' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = ENUM_TYPE;
  tt->param_name = tdrpStrDup("output_device");
  tt->descr = tdrpStrDup("Primary output device type.");
  tt->help = tdrpStrDup("There must be at least 1 output device, and you use this parameter to set that primary device. If, for example, you set output_device to OUT_FMQ, this has the effect of overriding write_fmq_output and setting it to TRUE. In addition to the primary output device, your can enable secondary outputs by setting any of write_fmq_output, write_udp_output or write_shmem_output to TRUE. For OUT_UDP, set output_udp_port and output_udp_address. For OUT_SHMEM, set output_shmem_key. For OUT_FMQ, set output_fmq_url, output_fmq_size, output_fmq_nslots and output_fmq_compress.");
  tt->val_offset = (char *) &(pp.output_device) - (char *) &pp;
  tt->enum_def.name = tdrpStrDup("output_device");
  tt->enum_def.nfields = 3;
  tt->enum_def.fields = (enum_field_t *)
      tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
    tt->enum_def.fields[0].name = tdrpStrDup("OUT_UDP");
    tt->enum_def.fields[0].val = OUT_UDP;
    tt->enum_def.fields[1].name = tdrpStrDup("OUT_SHMEM");
    tt->enum_def.fields[1].val = OUT_SHMEM;
    tt->enum_def.fields[2].name = tdrpStrDup("OUT_FMQ");
    tt->enum_def.fields[2].val = OUT_FMQ;
  tt->single_val.e = OUT_UDP;
  tt++;
  
  /* Parameter 'write_fmq_output' */
  /* ctype is 'tdrp_bool_t' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = BOOL_TYPE;
  tt->param_name = tdrpStrDup("write_fmq_output");
  tt->descr = tdrpStrDup("Option to output FMQ in addition to chosen output_device.");
  tt->help = tdrpStrDup("If set to TRUE, FMQ is output in addition to the strean chosen by output_device.");
  tt->val_offset = (char *) &(pp.write_fmq_output) - (char *) &pp;
  tt->single_val.b = pFALSE;
  tt++;
  
  /* Parameter 'write_udp_output' */
  /* ctype is 'tdrp_bool_t' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = BOOL_TYPE;
  tt->param_name = tdrpStrDup("write_udp_output");
  tt->descr = tdrpStrDup("Option to output UDP in addition to chosen output_device.");
  tt->help = tdrpStrDup("If set to TRUE, UDP is output in addition to the strean chosen by output_device.");
  tt->val_offset = (char *) &(pp.write_udp_output) - (char *) &pp;
  tt->single_val.b = pFALSE;
  tt++;
  
  /* Parameter 'write_shmem_output' */
  /* ctype is 'tdrp_bool_t' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = BOOL_TYPE;
  tt->param_name = tdrpStrDup("write_shmem_output");
  tt->descr = tdrpStrDup("Option to output SHMEM in addition to chosen output_device.");
  tt->help = tdrpStrDup("If set to TRUE, SHMEM is output in addition to the strean chosen by output_device.");
  tt->val_offset = (char *) &(pp.write_shmem_output) - (char *) &pp;
  tt->single_val.b = pFALSE;
  tt++;
  
  /* Parameter 'output_udp_port' */
  /* ctype is 'int' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = INT_TYPE;
  tt->param_name = tdrpStrDup("output_udp_port");
  tt->descr = tdrpStrDup("Output udp port number.");
  tt->help = tdrpStrDup("");
  tt->val_offset = (char *) &(pp.output_udp_port) - (char *) &pp;
  tt->single_val.i = 54321;
  tt++;
  
  /* Parameter 'output_udp_address' */
  /* ctype is 'char*' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = STRING_TYPE;
  tt->param_name = tdrpStrDup("output_udp_address");
  tt->descr = tdrpStrDup("Broadcast address for UDP output.");
  tt->help = tdrpStrDup("You can use 'ifconfig -a' (as root) to check which network connections you have.");
  tt->val_offset = (char *) &(pp.output_udp_address) - (char *) &pp;
  tt->single_val.s = tdrpStrDup("128.117.199.255");
  tt++;
  
  /* Parameter 'output_shmem_key' */
  /* ctype is 'int' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = INT_TYPE;
  tt->param_name = tdrpStrDup("output_shmem_key");
  tt->descr = tdrpStrDup("Shared memory key for output queue.");
  tt->help = tdrpStrDup("");
  tt->val_offset = (char *) &(pp.output_shmem_key) - (char *) &pp;
  tt->single_val.i = 3750400;
  tt++;
  
  /* Parameter 'output_fmq_url' */
  /* ctype is 'char*' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = STRING_TYPE;
  tt->param_name = tdrpStrDup("output_fmq_url");
  tt->descr = tdrpStrDup("Output URL for DsRadar data via FMQ");
  tt->help = tdrpStrDup("");
  tt->val_offset = (char *) &(pp.output_fmq_url) - (char *) &pp;
  tt->single_val.s = tdrpStrDup("fmqp:://localhost::./fmq.dsRadar");
  tt++;
  
  /* Parameter 'output_fmq_size' */
  /* ctype is 'int' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = INT_TYPE;
  tt->param_name = tdrpStrDup("output_fmq_size");
  tt->descr = tdrpStrDup("Size of output FMQ, in bytes.");
  tt->help = tdrpStrDup("This is the total size of the output FMQ buffer. Some of this buffer will be used for control bytes (12 bytes per message.");
  tt->val_offset = (char *) &(pp.output_fmq_size) - (char *) &pp;
  tt->single_val.i = 2000000;
  tt++;
  
  /* Parameter 'output_fmq_nslots' */
  /* ctype is 'int' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = INT_TYPE;
  tt->param_name = tdrpStrDup("output_fmq_nslots");
  tt->descr = tdrpStrDup("Number of slots in output FMQ.");
  tt->help = tdrpStrDup("The number of slots corresponds to the maximum number of messages which may be written to the buffer before overwrites occur. However, overwrites may occur sooner if the size is not set large enough.");
  tt->val_offset = (char *) &(pp.output_fmq_nslots) - (char *) &pp;
  tt->single_val.i = 3600;
  tt++;
  
  /* Parameter 'output_fmq_compress' */
  /* ctype is 'tdrp_bool_t' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = BOOL_TYPE;
  tt->param_name = tdrpStrDup("output_fmq_compress");
  tt->descr = tdrpStrDup("FMQ compression option.");
  tt->help = tdrpStrDup("If TRUE FMQ messages are compressed.");
  tt->val_offset = (char *) &(pp.output_fmq_compress) - (char *) &pp;
  tt->single_val.b = pFALSE;
  tt++;
  
  /* Parameter 'output_ds_dbz' */
  /* ctype is 'tdrp_bool_t' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = BOOL_TYPE;
  tt->param_name = tdrpStrDup("output_ds_dbz");
  tt->descr = tdrpStrDup("Option to output DBZ (reflectivity) field for DS_FORMAT output.");
  tt->help = tdrpStrDup("Only applicable it output_format = DS_FORMAT.");
  tt->val_offset = (char *) &(pp.output_ds_dbz) - (char *) &pp;
  tt->single_val.b = pTRUE;
  tt++;
  
  /* Parameter 'output_ds_vel' */
  /* ctype is 'tdrp_bool_t' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = BOOL_TYPE;
  tt->param_name = tdrpStrDup("output_ds_vel");
  tt->descr = tdrpStrDup("Option to output VEL (radial velocity) field for DS_FORMAT output.");
  tt->help = tdrpStrDup("Only applicable it output_format = DS_FORMAT.");
  tt->val_offset = (char *) &(pp.output_ds_vel) - (char *) &pp;
  tt->single_val.b = pTRUE;
  tt++;
  
  /* Parameter 'output_ds_spw' */
  /* ctype is 'tdrp_bool_t' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = BOOL_TYPE;
  tt->param_name = tdrpStrDup("output_ds_spw");
  tt->descr = tdrpStrDup("Option to output SPW (spectral width) field for DS_FORMAT output.");
  tt->help = tdrpStrDup("Only applicable it output_format = DS_FORMAT.");
  tt->val_offset = (char *) &(pp.output_ds_spw) - (char *) &pp;
  tt->single_val.b = pTRUE;
  tt++;
  
  /* Parameter 'output_ds_snr' */
  /* ctype is 'tdrp_bool_t' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = BOOL_TYPE;
  tt->param_name = tdrpStrDup("output_ds_snr");
  tt->descr = tdrpStrDup("Option to output SNR (signal to noise) field for DS_FORMAT output.");
  tt->help = tdrpStrDup("Only applicable it output_format = DS_FORMAT.");
  tt->val_offset = (char *) &(pp.output_ds_snr) - (char *) &pp;
  tt->single_val.b = pTRUE;
  tt++;
  
  /* Parameter 'vol_coverage_path' */
  /* ctype is 'char*' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = STRING_TYPE;
  tt->param_name = tdrpStrDup("vol_coverage_path");
  tt->descr = tdrpStrDup("Path of volume coverage file.");
  tt->help = tdrpStrDup("This file contains the NEXRAD scan strategies. It is used to compute the tilt numbers.");
  tt->val_offset = (char *) &(pp.vol_coverage_path) - (char *) &pp;
  tt->single_val.s = tdrpStrDup("nexrad_vcp.conf");
  tt++;
  
  /* Parameter 'radar_id' */
  /* ctype is 'int' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = INT_TYPE;
  tt->param_name = tdrpStrDup("radar_id");
  tt->descr = tdrpStrDup("Radar ID.");
  tt->help = tdrpStrDup("Project specified ID for the radar - for DS_FORMAT only.");
  tt->val_offset = (char *) &(pp.radar_id) - (char *) &pp;
  tt->single_val.i = 0;
  tt++;
  
  /* Parameter 'radar_name' */
  /* ctype is 'char*' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = STRING_TYPE;
  tt->param_name = tdrpStrDup("radar_name");
  tt->descr = tdrpStrDup("Radar name.");
  tt->help = tdrpStrDup("");
  tt->val_offset = (char *) &(pp.radar_name) - (char *) &pp;
  tt->single_val.s = tdrpStrDup("NEXRAD");
  tt++;
  
  /* Parameter 'site_name' */
  /* ctype is 'char*' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = STRING_TYPE;
  tt->param_name = tdrpStrDup("site_name");
  tt->descr = tdrpStrDup("Site name.");
  tt->help = tdrpStrDup("");
  tt->val_offset = (char *) &(pp.site_name) - (char *) &pp;
  tt->single_val.s = tdrpStrDup("KFTG-DEN");
  tt++;
  
  /* Parameter 'radar_location' */
  /* ctype is 'ridds2mom_ZR' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = STRUCT_TYPE;
  tt->param_name = tdrpStrDup("radar_location");
  tt->descr = tdrpStrDup("Radar location");
  tt->help = tdrpStrDup("The NEXRAD data stream does not include radar position. Use this parameter to set the Lat and lon in degrees, alt in km.");
  tt->val_offset = (char *) &(pp.radar_location) - (char *) &pp;
  tt->struct_def.name = tdrpStrDup("ZR");
  tt->struct_def.nfields = 3;
  tt->struct_def.fields = (struct_field_t *)
      tdrpMalloc(tt->struct_def.nfields * sizeof(struct_field_t));
    tt->struct_def.fields[0].ftype = tdrpStrDup("double");
    tt->struct_def.fields[0].fname = tdrpStrDup("latitude");
    tt->struct_def.fields[0].ptype = DOUBLE_TYPE;
    tt->struct_def.fields[0].rel_offset = 
      (char *) &(pp.radar_location.latitude) - (char *) &(pp.radar_location);
    tt->struct_def.fields[1].ftype = tdrpStrDup("double");
    tt->struct_def.fields[1].fname = tdrpStrDup("longitude");
    tt->struct_def.fields[1].ptype = DOUBLE_TYPE;
    tt->struct_def.fields[1].rel_offset = 
      (char *) &(pp.radar_location.longitude) - (char *) &(pp.radar_location);
    tt->struct_def.fields[2].ftype = tdrpStrDup("double");
    tt->struct_def.fields[2].fname = tdrpStrDup("altitude");
    tt->struct_def.fields[2].ptype = DOUBLE_TYPE;
    tt->struct_def.fields[2].rel_offset = 
      (char *) &(pp.radar_location.altitude) - (char *) &(pp.radar_location);
  tt->n_struct_vals = 3;
  tt->struct_vals = (tdrpVal_t *)
      tdrpMalloc(tt->n_struct_vals * sizeof(tdrpVal_t));
    tt->struct_vals[0].d = 0;
    tt->struct_vals[1].d = 0;
    tt->struct_vals[2].d = 0;
  tt++;
  
  /* Parameter 'time_correction' */
  /* ctype is 'int' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = INT_TYPE;
  tt->param_name = tdrpStrDup("time_correction");
  tt->descr = tdrpStrDup("Time correction (secs).");
  tt->help = tdrpStrDup("Data time is adjusted by this number of seconds before output.");
  tt->val_offset = (char *) &(pp.time_correction) - (char *) &pp;
  tt->single_val.i = 0;
  tt++;
  
  /* Parameter 'use_wallclock_time' */
  /* ctype is 'tdrp_bool_t' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = BOOL_TYPE;
  tt->param_name = tdrpStrDup("use_wallclock_time");
  tt->descr = tdrpStrDup("Wallclock time override option.");
  tt->help = tdrpStrDup("If set, wallclock time will be used to override data time.");
  tt->val_offset = (char *) &(pp.use_wallclock_time) - (char *) &pp;
  tt->single_val.b = pFALSE;
  tt++;
  
  /* Parameter 'samples_per_beam' */
  /* ctype is 'int' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = INT_TYPE;
  tt->param_name = tdrpStrDup("samples_per_beam");
  tt->descr = tdrpStrDup("Samples per beam");
  tt->help = tdrpStrDup("Number of pulses used to compute data from a beam");
  tt->val_offset = (char *) &(pp.samples_per_beam) - (char *) &pp;
  tt->has_min = TRUE;
  tt->min_val.i = 1;
  tt->single_val.i = 64;
  tt++;
  
  /* Parameter 'polarization_code' */
  /* ctype is 'int' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = INT_TYPE;
  tt->param_name = tdrpStrDup("polarization_code");
  tt->descr = tdrpStrDup("Radar polarization code.");
  tt->help = tdrpStrDup("1 = hor, 2 = ver, 3 = circ, 4 = ellip. Should be 3; 1 for Jing compatibility.");
  tt->val_offset = (char *) &(pp.polarization_code) - (char *) &pp;
  tt->single_val.i = 3;
  tt++;
  
  /* Parameter 'beam_width' */
  /* ctype is 'double' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = DOUBLE_TYPE;
  tt->param_name = tdrpStrDup("beam_width");
  tt->descr = tdrpStrDup("Beam width");
  tt->help = tdrpStrDup("Radar half-power beam width (deg)");
  tt->val_offset = (char *) &(pp.beam_width) - (char *) &pp;
  tt->has_min = TRUE;
  tt->has_max = TRUE;
  tt->min_val.d = 0.01;
  tt->max_val.d = 45;
  tt->single_val.d = 0.95;
  tt++;
  
  /* Parameter 'wavelength' */
  /* ctype is 'double' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = DOUBLE_TYPE;
  tt->param_name = tdrpStrDup("wavelength");
  tt->descr = tdrpStrDup("Wavelength");
  tt->help = tdrpStrDup("Radar wavelength (cm)");
  tt->val_offset = (char *) &(pp.wavelength) - (char *) &pp;
  tt->has_min = TRUE;
  tt->has_max = TRUE;
  tt->min_val.d = 0.01;
  tt->max_val.d = 1000;
  tt->single_val.d = 10;
  tt++;
  
  /* Parameter 'avg_xmit_pwr' */
  /* ctype is 'double' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = DOUBLE_TYPE;
  tt->param_name = tdrpStrDup("avg_xmit_pwr");
  tt->descr = tdrpStrDup("Average transmitter power (kW)");
  tt->help = tdrpStrDup("For output_format = LL_FORMAT only.");
  tt->val_offset = (char *) &(pp.avg_xmit_pwr) - (char *) &pp;
  tt->single_val.d = 7;
  tt++;
  
  /* Parameter 'peak_xmit_pwr' */
  /* ctype is 'double' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = DOUBLE_TYPE;
  tt->param_name = tdrpStrDup("peak_xmit_pwr");
  tt->descr = tdrpStrDup("Peak transmitter power (kW)");
  tt->help = tdrpStrDup("For output_format = DS_FORMAT only.");
  tt->val_offset = (char *) &(pp.peak_xmit_pwr) - (char *) &pp;
  tt->single_val.d = 1000;
  tt++;
  
  /* Parameter 'receiver_mds' */
  /* ctype is 'double' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = DOUBLE_TYPE;
  tt->param_name = tdrpStrDup("receiver_mds");
  tt->descr = tdrpStrDup("Receiver MDS (dBM)");
  tt->help = tdrpStrDup("Receiver minimum detectable signal (dMD) - for DS_FORMAT only.");
  tt->val_offset = (char *) &(pp.receiver_mds) - (char *) &pp;
  tt->single_val.d = -113;
  tt++;
  
  /* Parameter 'noise_dbz_at_100km' */
  /* ctype is 'double' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = DOUBLE_TYPE;
  tt->param_name = tdrpStrDup("noise_dbz_at_100km");
  tt->descr = tdrpStrDup("Receiver noise in dBZ.");
  tt->help = tdrpStrDup("Receiver noise in units of dBZ at a range of 100km.");
  tt->val_offset = (char *) &(pp.noise_dbz_at_100km) - (char *) &pp;
  tt->single_val.d = -14;
  tt++;
  
  /* Parameter 'receiver_gain' */
  /* ctype is 'double' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = DOUBLE_TYPE;
  tt->param_name = tdrpStrDup("receiver_gain");
  tt->descr = tdrpStrDup("Gain of the receiver (dB) - for DS_FORMAT only");
  tt->help = tdrpStrDup("");
  tt->val_offset = (char *) &(pp.receiver_gain) - (char *) &pp;
  tt->single_val.d = 0;
  tt++;
  
  /* Parameter 'antenna_gain' */
  /* ctype is 'double' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = DOUBLE_TYPE;
  tt->param_name = tdrpStrDup("antenna_gain");
  tt->descr = tdrpStrDup("Gain of the antenna (dB) - for DS_FORMAT only");
  tt->help = tdrpStrDup("");
  tt->val_offset = (char *) &(pp.antenna_gain) - (char *) &pp;
  tt->single_val.d = 45;
  tt++;
  
  /* Parameter 'system_gain' */
  /* ctype is 'double' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = DOUBLE_TYPE;
  tt->param_name = tdrpStrDup("system_gain");
  tt->descr = tdrpStrDup("Gain of the system (dB) - for DS_FORMAT only");
  tt->help = tdrpStrDup("");
  tt->val_offset = (char *) &(pp.system_gain) - (char *) &pp;
  tt->single_val.d = 0;
  tt++;
  
  /* Parameter 'print_summary' */
  /* ctype is 'tdrp_bool_t' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = BOOL_TYPE;
  tt->param_name = tdrpStrDup("print_summary");
  tt->descr = tdrpStrDup("Print summary option.");
  tt->help = tdrpStrDup("If TRUE summaries are printed at given interval.");
  tt->val_offset = (char *) &(pp.print_summary) - (char *) &pp;
  tt->single_val.b = pFALSE;
  tt++;
  
  /* Parameter 'summary_interval' */
  /* ctype is 'int' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = INT_TYPE;
  tt->param_name = tdrpStrDup("summary_interval");
  tt->descr = tdrpStrDup("Interval for summary printouts.");
  tt->help = tdrpStrDup("See print_summary.");
  tt->val_offset = (char *) &(pp.summary_interval) - (char *) &pp;
  tt->single_val.i = 90;
  tt++;
  
  /* Parameter 'print_header' */
  /* ctype is 'tdrp_bool_t' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = BOOL_TYPE;
  tt->param_name = tdrpStrDup("print_header");
  tt->descr = tdrpStrDup("Print header option.");
  tt->help = tdrpStrDup("If TRUE headers are printed at given interval.");
  tt->val_offset = (char *) &(pp.print_header) - (char *) &pp;
  tt->single_val.b = pFALSE;
  tt++;
  
  /* Parameter 'header_interval' */
  /* ctype is 'int' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = INT_TYPE;
  tt->param_name = tdrpStrDup("header_interval");
  tt->descr = tdrpStrDup("Interval for header printouts.");
  tt->help = tdrpStrDup("See print_header.");
  tt->val_offset = (char *) &(pp.header_interval) - (char *) &pp;
  tt->single_val.i = 360;
  tt++;
  
  /* Parameter 'write_archive_fmq' */
  /* ctype is 'tdrp_bool_t' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = BOOL_TYPE;
  tt->param_name = tdrpStrDup("write_archive_fmq");
  tt->descr = tdrpStrDup("Option to write an FMQ for archive purposes.");
  tt->help = tdrpStrDup("The archive FMQ is written in native RIDDS format. A separate program reads this FMQ and archives the data to tape.");
  tt->val_offset = (char *) &(pp.write_archive_fmq) - (char *) &pp;
  tt->single_val.b = pFALSE;
  tt++;
  
  /* Parameter 'archive_fmq_url' */
  /* ctype is 'char*' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = STRING_TYPE;
  tt->param_name = tdrpStrDup("archive_fmq_url");
  tt->descr = tdrpStrDup("Archive URL for DsRadar data via FMQ");
  tt->help = tdrpStrDup("");
  tt->val_offset = (char *) &(pp.archive_fmq_url) - (char *) &pp;
  tt->single_val.s = tdrpStrDup("fmqp:://localhost::./fmq.nexrad");
  tt++;
  
  /* Parameter 'archive_fmq_size' */
  /* ctype is 'int' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = INT_TYPE;
  tt->param_name = tdrpStrDup("archive_fmq_size");
  tt->descr = tdrpStrDup("Size of archive FMQ, in bytes.");
  tt->help = tdrpStrDup("This is the total size of the archive FMQ buffer. Some of this buffer will be used for control bytes (12 bytes per message.");
  tt->val_offset = (char *) &(pp.archive_fmq_size) - (char *) &pp;
  tt->single_val.i = 2000000;
  tt++;
  
  /* Parameter 'archive_fmq_nslots' */
  /* ctype is 'int' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = INT_TYPE;
  tt->param_name = tdrpStrDup("archive_fmq_nslots");
  tt->descr = tdrpStrDup("Number of slots in archive FMQ.");
  tt->help = tdrpStrDup("The number of slots corresponds to the maximum number of messages which may be written to the buffer before overwrites occur. However, overwrites may occur sooner if the size is not set large enough.");
  tt->val_offset = (char *) &(pp.archive_fmq_nslots) - (char *) &pp;
  tt->single_val.i = 3600;
  tt++;
  
  /* Parameter 'archive_fmq_compress' */
  /* ctype is 'tdrp_bool_t' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = BOOL_TYPE;
  tt->param_name = tdrpStrDup("archive_fmq_compress");
  tt->descr = tdrpStrDup("Archive FMQ compression option.");
  tt->help = tdrpStrDup("If TRUE FMQ messages are compressed.");
  tt->val_offset = (char *) &(pp.archive_fmq_compress) - (char *) &pp;
  tt->single_val.b = pFALSE;
  tt++;
  
  /* Parameter 'print_max_min_byte' */
  /* ctype is 'tdrp_bool_t' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = BOOL_TYPE;
  tt->param_name = tdrpStrDup("print_max_min_byte");
  tt->descr = tdrpStrDup("Compute max and min byte value per beam.");
  tt->help = tdrpStrDup("If TRUE, max and min byte value will be computed for each beam.  Values will be printed with summary.");
  tt->val_offset = (char *) &(pp.print_max_min_byte) - (char *) &pp;
  tt->single_val.b = pFALSE;
  tt++;
  
  /* trailing entry has param_name set to NULL */
  
  tt->param_name = NULL;
  
  return (Table);

}