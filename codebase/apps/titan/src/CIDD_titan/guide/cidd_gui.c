/*
 * cidd_gui.c - Contains main() for project cidd_gui
 * This file was generated by `gxv'.
 */

#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/textsw.h>
#include <xview/xv_xrect.h>
#include "page_pu_ui.h"
#include "fields_pu_ui.h"
#include "h_win_ui.h"
#include "movie_pu_ui.h"
#include "v_win_ui.h"
#include "data_pu_ui.h"
#include "zoom_pu_ui.h"
#include <group.h>
#include "save_pu_ui.h"
#include "over_pu_ui.h"
#include "status_pu_ui.h"
#include "draw_pu_ui.h"
#include "prod_pu_ui.h"
#include "fcast_pu_ui.h"
#include "route_pu_ui.h"
#include "gen_time_pu_ui.h"
#include "past_pu_ui.h"
#include "cmd_pu_ui.h"


/*
 * External variable declarations.
 */
h_win_horiz_bw_objects	*H_win_horiz_bw;
page_pu_page_pu_objects	*Extras_pu_page_pu;
fields_pu_fields_pu_objects	*Fields_pu_fields_pu;
movie_pu_movie_pu_objects	*Movie_pu_movie_pu;
v_win_v_win_pu_objects	*V_win_v_win_pu;
data_pu_data_pu_objects	*Data_pu_data_pu;
zoom_pu_zoom_pu_objects	*Zoom_pu_zoom_pu;
save_pu_save_im_pu_objects	*Save_pu_save_im_pu;
over_pu_over_pu_objects	*Over_pu_over_pu;
page_pu_page_pu_objects	*Page_pu_page_pu;
zoom_pu_bookmk_pu_objects	*Zoom_pu_bookmk_pu;
status_pu_status_pu_objects	*Status_pu_status_pu;
draw_pu_draw_pu_objects	*Draw_pu_draw_pu;
prod_pu_prod_pu_objects	*Prod_pu_prod_pu;
fcast_pu_fcast_pu_objects	*Fcast_pu_fcast_pu;
route_pu_route_pu_objects	*Route_pu_route_pu;
gen_time_pu_popup1_objects	*Gen_time_pu_popup1;
past_pu_past_pu_objects	*Past_pu_past_pu;
cmd_pu_cmd_pu_objects	*Cmd_pu_cmd_pu;

#ifdef MAIN

/*
 * Instance XV_KEY_DATA key.  An instance is a set of related
 * user interface objects.  A pointer to an object's instance
 * is stored under this key in every object.  This must be a
 * global variable.
 */
Attr_attribute	INSTANCE;

/*
 * main for project cidd_gui
 */
void
main(argc, argv)
	int		argc;
	char		**argv;
{
	/*
	 * Initialize XView.
	 */
	xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, NULL);
	INSTANCE = xv_unique_key();
	
	/*
	 * Initialize user interface components.
	 * Do NOT edit the object initializations by hand.
	 */
	H_win_horiz_bw = h_win_horiz_bw_objects_initialize(NULL, NULL);
	Extras_pu_page_pu = page_pu_page_pu_objects_initialize(NULL, NULL);
	Page_pu_page_pu = page_pu_page_pu_objects_initialize(NULL, NULL);
	Fields_pu_fields_pu = fields_pu_fields_pu_objects_initialize(NULL, NULL);
	Movie_pu_movie_pu = movie_pu_movie_pu_objects_initialize(NULL, NULL);
	V_win_v_win_pu = v_win_v_win_pu_objects_initialize(NULL, NULL);
	Data_pu_data_pu = data_pu_data_pu_objects_initialize(NULL, NULL);
	Zoom_pu_zoom_pu = zoom_pu_zoom_pu_objects_initialize(NULL, NULL);
	Zoom_pu_bookmk_pu = zoom_pu_bookmk_pu_objects_initialize(NULL, NULL);
	Save_pu_save_im_pu = save_pu_save_im_pu_objects_initialize(NULL, NULL);
	Over_pu_over_pu = over_pu_over_pu_objects_initialize(NULL, NULL);
	Status_pu_status_pu = status_pu_status_pu_objects_initialize(NULL, NULL);
	Draw_pu_draw_pu = draw_pu_draw_pu_objects_initialize(NULL, NULL);
	Prod_pu_prod_pu = prod_pu_prod_pu_objects_initialize(NULL, NULL);
	Fcast_pu_fcast_pu = fcast_pu_fcast_pu_objects_initialize(NULL, NULL);
	Status_pu_status_pu = status_pu_status_pu_objects_initialize(NULL, NULL);
	Route_pu_route_pu = route_pu_route_pu_objects_initialize(NULL, NULL);
	Gen_time_pu_popup1 = gen_time_pu_popup1_objects_initialize(NULL, NULL);
	Past_pu_past_pu = past_pu_past_pu_objects_initialize(NULL, NULL);
	Cmd_pu_cmd_pu = cmd_pu_cmd_pu_objects_initialize(NULL, NULL);
	
	/*
	 * Turn control over to XView.
	 */
	Status_pu_status_pu = status_pu_status_pu_objects_initialize(NULL, NULL);
	xv_main_loop(H_win_horiz_bw->horiz_bw);
	exit(0);
}

#endif
