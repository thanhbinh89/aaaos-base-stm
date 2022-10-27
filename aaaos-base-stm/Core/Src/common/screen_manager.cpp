/**
 ******************************************************************************
 * @Author: ThanNT
 * @Date:   31/11/2016
 ******************************************************************************
**/
#include "screen_manager.h"

static scr_mng_t* screen_manager = SCREEN_MANAGER_NULL;
static view_screen_t* view_screen = VIEW_SCREEN_NULL;

void scr_mng_ctor(scr_mng_t* scr_mng, screen_f init_scr, view_screen_t* scr_obj) {
	view_screen = scr_obj;				/* point to current screen object */
	screen_manager = scr_mng;			/* init singleton screen manager */
	screen_manager->screen = init_scr;	/* assign init handler */
}

void scr_mng_dispatch(uint32_t sig, void *msg) {
	if (screen_manager == SCREEN_MANAGER_NULL) {
		return;
	}

	screen_manager->screen(sig, msg);
	view_render_screen(view_screen);
}

void scr_mng_tran(screen_f target,  view_screen_t* scr_obj) {
	if (screen_manager == SCREEN_MANAGER_NULL) {
		return;
	}

	view_screen->focus_item_after = view_screen->focus_item ;
	view_screen->focus_item_before = view_screen->focus_item;

	/* change new screen */
	view_screen = scr_obj;
	screen_manager->screen = target;

	/* entry new screen */
	view_render_screen(view_screen);
}

screen_f scr_mng_get_current_screen() {
	return screen_manager->screen;
}

view_screen_t* scr_mng_get_obj_screen() {
	return view_screen;
}
