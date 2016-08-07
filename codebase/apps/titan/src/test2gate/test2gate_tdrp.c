/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
/* ** Copyright UCAR (c) 1990 - 2016                                         */
/* ** University Corporation for Atmospheric Research (UCAR)                 */
/* ** National Center for Atmospheric Research (NCAR)                        */
/* ** Boulder, Colorado, USA                                                 */
/* ** BSD licence applies - redistribution and use in source and binary      */
/* ** forms, with or without modification, are permitted provided that       */
/* ** the following conditions are met:                                      */
/* ** 1) If the software is modified to produce derivative works,            */
/* ** such modified software should be clearly marked, so as not             */
/* ** to confuse it with the version available from UCAR.                    */
/* ** 2) Redistributions of source code must retain the above copyright      */
/* ** notice, this list of conditions and the following disclaimer.          */
/* ** 3) Redistributions in binary form must reproduce the above copyright   */
/* ** notice, this list of conditions and the following disclaimer in the    */
/* ** documentation and/or other materials provided with the distribution.   */
/* ** 4) Neither the name of UCAR nor the names of its contributors,         */
/* ** if any, may be used to endorse or promote products derived from        */
/* ** this software without specific prior written permission.               */
/* ** DISCLAIMER: THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS  */
/* ** OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED      */
/* ** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.    */
/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
/*******************************************
 * test2gate_tdrp.c
 *
 * TDRP C code file 'test2gate' module.
 *
 * Code for program test2gate
 *
 * This file has been automatically
 * generated by TDRP, do not modify.
 *
 *******************************************/

#include "test2gate_tdrp.h"
#include <string.h>

/*
 * file scope variables
 */

static TDRPtable Table[22];
static test2gate_tdrp_struct *Params;
static char *Module = "test2gate";

/*************************************************************
 * test2gate_tdrp_load_from_args()
 *
 * Loads up TDRP using the command line args.
 *
 * Check TDRP_usage() for command line actions associated with
 * this function.
 *
 *   argc, argv: command line args
 *
 *   test2gate_tdrp_struct *params: loads up this struct
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

int test2gate_tdrp_load_from_args(int argc, char **argv,
                         test2gate_tdrp_struct *params,
                         char **override_list,
                         char **params_path_p)
{
  Params = params;
  test2gate_tdrp_init(Params);
  if (tdrpLoadFromArgs(argc, argv,
                       Table, Params,
                       override_list, params_path_p)) {
    return (-1);
  } else {
    return (0);
  }
}

/*************************************************************
 * test2gate_tdrp_load()
 *
 * Loads up TDRP for a given module.
 *
 * This version of load gives the programmer the option to load
 * up more than one module for a single application. It is a
 * lower-level routine than test2gate_tdrp_load_from_args,
 * and hence more flexible, but the programmer must do more work.
 *
 *   char *param_file_path: the parameter file to be read in.
 *
 *   test2gate_tdrp_struct *params: loads up this struct
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

int test2gate_tdrp_load(char *param_file_path,
               test2gate_tdrp_struct *params,
               char **override_list,
               int expand_env, int debug)
{
  Params = params;
  test2gate_tdrp_init(Params);
  if (tdrpLoad(param_file_path, Table,
               params, override_list,
               expand_env, debug)) {
    return (-1);
  } else {
    return (0);
  }
}

/*************************************************************
 * test2gate_tdrp_load_defaults()
 *
 * Loads up defaults for a given module.
 *
 * See test2gate_tdrp_load() for more details.
 *
 *  Returns 0 on success, -1 on failure.
 */

int test2gate_tdrp_load_defaults(test2gate_tdrp_struct *params,
                        int expand_env)
{
  Params = params;
  test2gate_tdrp_init(Params);
  if (tdrpLoad(NULL, Table,
               params, NULL,
               expand_env, FALSE)) {
    return (-1);
  } else {
    return (0);
  }
}

/*************************************************************
 * test2gate_tdrp_sync()
 *
 * Syncs the user struct data back into the parameter table,
 * in preparation for printing.
 */

void test2gate_tdrp_sync(void)
{
  tdrpUser2Table(Table, Params);
}

/*************************************************************
 * test2gate_tdrp_print()
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

void test2gate_tdrp_print(FILE *out, tdrp_print_mode_t mode)
{
  tdrpPrint(out, Table, Module, mode);
}

/*************************************************************
 * test2gate_tdrp_check_all_set()
 *
 * Return TRUE if all set, FALSE if not.
 *
 * If out is non-NULL, prints out warning messages for those
 * parameters which are not set.
 */

int test2gate_tdrp_check_all_set(FILE *out)
{
  return (tdrpCheckAllSet(out, Table, Params));
}

/*************************************************************
 * test2gate_tdrp_check_is_set()
 *
 * Return TRUE if parameter is set, FALSE if not.
 *
 */

int test2gate_tdrp_check_is_set(char *param_name)
{
  return (tdrpCheckIsSet(param_name, Table, Params));
}

/*************************************************************
 * test2gate_tdrp_free_all()
 *
 * Frees up all TDRP dynamic memory.
 */

void test2gate_tdrp_free_all(void)
{
  tdrpFreeAll(Table, Params);
}

/*************************************************************
 * test2gate_tdrp_array_realloc()
 *
 * Realloc 1D array.
 *
 * If size is increased, the values from the last array entry is
 * copied into the new space.
 *
 * Returns 0 on success, -1 on error.
 */

int test2gate_tdrp_array_realloc(char *param_name, int new_array_n)
{
  if (tdrpArrayRealloc(Table, Params, param_name, new_array_n)) {
    return (-1);
  } else {
    return (0);
  }
}

/*************************************************************
 * test2gate_tdrp_array2D_realloc()
 *
 * Realloc 2D array.
 *
 * If size is increased, the values from the last array entry is
 * copied into the new space.
 *
 * Returns 0 on success, -1 on error.
 */

int test2gate_tdrp_array2D_realloc(char *param_name,
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
 * test2gate_tdrp_table()
 *
 * Returns pointer to static Table for this module.
 */

TDRPtable *test2gate_tdrp_table(void)
{
  return (Table);
}

/*************************************************************
 * test2gate_tdrp_init()
 *
 * Module table initialization function.
 *
 *
 * Returns pointer to static Table for this module.
 */

TDRPtable *test2gate_tdrp_init(test2gate_tdrp_struct *params)

{

  TDRPtable *tt = Table;

  test2gate_tdrp_struct pp; /* for computing byte_offsets */

  /* zero out struct, and store size */

  memset(params, 0, sizeof(test2gate_tdrp_struct));
  params->struct_size = sizeof(test2gate_tdrp_struct);

  /* Parameter 'debug' */
  /* ctype is 'tdrp_bool_t' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = BOOL_TYPE;
  tt->param_name = tdrpStrDup("debug");
  tt->descr = tdrpStrDup("debug flag");
  tt->help = tdrpStrDup("TRUE if running in debug mode; FALSE otherwise.");
  tt->val_offset = (char *) &(pp.debug) - (char *) &pp;
  tt->single_val.b = pTRUE;
  tt++;
  
  /* Parameter 'malloc_debug_level' */
  /* ctype is 'long' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = LONG_TYPE;
  tt->param_name = tdrpStrDup("malloc_debug_level");
  tt->descr = tdrpStrDup("malloc debug level");
  tt->help = tdrpStrDup("0 - none, 1 - corruption checking, 2 - records all malloc blocks and checks, 3 - printout of all mallocs etc.");
  tt->val_offset = (char *) &(pp.malloc_debug_level) - (char *) &pp;
  tt->has_min = TRUE;
  tt->has_max = TRUE;
  tt->min_val.l = 0;
  tt->max_val.l = 3;
  tt->single_val.l = 0;
  tt++;
  
  /* Parameter 'instance' */
  /* ctype is 'char*' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = STRING_TYPE;
  tt->param_name = tdrpStrDup("instance");
  tt->descr = tdrpStrDup("Process instance");
  tt->help = tdrpStrDup("Used for registration with procmap.");
  tt->val_offset = (char *) &(pp.instance) - (char *) &pp;
  tt->single_val.s = tdrpStrDup("test");
  tt++;
  
  /* Parameter 'output_port' */
  /* ctype is 'long' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = LONG_TYPE;
  tt->param_name = tdrpStrDup("output_port");
  tt->descr = tdrpStrDup("output port");
  tt->help = tdrpStrDup("Port used for sending radar gate data to clients.");
  tt->val_offset = (char *) &(pp.output_port) - (char *) &pp;
  tt->single_val.l = 60000;
  tt++;
  
  /* Parameter 'output_vel_field' */
  /* ctype is 'tdrp_bool_t' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = BOOL_TYPE;
  tt->param_name = tdrpStrDup("output_vel_field");
  tt->descr = tdrpStrDup("Option to output veloocity field");
  tt->help = tdrpStrDup("If FALSE, only dBZ is output. If TRUE, velocity is also output.");
  tt->val_offset = (char *) &(pp.output_vel_field) - (char *) &pp;
  tt->single_val.b = pTRUE;
  tt++;
  
  /* Parameter 'output_geom_fields' */
  /* ctype is 'tdrp_bool_t' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = BOOL_TYPE;
  tt->param_name = tdrpStrDup("output_geom_fields");
  tt->descr = tdrpStrDup("Option to output geometry fields");
  tt->help = tdrpStrDup("If TRUE, elevation, azimuth and range fields are also output.");
  tt->val_offset = (char *) &(pp.output_geom_fields) - (char *) &pp;
  tt->single_val.b = pFALSE;
  tt++;
  
  /* Parameter 'radar_params' */
  /* ctype is 'test2gate_radar_params' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = STRUCT_TYPE;
  tt->param_name = tdrpStrDup("radar_params");
  tt->descr = tdrpStrDup("radar parameters");
  tt->help = tdrpStrDup("Default radar parameters.");
  tt->val_offset = (char *) &(pp.radar_params) - (char *) &pp;
  tt->struct_def.name = tdrpStrDup("radar_params");
  tt->struct_def.nfields = 12;
  tt->struct_def.fields = (struct_field_t *)
      tdrpMalloc(tt->struct_def.nfields * sizeof(struct_field_t));
    tt->struct_def.fields[0].ftype = tdrpStrDup("long");
    tt->struct_def.fields[0].fname = tdrpStrDup("radar_id");
    tt->struct_def.fields[0].ptype = LONG_TYPE;
    tt->struct_def.fields[0].rel_offset = 
      (char *) &(pp.radar_params.radar_id) - (char *) &(pp.radar_params);
    tt->struct_def.fields[1].ftype = tdrpStrDup("long");
    tt->struct_def.fields[1].fname = tdrpStrDup("altitude");
    tt->struct_def.fields[1].ptype = LONG_TYPE;
    tt->struct_def.fields[1].rel_offset = 
      (char *) &(pp.radar_params.altitude) - (char *) &(pp.radar_params);
    tt->struct_def.fields[2].ftype = tdrpStrDup("double");
    tt->struct_def.fields[2].fname = tdrpStrDup("latitude");
    tt->struct_def.fields[2].ptype = DOUBLE_TYPE;
    tt->struct_def.fields[2].rel_offset = 
      (char *) &(pp.radar_params.latitude) - (char *) &(pp.radar_params);
    tt->struct_def.fields[3].ftype = tdrpStrDup("double");
    tt->struct_def.fields[3].fname = tdrpStrDup("longitude");
    tt->struct_def.fields[3].ptype = DOUBLE_TYPE;
    tt->struct_def.fields[3].rel_offset = 
      (char *) &(pp.radar_params.longitude) - (char *) &(pp.radar_params);
    tt->struct_def.fields[4].ftype = tdrpStrDup("long");
    tt->struct_def.fields[4].fname = tdrpStrDup("num_gates");
    tt->struct_def.fields[4].ptype = LONG_TYPE;
    tt->struct_def.fields[4].rel_offset = 
      (char *) &(pp.radar_params.num_gates) - (char *) &(pp.radar_params);
    tt->struct_def.fields[5].ftype = tdrpStrDup("double");
    tt->struct_def.fields[5].fname = tdrpStrDup("gate_spacing");
    tt->struct_def.fields[5].ptype = DOUBLE_TYPE;
    tt->struct_def.fields[5].rel_offset = 
      (char *) &(pp.radar_params.gate_spacing) - (char *) &(pp.radar_params);
    tt->struct_def.fields[6].ftype = tdrpStrDup("double");
    tt->struct_def.fields[6].fname = tdrpStrDup("start_range");
    tt->struct_def.fields[6].ptype = DOUBLE_TYPE;
    tt->struct_def.fields[6].rel_offset = 
      (char *) &(pp.radar_params.start_range) - (char *) &(pp.radar_params);
    tt->struct_def.fields[7].ftype = tdrpStrDup("double");
    tt->struct_def.fields[7].fname = tdrpStrDup("beam_width");
    tt->struct_def.fields[7].ptype = DOUBLE_TYPE;
    tt->struct_def.fields[7].rel_offset = 
      (char *) &(pp.radar_params.beam_width) - (char *) &(pp.radar_params);
    tt->struct_def.fields[8].ftype = tdrpStrDup("long");
    tt->struct_def.fields[8].fname = tdrpStrDup("samples_per_beam");
    tt->struct_def.fields[8].ptype = LONG_TYPE;
    tt->struct_def.fields[8].rel_offset = 
      (char *) &(pp.radar_params.samples_per_beam) - (char *) &(pp.radar_params);
    tt->struct_def.fields[9].ftype = tdrpStrDup("double");
    tt->struct_def.fields[9].fname = tdrpStrDup("pulse_width");
    tt->struct_def.fields[9].ptype = DOUBLE_TYPE;
    tt->struct_def.fields[9].rel_offset = 
      (char *) &(pp.radar_params.pulse_width) - (char *) &(pp.radar_params);
    tt->struct_def.fields[10].ftype = tdrpStrDup("double");
    tt->struct_def.fields[10].fname = tdrpStrDup("prf");
    tt->struct_def.fields[10].ptype = DOUBLE_TYPE;
    tt->struct_def.fields[10].rel_offset = 
      (char *) &(pp.radar_params.prf) - (char *) &(pp.radar_params);
    tt->struct_def.fields[11].ftype = tdrpStrDup("double");
    tt->struct_def.fields[11].fname = tdrpStrDup("wavelength");
    tt->struct_def.fields[11].ptype = DOUBLE_TYPE;
    tt->struct_def.fields[11].rel_offset = 
      (char *) &(pp.radar_params.wavelength) - (char *) &(pp.radar_params);
  tt->n_struct_vals = 12;
  tt->struct_vals = (tdrpVal_t *)
      tdrpMalloc(tt->n_struct_vals * sizeof(tdrpVal_t));
    tt->struct_vals[0].l = 0;
    tt->struct_vals[1].l = 0;
    tt->struct_vals[2].d = 39.8782;
    tt->struct_vals[3].d = -104.759;
    tt->struct_vals[4].l = 65;
    tt->struct_vals[5].d = 900;
    tt->struct_vals[6].d = 700;
    tt->struct_vals[7].d = 5;
    tt->struct_vals[8].l = 32;
    tt->struct_vals[9].d = 6000;
    tt->struct_vals[10].d = 1000;
    tt->struct_vals[11].d = 5;
  tt++;
  
  /* Parameter 'use_scan_table' */
  /* ctype is 'tdrp_bool_t' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = BOOL_TYPE;
  tt->param_name = tdrpStrDup("use_scan_table");
  tt->descr = tdrpStrDup("Use scan table");
  tt->help = tdrpStrDup("If set, use scan azimuth table. If not, construct table from elevation angles, start azimuth and delta azimuth.");
  tt->val_offset = (char *) &(pp.use_scan_table) - (char *) &pp;
  tt->single_val.b = pTRUE;
  tt++;
  
  /* Parameter 'scan_table_path' */
  /* ctype is 'char*' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = STRING_TYPE;
  tt->param_name = tdrpStrDup("scan_table_path");
  tt->descr = tdrpStrDup("scan table path");
  tt->help = tdrpStrDup("");
  tt->val_offset = (char *) &(pp.scan_table_path) - (char *) &pp;
  tt->single_val.s = tdrpStrDup("./tass_scan_table");
  tt++;
  
  /* Parameter 'noise_dbz_at_100km' */
  /* ctype is 'double' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = DOUBLE_TYPE;
  tt->param_name = tdrpStrDup("noise_dbz_at_100km");
  tt->descr = tdrpStrDup("Noise dBZ level at 100 km range");
  tt->help = tdrpStrDup("Used to compute noise reflectivity at all ranges.");
  tt->val_offset = (char *) &(pp.noise_dbz_at_100km) - (char *) &pp;
  tt->single_val.d = 0;
  tt++;
  
  /* Parameter 'elev_angles' */
  /* ctype is 'double' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = DOUBLE_TYPE;
  tt->param_name = tdrpStrDup("elev_angles");
  tt->descr = tdrpStrDup("Elevation angle list");
  tt->help = tdrpStrDup("If use_scan_table is false, these elevation angles are used to construct the scan table.");
  tt->val_offset = (char *) &(pp.elev_angles.val) - (char *) &pp;
  tt->len_offset = (char *) &(pp.elev_angles.len) - (char *) &pp;
  tt->array_offset = (char *) &(pp._elev_angles) - (char *) &pp;
  tt->array_n_offset = (char *) &(pp.elev_angles_n) - (char *) &pp;
  tt->is_array = TRUE;
  tt->array_len_fixed = FALSE;
  tt->array_elem_size = sizeof(double);
  tt->array_n = 1;
  tt->array_vals = (tdrpVal_t *)
      tdrpMalloc(tt->array_n * sizeof(tdrpVal_t));
    tt->array_vals[0].d = 1;
  tt++;
  
  /* Parameter 'nazimuths' */
  /* ctype is 'long' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = LONG_TYPE;
  tt->param_name = tdrpStrDup("nazimuths");
  tt->descr = tdrpStrDup("Number of aziimuths");
  tt->help = tdrpStrDup("If use_scan_table is false, this is used to construct the scan table.");
  tt->val_offset = (char *) &(pp.nazimuths) - (char *) &pp;
  tt->single_val.l = 360;
  tt++;
  
  /* Parameter 'start_azimuth' */
  /* ctype is 'double' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = DOUBLE_TYPE;
  tt->param_name = tdrpStrDup("start_azimuth");
  tt->descr = tdrpStrDup("Start azimuth angle");
  tt->help = tdrpStrDup("If use_scan_table is false, this is used to construct the scan table.");
  tt->val_offset = (char *) &(pp.start_azimuth) - (char *) &pp;
  tt->single_val.d = 0;
  tt++;
  
  /* Parameter 'delta_azimuth' */
  /* ctype is 'double' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = DOUBLE_TYPE;
  tt->param_name = tdrpStrDup("delta_azimuth");
  tt->descr = tdrpStrDup("Delta azimuth angle");
  tt->help = tdrpStrDup("If use_scan_table is false, this is used to construct the scan table.");
  tt->val_offset = (char *) &(pp.delta_azimuth) - (char *) &pp;
  tt->single_val.d = 1;
  tt++;
  
  /* Parameter 'radar_sample_file_path' */
  /* ctype is 'char*' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = STRING_TYPE;
  tt->param_name = tdrpStrDup("radar_sample_file_path");
  tt->descr = tdrpStrDup("Radar sample file path");
  tt->help = tdrpStrDup("Path for file from which radar data are sampled");
  tt->val_offset = (char *) &(pp.radar_sample_file_path) - (char *) &pp;
  tt->single_val.s = tdrpStrDup("null");
  tt++;
  
  /* Parameter 'sample_dbz_field' */
  /* ctype is 'long' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = LONG_TYPE;
  tt->param_name = tdrpStrDup("sample_dbz_field");
  tt->descr = tdrpStrDup("Sample dbz field num");
  tt->help = tdrpStrDup("");
  tt->val_offset = (char *) &(pp.sample_dbz_field) - (char *) &pp;
  tt->single_val.l = 0;
  tt++;
  
  /* Parameter 'sample_vel_field' */
  /* ctype is 'long' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = LONG_TYPE;
  tt->param_name = tdrpStrDup("sample_vel_field");
  tt->descr = tdrpStrDup("Sample vel field num");
  tt->help = tdrpStrDup("");
  tt->val_offset = (char *) &(pp.sample_vel_field) - (char *) &pp;
  tt->single_val.l = 0;
  tt++;
  
  /* Parameter 'override_vel' */
  /* ctype is 'tdrp_bool_t' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = BOOL_TYPE;
  tt->param_name = tdrpStrDup("override_vel");
  tt->descr = tdrpStrDup("Override velocity with origin movement.");
  tt->help = tdrpStrDup("If TRUE, the velocity field will be set using the motion of the sampling origin.");
  tt->val_offset = (char *) &(pp.override_vel) - (char *) &pp;
  tt->single_val.b = pFALSE;
  tt++;
  
  /* Parameter 'sampling_origin' */
  /* ctype is 'test2gate_sampling_origin' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = STRUCT_TYPE;
  tt->param_name = tdrpStrDup("sampling_origin");
  tt->descr = tdrpStrDup("Radar data sampling origin");
  tt->help = tdrpStrDup("Origin from which the dobson radar data are sampled.");
  tt->val_offset = (char *) &(pp.sampling_origin) - (char *) &pp;
  tt->struct_def.name = tdrpStrDup("sampling_origin");
  tt->struct_def.nfields = 5;
  tt->struct_def.fields = (struct_field_t *)
      tdrpMalloc(tt->struct_def.nfields * sizeof(struct_field_t));
    tt->struct_def.fields[0].ftype = tdrpStrDup("double");
    tt->struct_def.fields[0].fname = tdrpStrDup("start_x");
    tt->struct_def.fields[0].ptype = DOUBLE_TYPE;
    tt->struct_def.fields[0].rel_offset = 
      (char *) &(pp.sampling_origin.start_x) - (char *) &(pp.sampling_origin);
    tt->struct_def.fields[1].ftype = tdrpStrDup("double");
    tt->struct_def.fields[1].fname = tdrpStrDup("start_y");
    tt->struct_def.fields[1].ptype = DOUBLE_TYPE;
    tt->struct_def.fields[1].rel_offset = 
      (char *) &(pp.sampling_origin.start_y) - (char *) &(pp.sampling_origin);
    tt->struct_def.fields[2].ftype = tdrpStrDup("double");
    tt->struct_def.fields[2].fname = tdrpStrDup("speed");
    tt->struct_def.fields[2].ptype = DOUBLE_TYPE;
    tt->struct_def.fields[2].rel_offset = 
      (char *) &(pp.sampling_origin.speed) - (char *) &(pp.sampling_origin);
    tt->struct_def.fields[3].ftype = tdrpStrDup("double");
    tt->struct_def.fields[3].fname = tdrpStrDup("dirn");
    tt->struct_def.fields[3].ptype = DOUBLE_TYPE;
    tt->struct_def.fields[3].rel_offset = 
      (char *) &(pp.sampling_origin.dirn) - (char *) &(pp.sampling_origin);
    tt->struct_def.fields[4].ftype = tdrpStrDup("double");
    tt->struct_def.fields[4].fname = tdrpStrDup("max_dist");
    tt->struct_def.fields[4].ptype = DOUBLE_TYPE;
    tt->struct_def.fields[4].rel_offset = 
      (char *) &(pp.sampling_origin.max_dist) - (char *) &(pp.sampling_origin);
  tt->n_struct_vals = 5;
  tt->struct_vals = (tdrpVal_t *)
      tdrpMalloc(tt->n_struct_vals * sizeof(tdrpVal_t));
    tt->struct_vals[0].d = 0;
    tt->struct_vals[1].d = 0;
    tt->struct_vals[2].d = 0;
    tt->struct_vals[3].d = 0;
    tt->struct_vals[4].d = 200;
  tt++;
  
  /* Parameter 'beam_wait_msecs' */
  /* ctype is 'long' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = LONG_TYPE;
  tt->param_name = tdrpStrDup("beam_wait_msecs");
  tt->descr = tdrpStrDup("Wait per beam (milli-secs)");
  tt->help = tdrpStrDup("");
  tt->val_offset = (char *) &(pp.beam_wait_msecs) - (char *) &pp;
  tt->single_val.l = 50;
  tt++;
  
  /* Parameter 'vol_wait_secs' */
  /* ctype is 'long' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = LONG_TYPE;
  tt->param_name = tdrpStrDup("vol_wait_secs");
  tt->descr = tdrpStrDup("Wait per vol (secs)");
  tt->help = tdrpStrDup("");
  tt->val_offset = (char *) &(pp.vol_wait_secs) - (char *) &pp;
  tt->single_val.l = 30;
  tt++;
  
  /* trailing entry has param_name set to NULL */
  
  tt->param_name = NULL;
  
  return (Table);

}