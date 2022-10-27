#include <string.h>
#include "view_render.h"
#include "sys_io.h"

U8GLIB *view_render;

static int view_render_rectangle(void *rectangle);
static int view_render_dynamic(void *dynamic);

static view_render_item render_list[] =
	{ view_render_rectangle, view_render_dynamic };

void view_render_init() {
	view_render = new U8GLIB(&u8g_dev_st7920_128x64_hw_spi, u8g_com_hw_spi_fn);
}

void view_render_display_on() {
	setLcdBl(true);
}

void view_render_display_off() {
	setLcdBl(false);
}

int view_render_rectangle(void *rectangle) {
	uint8_t x, y, len;
	uint8_t x_forcus, y_forcus, w_forcus, h_forcus;

	view_rectangle_t *rect = (view_rectangle_t*) rectangle;
	if (rect == NULL) {
		return (-1);
	}

	/*paint back ground of object screen on lcd*/
	switch (rect->type) {
	case BACK_GND_STYLE_FILL:
		view_render->setDefaultForegroundColor();
		view_render->drawBox(rect->x, rect->y, rect->width, rect->height);
		view_render->setDefaultBackgroundColor();
		break;

	case BACK_GND_STYLE_OUTLINE:
		view_render->setDefaultBackgroundColor();
		view_render->drawBox(rect->x, rect->y, rect->width, rect->height);

		view_render->setDefaultForegroundColor();
		view_render->drawFrame(rect->x, rect->y, rect->width, rect->height);
		break;

	case BACK_GND_STYLE_NONE_OUTLINE:
		view_render->setDefaultBackgroundColor();
		view_render->drawBox(rect->x, rect->y, rect->width, rect->height);
		view_render->setDefaultForegroundColor();
		break;

	default:
		break;
	}

	len = strlen(rect->text);

	switch (rect->align_text) {
	case ITEM_TYPE_ALINE_LEFT:
		x = rect->x + 5;
		break;

	case ITEM_TYPE_ALINE_CENTER:
		x = rect->x + (rect->width - (len * X_SIZE_FONT * rect->font_size + len)) / 2;
		break;

	case ITEM_TYPE_ALINE_RIGHT:
		x = rect->x + rect->width - 5 - (len * X_SIZE_FONT * rect->font_size + len);
		break;

	default:
		x = rect->x + 5;
		break;
	}

	y = rect->y + (rect->height - rect->font_size * Y_SIZE_FONT) / 2;

	/*set font size for object screen - set cursor on lcd for object screen*/
	if (rect->font_size == 1) {
		view_render->setFont(u8g_font_5x7);
		view_render->drawStr(x, y + 7, rect->text);
	}
	else {
		view_render->setFont(u8g_font_7x14);
		view_render->drawStr(x, y + 14, rect->text);
	}

	/*paint back ground of object screen when setting*/
	if (rect->border_width != 0) {
		x_forcus = x + rect->focus_cursor * (X_SIZE_FONT * rect->font_size + 1) - 2;
		y_forcus = rect->y + 1;

		w_forcus = rect->focus_size * (X_SIZE_FONT * rect->font_size + 1) + 3;
		h_forcus = rect->height - 2;

		if (rect->type == BACK_GND_STYLE_FILL) {
			view_render->setDefaultBackgroundColor();
			view_render->drawFrame(x_forcus, y_forcus, w_forcus, h_forcus);
		}
		else {
			view_render->setDefaultForegroundColor();
			view_render->drawFrame(x_forcus, y_forcus, w_forcus, h_forcus);
		}
	}

	return 0;
}

int view_render_dynamic(void *dynamic) {
	((view_dynamic_t*) dynamic)->render();
	return 0;
}

int view_render_screen(view_screen_t *screen) {
	if ((view_item_t*) screen->item[screen->focus_item] != ITEM_NULL) {
		if (((view_item_t*) screen->item[screen->focus_item])->item_type == ITEM_TYPE_RECTANGLE) {
			((view_rectangle_t*) screen->item[screen->focus_item])->type = BACK_GND_STYLE_FILL;
		}
	}

	for (uint8_t i = 0; i < NUMBER_SCREEN_ITEMS_MAX; i++) {
		if ((view_item_t*) screen->item[i] != ITEM_NULL) {
			if (((view_item_t*) screen->item[i])->item_type == ITEM_TYPE_RECTANGLE) {
				if (i != screen->focus_item && ((view_rectangle_t*) screen->item[i])->type != BACK_GND_STYLE_NONE_OUTLINE) {
					((view_rectangle_t*) screen->item[i])->type = BACK_GND_STYLE_NONE_OUTLINE;
				}
			}
		}
	}

	view_render->firstPage();
	do {
		for (int i = 0; i < NUMBER_SCREEN_ITEMS_MAX; i++) {
			if ((view_item_t*) screen->item[i] != ITEM_NULL) {
				render_list[((view_item_t*) screen->item[i])->item_type]((view_item_t*) screen->item[i]);
			}
		}
	}
	while (view_render->nextPage());

	return 0;
}
