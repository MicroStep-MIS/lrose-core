// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
// ** Copyright UCAR (c) 1992 - 2017
// ** University Corporation for Atmospheric Research(UCAR)
// ** National Center for Atmospheric Research(NCAR)
// ** Boulder, Colorado, USA
// *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
////////////////////////////////////////////
// Params.hh
//
// TDRP header file for 'Params' class.
//
// Code for program Radx2Grid
//
// This header file has been automatically
// generated by TDRP, do not modify.
//
/////////////////////////////////////////////

/**
 *
 * @file Params.hh
 *
 * This class is automatically generated by the Table
 * Driven Runtime Parameters (TDRP) system
 *
 * @class Params
 *
 * @author automatically generated
 *
 */

#ifndef Params_hh
#define Params_hh

using namespace std;

#include <tdrp/tdrp.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <cfloat>

// Class definition

class Params {

public:

  // enum typedefs

  typedef enum {
    DEBUG_OFF = 0,
    DEBUG_NORM = 1,
    DEBUG_VERBOSE = 2,
    DEBUG_EXTRA = 3
  } debug_t;

  typedef enum {
    FILELIST = 0,
    ARCHIVE = 1,
    REALTIME = 2
  } mode_t;

  typedef enum {
    CF_NETCDF = 0,
    ZEBRA_NETCDF = 1,
    MDV = 2,
    CEDRIC = 3
  } output_format_t;

  typedef enum {
    INTERP_MODE_CART = 0,
    INTERP_MODE_PPI = 1,
    INTERP_MODE_POLAR = 2,
    INTERP_MODE_CART_REORDER = 3,
    INTERP_MODE_CART_SAT = 4
  } interp_mode_t;

  typedef enum {
    PROJ_LATLON = 0,
    PROJ_LAMBERT_CONF = 3,
    PROJ_MERCATOR = 4,
    PROJ_POLAR_STEREO = 5,
    PROJ_FLAT = 8,
    PROJ_OBLIQUE_STEREO = 12,
    PROJ_TRANS_MERCATOR = 15,
    PROJ_ALBERS = 16,
    PROJ_LAMBERT_AZIM = 17,
    PROJ_VERT_PERSP = 18
  } projection_t;

  typedef enum {
    TRANSFORM_DB_TO_LINEAR = 0,
    TRANSFORM_DB_TO_LINEAR_AND_BACK = 1,
    TRANSFORM_LINEAR_TO_DB = 2,
    TRANSFORM_LINEAR_TO_DB_AND_BACK = 3
  } interp_transform_t;

  typedef enum {
    LOGICAL_AND = 0,
    LOGICAL_OR = 1
  } logical_t;

  typedef enum {
    CLASSIC = 0,
    NC64BIT = 1,
    NETCDF4_CLASSIC = 2,
    NETCDF4 = 3
  } netcdf_style_t;

  // struct typedefs

  typedef struct {
    int nz;
    double minz;
    double dz;
  } grid_z_geom_t;

  typedef struct {
    int nx;
    int ny;
    double minx;
    double miny;
    double dx;
    double dy;
  } grid_xy_geom_t;

  typedef struct {
    char* input_name;
    tdrp_bool_t process_this_field;
  } select_field_t;

  typedef struct {
    char* input_name;
    char* output_name;
    char* output_units;
    interp_transform_t transform;
  } transform_field_t;

  typedef struct {
    char* input_name;
    tdrp_bool_t field_folds;
    tdrp_bool_t use_global_nyquist;
    double fold_limit_lower;
    double fold_limit_upper;
  } fold_field_t;

  typedef struct {
    char* input_name;
    tdrp_bool_t is_discrete;
  } discrete_field_t;

  typedef struct {
    char* input_name;
    char* output_name;
  } rename_field_t;

  typedef struct {
    char* name;
    double min_valid_value;
    double max_valid_value;
    logical_t combination_method;
  } censoring_field_t;

  typedef struct {
    char* input_name;
    double min_value;
    double max_value;
  } bound_field_t;

  ///////////////////////////
  // Member functions
  //

  ////////////////////////////////////////////
  // Default constructor
  //

  Params ();

  ////////////////////////////////////////////
  // Copy constructor
  //

  Params (const Params&);

  ////////////////////////////////////////////
  // Destructor
  //

  ~Params ();

  ////////////////////////////////////////////
  // Assignment
  //

  void operator=(const Params&);

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

  int loadFromArgs(int argc, char **argv,
                   char **override_list,
                   char **params_path_p,
                   bool defer_exit = false);

  bool exitDeferred() { return (_exitDeferred); }

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

  int loadApplyArgs(const char *params_path,
                    int argc, char **argv,
                    char **override_list,
                    bool defer_exit = false);

  ////////////////////////////////////////////
  // isArgValid()
  // 
  // Check if a command line arg is a valid TDRP arg.
  //

  static bool isArgValid(const char *arg);

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

  int load(const char *param_file_path,
           char **override_list,
           int expand_env, int debug);

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

  int loadFromBuf(const char *param_source_str,
                  char **override_list,
                  const char *inbuf, int inlen,
                  int start_line_num,
                  int expand_env, int debug);

  ////////////////////////////////////////////
  // loadDefaults()
  //
  // Loads up default params for a given class.
  //
  // See load() for more detailed info.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int loadDefaults(int expand_env);

  ////////////////////////////////////////////
  // sync()
  //
  // Syncs the user struct data back into the parameter table,
  // in preparation for printing.
  //
  // This function alters the table in a consistent manner.
  // Therefore it can be regarded as const.
  //

  void sync() const;

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

  void print(FILE *out, tdrp_print_mode_t mode = PRINT_NORM);

  ////////////////////////////////////////////
  // checkAllSet()
  //
  // Return TRUE if all set, FALSE if not.
  //
  // If out is non-NULL, prints out warning messages for those
  // parameters which are not set.
  //

  int checkAllSet(FILE *out);

  //////////////////////////////////////////////////////////////
  // checkIsSet()
  //
  // Return TRUE if parameter is set, FALSE if not.
  //
  //

  int checkIsSet(const char *param_name);

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

  int arrayRealloc(const char *param_name,
                   int new_array_n);

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

  int array2DRealloc(const char *param_name,
                     int new_array_n1,
                     int new_array_n2);

  ////////////////////////////////////////////
  // freeAll()
  //
  // Frees up all TDRP dynamic memory.
  //

  void freeAll(void);

  ////////////////////////////////////////////
  // usage()
  //
  // Prints out usage message for TDRP args as passed
  // in to loadFromArgs().
  //

  static void usage(ostream &out);

  ///////////////////////////
  // Data Members
  //

  char _start_; // start of data region
                // needed for zeroing out data
                // and computing offsets

  debug_t debug;

  char* input_dir;

  mode_t mode;

  int max_realtime_data_age_secs;

  char* start_time;

  char* end_time;

  char* output_dir;

  output_format_t output_format;

  interp_mode_t interp_mode;

  tdrp_bool_t use_nearest_neighbor;

  int min_nvalid_for_interp;

  tdrp_bool_t use_fixed_angle_for_interpolation;

  tdrp_bool_t use_fixed_angle_for_data_limits;

  double beam_width_fraction_for_data_limit_extension;

  grid_z_geom_t grid_z_geom;

  tdrp_bool_t specify_individual_z_levels;

  double *_z_level_array;
  int z_level_array_n;

  grid_xy_geom_t grid_xy_geom;

  tdrp_bool_t center_grid_on_radar;

  projection_t grid_projection;

  double grid_rotation;

  double grid_origin_lat;

  double grid_origin_lon;

  tdrp_bool_t auto_remap_flat_to_latlon;

  double grid_lat1;

  double grid_lat2;

  double grid_central_scale;

  double grid_tangent_lat;

  double grid_tangent_lon;

  tdrp_bool_t grid_pole_is_north;

  double grid_persp_radius;

  double grid_false_northing;

  double grid_false_easting;

  tdrp_bool_t grid_set_offset_origin;

  double grid_offset_origin_latitude;

  double grid_offset_origin_longitude;

  tdrp_bool_t select_fields;

  select_field_t *_selected_fields;
  int selected_fields_n;

  tdrp_bool_t transform_fields_for_interpolation;

  transform_field_t *_transform_fields;
  int transform_fields_n;

  tdrp_bool_t set_fold_limits;

  fold_field_t *_folded_fields;
  int folded_fields_n;

  tdrp_bool_t override_nyquist;

  double nyquist_velocity;

  tdrp_bool_t set_discrete_fields;

  discrete_field_t *_discrete_fields;
  int discrete_fields_n;

  tdrp_bool_t rename_fields;

  rename_field_t *_renamed_fields;
  int renamed_fields_n;

  tdrp_bool_t output_coverage_field;

  char* coverage_field_name;

  tdrp_bool_t output_time_field;

  tdrp_bool_t interp_time_field;

  tdrp_bool_t output_test_fields;

  tdrp_bool_t interp_test_fields;

  tdrp_bool_t output_range_field;

  tdrp_bool_t interp_range_field;

  double modulus_for_elevation;

  double modulus_for_azimuth;

  double modulus_for_range;

  tdrp_bool_t apply_censoring;

  censoring_field_t *_censoring_fields;
  int censoring_fields_n;

  int censoring_min_valid_run;

  tdrp_bool_t override_standard_pseudo_earth_radius;

  double pseudo_earth_radius_ratio;

  tdrp_bool_t aggregate_sweep_files_on_read;

  tdrp_bool_t ignore_idle_scan_mode_on_read;

  tdrp_bool_t remove_rays_with_antenna_transitions;

  int transition_nrays_margin;

  tdrp_bool_t remove_long_range_rays;

  tdrp_bool_t remove_short_range_rays;

  tdrp_bool_t trim_surveillance_sweeps_to_360deg;

  tdrp_bool_t override_fixed_angle_with_mean_measured_angle;

  tdrp_bool_t reorder_sweeps_by_ascending_angle;

  tdrp_bool_t compute_sweep_angles_from_vcp_tables;

  tdrp_bool_t apply_time_offset;

  double time_offset_secs;

  tdrp_bool_t set_max_range;

  double max_range_km;

  tdrp_bool_t set_elevation_angle_limits;

  double lower_elevation_angle_limit;

  double upper_elevation_angle_limit;

  tdrp_bool_t set_azimuth_angle_limits;

  double lower_azimuth_angle_limit;

  double upper_azimuth_angle_limit;

  tdrp_bool_t check_fixed_angle_error;

  double max_fixed_angle_error;

  tdrp_bool_t check_number_of_sweeps;

  double min_number_of_sweeps;

  tdrp_bool_t override_volume_number;

  int starting_volume_number;

  tdrp_bool_t autoincrement_volume_number;

  tdrp_bool_t override_radar_location;

  double radar_latitude_deg;

  double radar_longitude_deg;

  double radar_altitude_meters;

  tdrp_bool_t override_instrument_name;

  char* instrument_name;

  tdrp_bool_t override_site_name;

  char* site_name;

  tdrp_bool_t override_beam_width;

  double beam_width_deg_h;

  double beam_width_deg_v;

  tdrp_bool_t override_gate_geometry;

  double start_range_km;

  double gate_spacing_km;

  double azimuth_correction_deg;

  double elevation_correction_deg;

  tdrp_bool_t specify_output_filename;

  char* output_filename;

  tdrp_bool_t name_file_from_start_time;

  char* netcdf_file_prefix;

  char* netcdf_file_suffix;

  tdrp_bool_t use_iso8601_filename_convention;

  tdrp_bool_t netcdf_compressed;

  int netcdf_compression_level;

  netcdf_style_t netcdf_style;

  tdrp_bool_t netcdf_include_latlon_arrays;

  tdrp_bool_t netcdf_output_mdv_attributes;

  tdrp_bool_t netcdf_output_mdv_chunks;

  char* ncf_title;

  char* ncf_institution;

  char* ncf_references;

  char* ncf_source;

  char* ncf_history;

  char* ncf_comment;

  char* instance;

  tdrp_bool_t register_with_procmap;

  int procmap_register_interval;

  tdrp_bool_t free_memory_between_files;

  tdrp_bool_t use_multiple_threads;

  int n_compute_threads;

  tdrp_bool_t sat_data_invert_in_range;

  tdrp_bool_t sat_data_set_range_geom_from_fields;

  tdrp_bool_t write_search_matrix_files;

  char* search_matrix_dir;

  tdrp_bool_t identify_convective_stratiform_split;

  char* conv_strat_dbz_field_name;

  double conv_strat_texture_radius_km;

  double conv_strat_texture_depth_km;

  double conv_strat_min_valid_fraction_for_texture;

  double conv_strat_min_valid_height;

  double conv_strat_max_valid_height;

  double conv_strat_min_valid_dbz;

  double conv_strat_dbz_threshold_for_definite_convection;

  double conv_strat_convective_radius_km;

  double conv_strat_min_texture_for_convection;

  tdrp_bool_t conv_strat_write_partition;

  tdrp_bool_t conv_strat_write_debug_fields;

  int reorder_npoints_search;

  double reorder_search_radius_km;

  tdrp_bool_t reorder_scale_search_radius_with_range;

  double reorder_nominal_range_for_search_radius_km;

  double reorder_z_search_ratio;

  tdrp_bool_t reorder_bound_grid_point_vertically;

  double reorder_min_valid_wt_ratio;

  int reorder_blocks_nrows;

  int reorder_blocks_ncols;

  int reorder_min_nvalid_for_interp;

  tdrp_bool_t reorder_weighted_interpolation;

  tdrp_bool_t bound_fields;

  bound_field_t *_bounded_fields;
  int bounded_fields_n;

  char _end_; // end of data region
              // needed for zeroing out data

private:

  void _init();

  mutable TDRPtable _table[201];

  const char *_className;

  bool _exitDeferred;

};

#endif

