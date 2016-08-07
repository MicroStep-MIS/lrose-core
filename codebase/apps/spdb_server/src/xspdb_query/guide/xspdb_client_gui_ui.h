#ifndef	xspdb_client_gui_HEADER
#define	xspdb_client_gui_HEADER

/*
 * xspdb_client_gui_ui.h - User interface object and function declarations.
 * This file was generated by `gxv' from `xspdb_client_gui.G'.
 * DO NOT EDIT BY HAND.
 */

extern Attr_attribute	INSTANCE;


typedef struct {
	Xv_opaque	window1;
	Xv_opaque	controls1;
	Xv_opaque	now_bt;
	Xv_opaque	source_tx;
	Xv_opaque	prod_typ_st;
	Xv_opaque	prod_id_tx;
	Xv_opaque	req_typ_st;
	Xv_opaque	group1;
	Xv_opaque	end_tx;
	Xv_opaque	time_delta_ntx;
	Xv_opaque	min_msg;
	Xv_opaque	submit_bt;
	Xv_opaque	clear_bt;
	Xv_opaque	textpane1;
} xspdb_client_gui_window1_objects;

extern xspdb_client_gui_window1_objects	*xspdb_client_gui_window1_objects_initialize();

extern Xv_opaque	xspdb_client_gui_window1_window1_create();
extern Xv_opaque	xspdb_client_gui_window1_controls1_create();
extern Xv_opaque	xspdb_client_gui_window1_now_bt_create();
extern Xv_opaque	xspdb_client_gui_window1_source_tx_create();
extern Xv_opaque	xspdb_client_gui_window1_prod_typ_st_create();
extern Xv_opaque	xspdb_client_gui_window1_prod_id_tx_create();
extern Xv_opaque	xspdb_client_gui_window1_req_typ_st_create();
extern Xv_opaque	xspdb_client_gui_window1_group1_create();
extern Xv_opaque	xspdb_client_gui_window1_end_tx_create();
extern Xv_opaque	xspdb_client_gui_window1_time_delta_ntx_create();
extern Xv_opaque	xspdb_client_gui_window1_min_msg_create();
extern Xv_opaque	xspdb_client_gui_window1_submit_bt_create();
extern Xv_opaque	xspdb_client_gui_window1_clear_bt_create();
extern Xv_opaque	xspdb_client_gui_window1_textpane1_create();
#endif