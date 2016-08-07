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
 * uf2mdv_tdrp.h
 *
 * TDRP header file for 'uf2mdv' module.
 *
 * This header file has been automatically
 * generated by TDRP, do not modify.
 *
 *******************************************/

#ifndef _uf2mdv_tdrp_h
#define _uf2mdv_tdrp_h

#ifdef __cplusplus
extern "C" {
#endif

#include <tdrp/tdrp.h>

/*
 * typedefs
 */

typedef enum {
  FILTER_NONE = 0,
  FILTER_NEGATIVE = 1
} uf2mdv_input_filter;

typedef enum {
  BROWSE = 0,
  ANALYZE_DATA_RANGE = 1,
  ANALYZE = 2,
  ANALYZE_BUILD_OUTPUT = 3
} uf2mdv_processing;

typedef enum {
  BROWSE_NONE = 0,
  BROWSE_1LINE = 1,
  BROWSE_MULTILINE = 2,
  BROWSE_MULTILINE_AND_DATA = 3
} uf2mdv_browse_level;

typedef enum {
  ANALYZE_FEEDBACK_NONE = 0,
  ANALYZE_FEEDBACK_FINAL = 1,
  ANALYZE_FEEDBACK_INIT_FINAL = 2,
  ANALYZE_FEEDBACK_MEGA = 3
} uf2mdv_analyze_feedback;

typedef enum {
  UF_FORTRAN_BINARY = 0,
  UF_PURE_DATA = 1
} uf2mdv_input_data_format;

typedef struct {
  char* input_name;
  uf2mdv_input_filter filter;
  double scale;
  double bias;
  char* output_name;
} uf2mdv_data_types_t;

typedef struct {
  double az0;
  double delta_az;
  long naz;
  double gt0;
  double delta_gt;
  long ngt;
  double elev0;
  double delta_elev;
  long nelev;
} uf2mdv_grid_params;

/*
 * typedef for main struct - uf2mdv_tdrp_struct
 */

typedef struct {

  size_t struct_size;

  /***** check_params *****/

  tdrp_bool_t check_params;

  /***** print_params *****/

  tdrp_bool_t print_params;

  /***** debug_level *****/

  long debug_level;

  /***** processing *****/

  uf2mdv_processing processing;

  /***** input_file *****/

  struct {
    char* *val;
    int len;
  } input_file;
  char* *_input_file;
  int input_file_n;

  /***** browse_level *****/

  uf2mdv_browse_level browse_level;

  /***** analyze_feedback *****/

  uf2mdv_analyze_feedback analyze_feedback;

  /***** input_data_format *****/

  uf2mdv_input_data_format input_data_format;

  /***** byte_swap *****/

  tdrp_bool_t byte_swap;

  /***** sticky_azimuths *****/

  tdrp_bool_t sticky_azimuths;

  /***** elevation_wobble_max *****/

  double elevation_wobble_max;

  /***** min_prf *****/

  double min_prf;

  /***** max_prf *****/

  double max_prf;

  /***** data_types *****/

  struct {
    uf2mdv_data_types_t *val;
    int len;
  } data_types;
  uf2mdv_data_types_t *_data_types;
  int data_types_n;

  /***** year_convert *****/

  long year_convert;

  /***** start_time *****/

  struct {
    long *val;
    int len;
  } start_time;
  long *_start_time;
  int start_time_n;

  /***** stop_time *****/

  struct {
    long *val;
    int len;
  } stop_time;
  long *_stop_time;
  int stop_time_n;

  /***** filter_min_beams *****/

  tdrp_bool_t filter_min_beams;

  /***** filter_redundant_elev_based_on_size *****/

  tdrp_bool_t filter_redundant_elev_based_on_size;

  /***** min_beams_per_scan *****/

  long min_beams_per_scan;

  /***** grid_params *****/

  uf2mdv_grid_params grid_params;

  /***** use_vertical_levels *****/

  tdrp_bool_t use_vertical_levels;

  /***** vertical_levels *****/

  struct {
    double *val;
    int len;
  } vertical_levels;
  double *_vertical_levels;
  int vertical_levels_n;

  /***** output_file_suffix *****/

  char* output_file_suffix;

  /***** output_dir *****/

  char* output_dir;

  /***** output_encoding_type *****/

  long output_encoding_type;

  /***** override_sensor_location *****/

  tdrp_bool_t override_sensor_location;

  /***** sensor_latitude *****/

  double sensor_latitude;

  /***** sensor_longitude *****/

  double sensor_longitude;

  /***** sensor_altitude *****/

  double sensor_altitude;

} uf2mdv_tdrp_struct;

/*
 * function prototypes
 */

/*************************************************************
 * uf2mdv_tdrp_load_from_args()
 *
 * Loads up TDRP using the command line args.
 *
 * Check TDRP_usage() for command line actions associated with
 * this function.
 *
 *   argc, argv: command line args
 *
 *   uf2mdv_tdrp_struct *params: loads up this struct
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

extern int uf2mdv_tdrp_load_from_args(int argc, char **argv,
                                uf2mdv_tdrp_struct *params,
                                char **override_list,
                                char **params_path_p);

/*************************************************************
 * uf2mdv_tdrp_load()
 *
 * Loads up TDRP for a given module.
 *
 * This version of load gives the programmer the option to load
 * up more than one module for a single application. It is a
 * lower-level routine than uf2mdv_tdrp_load_from_args,
 * and hence more flexible, but the programmer must do more work.
 *
 *   char *param_file_path: the parameter file to be read in.
 *
 *   uf2mdv_tdrp_struct *params: loads up this struct
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

extern int uf2mdv_tdrp_load(char *param_file_path,
                      uf2mdv_tdrp_struct *params,
                      char **override_list,
                      int expand_env, int debug);

/*************************************************************
 * uf2mdv_tdrp_load_defaults()
 *
 * Loads up defaults for a given module.
 *
 * See uf2mdv_tdrp_load() for more details.
 *
 *  Returns 0 on success, -1 on failure.
 */

extern int uf2mdv_tdrp_load_defaults(uf2mdv_tdrp_struct *params,
                               int expand_env);

/*************************************************************
 * uf2mdv_tdrp_sync()
 *
 * Syncs the user struct data back into the parameter table,
 * in preparation for printing.
 */

extern void uf2mdv_tdrp_sync(void);

/*************************************************************
 * uf2mdv_tdrp_print()
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

extern void uf2mdv_tdrp_print(FILE *out, tdrp_print_mode_t mode);

/*************************************************************
 * uf2mdv_tdrp_free_all()
 *
 * Frees up all TDRP dynamic memory.
 */

extern void uf2mdv_tdrp_free_all(void);

/*************************************************************
 * uf2mdv_tdrp_check_all_set()
 *
 * Return TRUE if all set, FALSE if not.
 *
 * If out is non-NULL, prints out warning messages for those
 * parameters which are not set.
 */

extern int uf2mdv_tdrp_check_all_set(FILE *out);

/*************************************************************
 * uf2mdv_tdrp_check_is_set()
 *
 * Return TRUE if parameter is set, FALSE if not.
 *
 */

extern int uf2mdv_tdrp_check_is_set(char *param_name);

/*************************************************************
 * uf2mdv_tdrp_array_realloc()
 *
 * Realloc 1D array.
 *
 * If size is increased, the values from the last array entry is
 * copied into the new space.
 *
 * Returns 0 on success, -1 on error.
 */

extern int uf2mdv_tdrp_array_realloc(char *param_name,
                              int new_array_n);

/*************************************************************
 * uf2mdv_tdrp_array2D_realloc()
 *
 * Realloc 2D array.
 *
 * If size is increased, the values from the last array entry is
 * copied into the new space.
 *
 * Returns 0 on success, -1 on error.
 */

extern int uf2mdv_tdrp_array2D_realloc(char *param_name,
                                 int new_array_n1,
                                 int new_array_n2);

/*************************************************************
 * uf2mdv_tdrp_table()
 *
 * Returns pointer to static Table for this module.
 */

extern TDRPtable *uf2mdv_tdrp_table(void);

/*************************************************************
 * uf2mdv_tdrp_init()
 *
 * Module table initialization function.
 *
 *
 * Returns pointer to static Table for this module.
 */

extern TDRPtable *uf2mdv_tdrp_init(uf2mdv_tdrp_struct *params);

#ifdef __cplusplus
}
#endif

#endif
