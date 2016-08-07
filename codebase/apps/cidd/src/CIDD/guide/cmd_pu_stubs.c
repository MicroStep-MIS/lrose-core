/*
 * cmd_pu_stubs.c - Notify and event callback function stubs.
 * This file was generated by `gxv' from `cmd_pu.G'.
 */

#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/textsw.h>
#include <xview/xv_xrect.h>
#include "cidd_gui.h"


/*
 * Notify callback function for `cmd_st'.
 */
void
cmd_proc(Panel_item item, int value, Event *event)
{
	cmd_pu_cmd_pu_objects *ip = (cmd_pu_cmd_pu_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	fprintf(stderr, "cidd_gui: cmd_proc: value: %u\n", value);
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

}