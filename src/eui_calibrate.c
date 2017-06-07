/*
 * ES5932517 firmware
 *
 * Copyright (c) Antti Partanen 2014
 */

/******************************************************************************/
#include "eui_calibrate.h"


/******************************************************************************/
extern float ui_touch_cal_x;
extern float ui_touch_cal_y;
extern float ui_touch_cal_ppx;
extern float ui_touch_cal_ppy;
extern ui_coord_t ui_screen_w;
extern ui_coord_t ui_screen_h;
extern ui_coord_t ui_touch_x;
extern ui_coord_t ui_touch_y;
extern uint8_t ui_touch_state;
extern int16_t ui_touch_raw_x;
extern int16_t ui_touch_raw_y;
extern ui_context_t ui_context;

static int ui_calibrate_state = 0;


/******************************************************************************/
static void _ui_calibrate_start()
{
	ui_status_off();

	ui_set_bg24(&ui_context, ClrBlack);
	ui_set_fg24(&ui_context, ClrBlack);
	ui_clear_display(&ui_context);
	ui_set_fg24(&ui_context, ClrWhite);

	//ui_set_font(&ui_context, &g_sFontCm24);
	ui_draw_str_centered(&ui_context, "Touchscreen calibration:", -1, ui_screen_w / 2, ui_screen_h / 2 - 20, 0);
	//ui_set_font(&ui_context, &g_sFontCm14);
	ui_draw_str_centered(&ui_context, "Tap the center off each circle on each corner.", -1, ui_screen_w / 2, ui_screen_h / 2, 0);

	ui_draw_circle(&ui_context, 32, 32, 20);
	ui_draw_circle(&ui_context, 32, ui_screen_h - 33, 20);
	ui_draw_circle(&ui_context, ui_screen_w - 33, 32, 20);
	ui_draw_circle(&ui_context, ui_screen_w - 33, ui_screen_h - 33, 20);
}


/******************************************************************************/
void ui_calibrate_init()
{

}


/******************************************************************************/
int ui_calibrate_do()
{
	static ui_coord_t tl_x = -1, bl_x = -1, tr_x = -1, br_x = -1;
	static ui_coord_t tl_y = -1, bl_y = -1, tr_y = -1, br_y = -1;
	char str[128];

	if (ui_calibrate_state == 0)
	{
		_ui_calibrate_start();
		ui_calibrate_state++;
	}
	else if (ui_calibrate_state == 1)
	{
		sprintf(str, "    %04d x %04d    ", ui_touch_raw_x, ui_touch_raw_y);
		ui_set_bg24(&ui_context, ClrBlack);
		ui_set_fg24(&ui_context, ClrWhite);
		ui_draw_str_centered(&ui_context, str, -1, ui_screen_w / 2, ui_screen_h / 2 + 20, 1);
		if (ui_touch_state == UI_FLAG_TOUCH_RELEASED)
		{
			ui_draw_str_centered(&ui_context, str, -1, ui_screen_w / 2, ui_screen_h / 2 + 40, 1);
			if (ui_touch_raw_x > 2800 && ui_touch_raw_y > 2800)   /* top left */
			{
				tl_x = ui_touch_raw_x;
				tl_y = ui_touch_raw_y;
				ui_fill_circle(&ui_context, 32, 32, 20);
			}
			else if (ui_touch_raw_x > 2800 && ui_touch_raw_y < 1400)     /* bot left */
			{
				bl_x = ui_touch_raw_x;
				bl_y = ui_touch_raw_y;
				ui_fill_circle(&ui_context, 32, ui_screen_h - 33, 20);
			}
			else if (ui_touch_raw_x < 1400 && ui_touch_raw_y > 2800)     /* top right */
			{
				tr_x = ui_touch_raw_x;
				tr_y = ui_touch_raw_y;
				ui_fill_circle(&ui_context, ui_screen_w - 33, 32, 20);
			}
			else if (ui_touch_raw_x < 1400 && ui_touch_raw_y < 1400)     /* bot right */
			{
				br_x = ui_touch_raw_x;
				br_y = ui_touch_raw_y;
				ui_fill_circle(&ui_context, ui_screen_w - 33, ui_screen_h - 33, 20);
			}

			if (tl_x > -1 && tl_y > -1 && bl_x > -1 && bl_y > -1 &&
			        tr_x > -1 && tr_y > -1 && br_x > -1 && br_y > -1)
			{
				ui_calibrate_state = 2;
			}
		}
	}
	else if (ui_calibrate_state == 2)
	{
		ui_touch_cal_x = ((float)(tl_x + bl_x) / 2.0 + (float)(tr_x + br_x) / 2.0) / 2.0;
		ui_touch_cal_y = ((float)(tl_y + tr_y) / 2.0 + (float)(bl_y + br_y) / 2.0) / 2.0;
		ui_touch_cal_ppx = (((float)(tr_x - tl_x) + (float)(br_x - bl_x)) / 2.0) / (float)(ui_screen_w - 64);
		ui_touch_cal_ppy = (((float)(bl_y - tl_y) + (float)(br_y - tr_y)) / 2.0) / (float)(ui_screen_h - 64);

		ui_set_bg24(&ui_context, ClrBlack);
		ui_set_fg24(&ui_context, ClrBlack);
		ui_clear_display(&ui_context);
		ui_set_fg24(&ui_context, ClrWhite);

		//ui_set_font(&ui_context, &g_sFontCm24);
		ui_draw_str_centered(&ui_context, "Calibration finished", -1, ui_screen_w / 2, ui_screen_h / 2 - 20, 0);
		//ui_set_font(&ui_context, &g_sFontCm14);
		ui_draw_str_centered(&ui_context, "Test calibration by touching the screen.", -1, ui_screen_w / 2, ui_screen_h / 2, 0);

		ui_calibrate_state = 3;

	}
	else if (ui_calibrate_state == 3)
	{
		sprintf(str, "x: %d, y: %d, ppx: %d, ppy: %d", (int)ui_touch_cal_x, (int)ui_touch_cal_y, (int)(ui_touch_cal_ppx * 1000), (int)(ui_touch_cal_ppy * 1000));
		ui_set_bg24(&ui_context, ClrBlack);
		ui_set_fg24(&ui_context, ClrWhite);
		ui_draw_str_centered(&ui_context, str, -1, ui_screen_w / 2, ui_screen_h / 2 + 50, 1);
		sprintf(str, "x: %d, y: %d", ui_touch_x, ui_touch_y);
		ui_set_bg24(&ui_context, ClrBlack);
		ui_set_fg24(&ui_context, ClrWhite);
		ui_draw_str_centered(&ui_context, str, -1, ui_screen_w / 2, ui_screen_h / 2 + 20, 1);
		if (ui_touch_state == UI_FLAG_TOUCH_PRESSED)
		{
			ui_set_fg24(&ui_context, ClrGreen);
			ui_draw_circle(&ui_context, ui_touch_x, ui_touch_y, 10);
		}
		else if (ui_touch_state == UI_FLAG_TOUCH_RELEASED)
		{
			ui_set_fg24(&ui_context, ClrYellow);
			ui_draw_circle(&ui_context, ui_touch_x, ui_touch_y, 10);
		}
	}


	return 0;
}


