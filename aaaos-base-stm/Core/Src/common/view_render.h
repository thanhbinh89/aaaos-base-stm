#ifndef __VIEW_RENDER_H__
#define __VIEW_RENDER_H__

#include "view_item.h"

#include "u8g_arm.h"

#define	X_SIZE_FONT						(5)
#define	Y_SIZE_FONT						(7)

#define BACK_GND_STYLE_FILL				(0)
#define BACK_GND_STYLE_OUTLINE			(1)
#define BACK_GND_STYLE_NONE_OUTLINE		(2)

#define DATA_STYLE_TEXT					(0)
#define DATA_STYLE_NUMBER				(1)

#define DATA_STYLE_NONE_SETTING			(0)
#define DATA_STYLE_SETTING				(1)

typedef int (*view_render_item)(void*);

extern U8GLIB *view_render;

extern void view_render_init();
extern void view_render_reinit();
extern void view_render_display_off();
extern void view_render_display_on();

extern int view_render_screen(view_screen_t* screen);

#endif //__VIEW_RENDER_H__
