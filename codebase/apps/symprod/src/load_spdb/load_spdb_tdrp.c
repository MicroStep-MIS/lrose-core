/*******************************************
 * load_spdb_tdrp.c
 *
 * TDRP C code file 'load_spdb' module.
 *
 * Code for program load_spdb
 *
 * This file has been automatically
 * generated by TDRP, do not modify.
 *
 *******************************************/

#include "load_spdb_tdrp.h"
#include <string.h>

/*
 * file scope variables
 */

static TDRPtable Table[13];
static load_spdb_tdrp_struct *Params;
static char *Module = "load_spdb";

/*************************************************************
 * load_spdb_tdrp_load_from_args()
 *
 * Loads up TDRP using the command line args.
 *
 * Check TDRP_usage() for command line actions associated with
 * this function.
 *
 *   argc, argv: command line args
 *
 *   load_spdb_tdrp_struct *params: loads up this struct
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

int load_spdb_tdrp_load_from_args(int argc, char **argv,
                         load_spdb_tdrp_struct *params,
                         char **override_list,
                         char **params_path_p)
{
  Params = params;
  load_spdb_tdrp_init(Params);
  if (tdrpLoadFromArgs(argc, argv,
                       Table, Params,
                       override_list, params_path_p)) {
    return (-1);
  } else {
    return (0);
  }
}

/*************************************************************
 * load_spdb_tdrp_load()
 *
 * Loads up TDRP for a given module.
 *
 * This version of load gives the programmer the option to load
 * up more than one module for a single application. It is a
 * lower-level routine than load_spdb_tdrp_load_from_args,
 * and hence more flexible, but the programmer must do more work.
 *
 *   char *param_file_path: the parameter file to be read in.
 *
 *   load_spdb_tdrp_struct *params: loads up this struct
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

int load_spdb_tdrp_load(char *param_file_path,
               load_spdb_tdrp_struct *params,
               char **override_list,
               int expand_env, int debug)
{
  Params = params;
  load_spdb_tdrp_init(Params);
  if (tdrpLoad(param_file_path, Table,
               params, override_list,
               expand_env, debug)) {
    return (-1);
  } else {
    return (0);
  }
}

/*************************************************************
 * load_spdb_tdrp_load_defaults()
 *
 * Loads up defaults for a given module.
 *
 * See load_spdb_tdrp_load() for more details.
 *
 *  Returns 0 on success, -1 on failure.
 */

int load_spdb_tdrp_load_defaults(load_spdb_tdrp_struct *params,
                        int expand_env)
{
  Params = params;
  load_spdb_tdrp_init(Params);
  if (tdrpLoad(NULL, Table,
               params, NULL,
               expand_env, FALSE)) {
    return (-1);
  } else {
    return (0);
  }
}

/*************************************************************
 * load_spdb_tdrp_sync()
 *
 * Syncs the user struct data back into the parameter table,
 * in preparation for printing.
 */

void load_spdb_tdrp_sync(void)
{
  tdrpUser2Table(Table, Params);
}

/*************************************************************
 * load_spdb_tdrp_print()
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

void load_spdb_tdrp_print(FILE *out, tdrp_print_mode_t mode)
{
  tdrpPrint(out, Table, Module, mode);
}

/*************************************************************
 * load_spdb_tdrp_check_all_set()
 *
 * Return TRUE if all set, FALSE if not.
 *
 * If out is non-NULL, prints out warning messages for those
 * parameters which are not set.
 */

int load_spdb_tdrp_check_all_set(FILE *out)
{
  return (tdrpCheckAllSet(out, Table, Params));
}

/*************************************************************
 * load_spdb_tdrp_check_is_set()
 *
 * Return TRUE if parameter is set, FALSE if not.
 *
 */

int load_spdb_tdrp_check_is_set(char *param_name)
{
  return (tdrpCheckIsSet(param_name, Table, Params));
}

/*************************************************************
 * load_spdb_tdrp_free_all()
 *
 * Frees up all TDRP dynamic memory.
 */

void load_spdb_tdrp_free_all(void)
{
  tdrpFreeAll(Table, Params);
}

/*************************************************************
 * load_spdb_tdrp_array_realloc()
 *
 * Realloc 1D array.
 *
 * If size is increased, the values from the last array entry is
 * copied into the new space.
 *
 * Returns 0 on success, -1 on error.
 */

int load_spdb_tdrp_array_realloc(char *param_name, int new_array_n)
{
  if (tdrpArrayRealloc(Table, Params, param_name, new_array_n)) {
    return (-1);
  } else {
    return (0);
  }
}

/*************************************************************
 * load_spdb_tdrp_array2D_realloc()
 *
 * Realloc 2D array.
 *
 * If size is increased, the values from the last array entry is
 * copied into the new space.
 *
 * Returns 0 on success, -1 on error.
 */

int load_spdb_tdrp_array2D_realloc(char *param_name,
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
 * load_spdb_tdrp_table()
 *
 * Returns pointer to static Table for this module.
 */

TDRPtable *load_spdb_tdrp_table(void)
{
  return (Table);
}

/*************************************************************
 * load_spdb_tdrp_init()
 *
 * Module table initialization function.
 *
 *
 * Returns pointer to static Table for this module.
 */

TDRPtable *load_spdb_tdrp_init(load_spdb_tdrp_struct *params)

{

  TDRPtable *tt = Table;

  load_spdb_tdrp_struct pp; /* for computing byte_offsets */

  /* zero out struct, and store size */

  memset(params, 0, sizeof(load_spdb_tdrp_struct));
  params->struct_size = sizeof(load_spdb_tdrp_struct);

  /* Parameter 'Comment 0' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = COMMENT_TYPE;
  tt->param_name = tdrpStrDup("Comment 0");
  tt->comment_hdr = tdrpStrDup("load_spdb allows you to load an SPDB data base from ASCII data. The binary structure stored is specified in the parameter file in terms of binary word lengths and offsets.");
  tt->comment_text = tdrpStrDup("");
  tt++;
  
  /* Parameter 'debug' */
  /* ctype is 'load_spdb_debug' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = ENUM_TYPE;
  tt->param_name = tdrpStrDup("debug");
  tt->descr = tdrpStrDup("Debug option");
  tt->help = tdrpStrDup("If set, debug messages will be printed appropriately");
  tt->val_offset = (char *) &(pp.debug) - (char *) &pp;
  tt->enum_def.name = tdrpStrDup("debug");
  tt->enum_def.nfields = 4;
  tt->enum_def.fields = (enum_field_t *)
      tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
    tt->enum_def.fields[0].name = tdrpStrDup("DEBUG_OFF");
    tt->enum_def.fields[0].val = DEBUG_OFF;
    tt->enum_def.fields[1].name = tdrpStrDup("DEBUG_WARNINGS");
    tt->enum_def.fields[1].val = DEBUG_WARNINGS;
    tt->enum_def.fields[2].name = tdrpStrDup("DEBUG_NORM");
    tt->enum_def.fields[2].val = DEBUG_NORM;
    tt->enum_def.fields[3].name = tdrpStrDup("DEBUG_EXTRA");
    tt->enum_def.fields[3].val = DEBUG_EXTRA;
  tt->single_val.e = DEBUG_OFF;
  tt++;
  
  /* Parameter 'malloc_debug_level' */
  /* ctype is 'long' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = LONG_TYPE;
  tt->param_name = tdrpStrDup("malloc_debug_level");
  tt->descr = tdrpStrDup("Malloc debug level");
  tt->help = tdrpStrDup("0 - none, 1 - corruption checking, 2 - records all malloc blocks and checks, 3 - printout of all mallocs etc.");
  tt->val_offset = (char *) &(pp.malloc_debug_level) - (char *) &pp;
  tt->has_min = TRUE;
  tt->has_max = TRUE;
  tt->min_val.l = 0;
  tt->max_val.l = 3;
  tt->single_val.l = 0;
  tt++;
  
  /* Parameter 'data_base_dir' */
  /* ctype is 'char*' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = STRING_TYPE;
  tt->param_name = tdrpStrDup("data_base_dir");
  tt->descr = tdrpStrDup("Directory for data base");
  tt->help = tdrpStrDup("");
  tt->val_offset = (char *) &(pp.data_base_dir) - (char *) &pp;
  tt->single_val.s = tdrpStrDup("none");
  tt++;
  
  /* Parameter 'product_file_path' */
  /* ctype is 'char*' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = STRING_TYPE;
  tt->param_name = tdrpStrDup("product_file_path");
  tt->descr = tdrpStrDup("Text file path from which to read the product data");
  tt->help = tdrpStrDup("This is a text file which holds product data line-by-line.");
  tt->val_offset = (char *) &(pp.product_file_path) - (char *) &pp;
  tt->single_val.s = tdrpStrDup("none");
  tt++;
  
  /* Parameter 'product_label' */
  /* ctype is 'char*' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = STRING_TYPE;
  tt->param_name = tdrpStrDup("product_label");
  tt->descr = tdrpStrDup("Product label string");
  tt->help = tdrpStrDup("This label is stored in the product data base.");
  tt->val_offset = (char *) &(pp.product_label) - (char *) &pp;
  tt->single_val.s = tdrpStrDup("none");
  tt++;
  
  /* Parameter 'product_id' */
  /* ctype is 'long' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = LONG_TYPE;
  tt->param_name = tdrpStrDup("product_id");
  tt->descr = tdrpStrDup("Product ID");
  tt->help = tdrpStrDup("This ID is used for identifying the product, specifying transformation routines, etc.");
  tt->val_offset = (char *) &(pp.product_id) - (char *) &pp;
  tt->single_val.l = 10000;
  tt++;
  
  /* Parameter 'load_mode' */
  /* ctype is 'load_spdb_load_mode' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = ENUM_TYPE;
  tt->param_name = tdrpStrDup("load_mode");
  tt->descr = tdrpStrDup("Load mode");
  tt->help = tdrpStrDup("If LOAD_ONCE, a chunk at a given time can only be stored once - a second attempt causes and error. If LOAD_OVERWRITE, a new chunk at a given time will overwrite and old one. If LOAD_ADD, new chunks will be added to the list at a given time.");
  tt->val_offset = (char *) &(pp.load_mode) - (char *) &pp;
  tt->enum_def.name = tdrpStrDup("load_mode");
  tt->enum_def.nfields = 3;
  tt->enum_def.fields = (enum_field_t *)
      tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
    tt->enum_def.fields[0].name = tdrpStrDup("LOAD_ONCE");
    tt->enum_def.fields[0].val = LOAD_ONCE;
    tt->enum_def.fields[1].name = tdrpStrDup("LOAD_OVERWRITE");
    tt->enum_def.fields[1].val = LOAD_OVERWRITE;
    tt->enum_def.fields[2].name = tdrpStrDup("LOAD_ADD");
    tt->enum_def.fields[2].val = LOAD_ADD;
  tt->single_val.e = LOAD_ADD;
  tt++;
  
  /* Parameter 'string_len' */
  /* ctype is 'long' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = LONG_TYPE;
  tt->param_name = tdrpStrDup("string_len");
  tt->descr = tdrpStrDup("Max length of any string in the input file");
  tt->help = tdrpStrDup("If an input string exceeds this length, it will be truncated.");
  tt->val_offset = (char *) &(pp.string_len) - (char *) &pp;
  tt->single_val.l = 32;
  tt++;
  
  /* Parameter 'data_format' */
  /* ctype is 'char*' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = STRING_TYPE;
  tt->param_name = tdrpStrDup("data_format");
  tt->descr = tdrpStrDup("Format template - specifies the product format in the text file");
  tt->help = tdrpStrDup("The template specifies the order and type of the fields in the input file. Legal values are 'int', 'float' and 'string'. Strings must be shorter than max_string_len. Multi-word strings must be separated by underscores. The fields in the data line must be delimited by spaces or tabs.");
  tt->val_offset = (char *) &(pp.data_format.val) - (char *) &pp;
  tt->len_offset = (char *) &(pp.data_format.len) - (char *) &pp;
  tt->array_offset = (char *) &(pp._data_format) - (char *) &pp;
  tt->array_n_offset = (char *) &(pp.data_format_n) - (char *) &pp;
  tt->is_array = TRUE;
  tt->array_len_fixed = FALSE;
  tt->array_elem_size = sizeof(char*);
  tt->array_n = 1;
  tt->array_vals = (tdrpVal_t *)
      tdrpMalloc(tt->array_n * sizeof(tdrpVal_t));
    tt->array_vals[0].s = tdrpStrDup("float");
  tt++;
  
  /* Parameter 'date_time_format' */
  /* ctype is 'load_spdb_date_time_format' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = ENUM_TYPE;
  tt->param_name = tdrpStrDup("date_time_format");
  tt->descr = tdrpStrDup("Format of the date/time field");
  tt->help = tdrpStrDup("Each data line must start with valid time, either as unix_time or as year month day hour min sec. This parameter specifies the format.");
  tt->val_offset = (char *) &(pp.date_time_format) - (char *) &pp;
  tt->enum_def.name = tdrpStrDup("date_time_format");
  tt->enum_def.nfields = 2;
  tt->enum_def.fields = (enum_field_t *)
      tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
    tt->enum_def.fields[0].name = tdrpStrDup("UNIX_TIME");
    tt->enum_def.fields[0].val = UNIX_TIME;
    tt->enum_def.fields[1].name = tdrpStrDup("YR_MO_DAY_HR_MIN_SEC");
    tt->enum_def.fields[1].val = YR_MO_DAY_HR_MIN_SEC;
  tt->single_val.e = UNIX_TIME;
  tt++;
  
  /* Parameter 'product_lifetime' */
  /* ctype is 'long' */
  
  memset(tt, 0, sizeof(TDRPtable));
  tt->ptype = LONG_TYPE;
  tt->param_name = tdrpStrDup("product_lifetime");
  tt->descr = tdrpStrDup("Number of secs for which product is valid.");
  tt->help = tdrpStrDup("The expire time is computed by adding this to the valid time.");
  tt->val_offset = (char *) &(pp.product_lifetime) - (char *) &pp;
  tt->single_val.l = 0;
  tt++;
  
  /* trailing entry has param_name set to NULL */
  
  tt->param_name = NULL;
  
  return (Table);

}
