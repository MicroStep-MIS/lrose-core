#ifndef	over_pu_HEADER
#define	over_pu_HEADER

/*
 * over_pu_ui.h - User interface object and function declarations.
 * This file was generated by `gxv' from `over_pu.G'.
 * DO NOT EDIT BY HAND.
 */

extern Attr_attribute	INSTANCE;


typedef struct {
	Xv_opaque	over_pu;
	Xv_opaque	over_cp;
	Xv_opaque	over_lst;
	Xv_opaque	over_pu_st;
} over_pu_over_pu_objects;

extern over_pu_over_pu_objects	*over_pu_over_pu_objects_initialize(over_pu_over_pu_objects *, Xv_opaque);

extern Xv_opaque	over_pu_over_pu_over_pu_create(over_pu_over_pu_objects *, Xv_opaque);
extern Xv_opaque	over_pu_over_pu_over_cp_create(over_pu_over_pu_objects *, Xv_opaque);
extern Xv_opaque	over_pu_over_pu_over_lst_create(over_pu_over_pu_objects *, Xv_opaque);
extern Xv_opaque	over_pu_over_pu_over_pu_st_create(over_pu_over_pu_objects *, Xv_opaque);
#endif