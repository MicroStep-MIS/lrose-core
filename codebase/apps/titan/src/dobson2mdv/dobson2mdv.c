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
/*********************************************************************
 * dobson2mdv.c
 *
 * Smooth the data in an mdv file, using either a mean or median
 * filter.
 *
 * Mike Dixon  RAP NCAR Boulder CO USA
 *
 * Oct 1993
 *
 **********************************************************************/

#define MAIN
#include "dobson2mdv.h"
#undef MAIN

#include <cidd/cdata_util.h>

int main(int argc, char **argv)

{

  /*
   * basic declarations
   */

  char *params_file_path = NULL;
  char **input_file_list;

  int i;
  int check_params;
  int print_params;
  int n_input_files = 0;
 
  path_parts_t progname_parts;
  vol_file_handle_t v_handle;
  tdrp_override_t override;

  /*
   * allocate space for the global structure
   */
  
  Glob = (global_t *)
    umalloc((ui32) sizeof(global_t));

  /*
   * set program name
   */
  
  uparse_path(argv[0], &progname_parts);
  Glob->prog_name = (char *)
    umalloc ((ui32) (strlen(progname_parts.base) + 1));
  strcpy(Glob->prog_name, progname_parts.base);
  
  /*
   * display ucopyright message
   */

  ucopyright(Glob->prog_name);

  /*
   * parse command line arguments
   */
  
  parse_args(argc, argv, &check_params, &print_params,
	     &params_file_path, &override,
             &n_input_files, &input_file_list);

   /*
   * load up parameters
   */
  
  Glob->table = dobson2mdv_tdrp_init(&Glob->params);

  if (FALSE == TDRP_read(params_file_path,
			 Glob->table,
			 &Glob->params,
			 override.list)) {
    fprintf(stderr, "ERROR - %s:main\n", Glob->prog_name);
    fprintf(stderr, "Cannot read params file '%s'\n",
	    params_file_path);
    tidy_and_exit(-1);
  }
  
  TDRP_free_override(&override);
  
  if (check_params) {
    TDRP_check_is_set(Glob->table, &Glob->params);
    tidy_and_exit(0);
  }
  
  if (print_params) {
    TDRP_print_params(Glob->table, &Glob->params, Glob->prog_name, TRUE);
    tidy_and_exit(0);
  }
  
  if (Glob->params.malloc_debug_level > 0)
    umalloc_debug(Glob->params.malloc_debug_level);
  
  /*
   * initialize index
   */

  RfInitVolFileHandle(&v_handle,
		      Glob->prog_name,
		      (char *) NULL,
		      (FILE *) NULL);

  /*
   * convert files in list
   */
  
  for (i = 0; i < n_input_files; i++) {
    convert_file(&v_handle, input_file_list[i]);
  }
  
  /*
   * quit
   */
  
  tidy_and_exit(0);

  return(0);

}

