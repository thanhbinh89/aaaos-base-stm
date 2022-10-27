#include "aaa.h"
#include "sys_io.h"
#include "u8g_arm.h"

uint8_t control = 0;
/*========================================================================*/
/*
  The following delay procedures must be implemented for u8glib

  void u8g_Delay(uint16_t val)		Delay by "val" milliseconds
  void u8g_MicroDelay(void)		Delay be one microsecond
  void u8g_10MicroDelay(void)	Delay by 10 microseconds

*/

void u8g_Delay(uint16_t val) {
	vTaskDelay(val);
}

void u8g_MicroDelay(void) {
	delayBlockingUs(1);
}

void u8g_10MicroDelay(void) {
	delayBlockingUs(10);
}

/*************************************************************************************/
uint8_t u8g_com_hw_spi_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr) {
	switch(msg) {
	case U8G_COM_MSG_STOP:
		break;

	case U8G_COM_MSG_INIT:
		break;

	case U8G_COM_MSG_ADDRESS: {
		if (arg_val == 0) {
			control = 0xF8;
		}
		else {
			control = 0xFA;
		}
		u8g_10MicroDelay();
	}
		break;

	case U8G_COM_MSG_RESET:
		setLcdRst(arg_val ? true : false);
		break;

	case U8G_COM_MSG_WRITE_BYTE:
		lcd_out(arg_val, control);
		u8g_MicroDelay();
		break;

	case U8G_COM_MSG_WRITE_SEQ:
	case U8G_COM_MSG_WRITE_SEQ_P: {
		uint8_t *ptr = (uint8_t*)arg_ptr;

		setLcdCs(true);
		spi_out_lcd_128x64_ap(control);
		while( arg_val > 0 ) {
			spi_out_lcd_128x64_ap(*ptr & 0xF0);
			spi_out_lcd_128x64_ap(*ptr << 4);
			ptr++;
			arg_val--;
		}
		setLcdCs(false);
	}
		break;

	case U8G_DEV_MSG_SLEEP_ON:
		setLcdBl(true);
		break;

	case U8G_DEV_MSG_SLEEP_OFF:
		setLcdBl(false);
		break;

	default:
		break;

	}

	return 1;
}

/******************************************************************************/
void spi_out_lcd_128x64_ap (uint8_t Data_LCD12864) {
	tranferLcdData(Data_LCD12864);
}

/**********************************************************************************/
void lcd_out (uint8_t Data_LCD, uint8_t Np_LCD) {
	setLcdCs(true);
	tranferLcdData(Np_LCD);
	tranferLcdData(Data_LCD & 0xF0);
	tranferLcdData(Data_LCD << 4);
	setLcdCs(false);
}
