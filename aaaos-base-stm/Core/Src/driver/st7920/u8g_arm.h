#ifndef __U8G_ARM_H__
#define __U8G_ARM_H__

#include "U8glib/src/clib/u8g.h"
#include "U8glib/src/U8glib.h"

/*************************************************************************/
void spi_out_lcd_128x64_ap (uint8_t Data_LCD12864);
void lcd_out(uint8_t Data_LCD, uint8_t Np_LCD);
uint8_t u8g_com_hw_spi_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr);

#endif //__U8G_ARM_H__

