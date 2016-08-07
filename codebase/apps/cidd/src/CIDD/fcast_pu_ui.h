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
#ifndef	fcast_pu_HEADER
#define	fcast_pu_HEADER

/*
 * fcast_pu_ui.h - User interface object and function declarations.
 * This file was generated by `gxv' from `fcast_pu.G'.
 * DO NOT EDIT BY HAND.
 */

extern Attr_attribute	INSTANCE;


typedef struct {
	Xv_opaque	fcast_pu;
	Xv_opaque	cp;
	Xv_opaque	fcast_st;
} fcast_pu_fcast_pu_objects;

extern fcast_pu_fcast_pu_objects	*fcast_pu_fcast_pu_objects_initialize(fcast_pu_fcast_pu_objects *, Xv_opaque);

extern Xv_opaque	fcast_pu_fcast_pu_fcast_pu_create(fcast_pu_fcast_pu_objects *, Xv_opaque);
extern Xv_opaque	fcast_pu_fcast_pu_cp_create(fcast_pu_fcast_pu_objects *, Xv_opaque);
extern Xv_opaque	fcast_pu_fcast_pu_fcast_st_create(fcast_pu_fcast_pu_objects *, Xv_opaque);
#endif